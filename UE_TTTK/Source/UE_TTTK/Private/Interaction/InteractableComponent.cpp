#include "Interaction/InteractableComponent.h"

#include "Net/UnrealNetwork.h"
#include "MainPlayer.h"
#include "MovieSceneTracksComponentTypes.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Particles/ParticleSystem.h"


UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	interactionGuideComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionGuide"));

	interactionRadius = 300.0f;
	if (interactionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionArea")))
	{
		interactionSphere->SetSphereRadius(interactionRadius);
		interactionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		interactionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		interactionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		interactionSphere->SetGenerateOverlapEvents(true);
	}
	
	ComponentTags.Add(TEXT("Interactable"));
}

void UInteractableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractableComponent, possessingPlayer)
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	clientState = EInteractableState::OutOfBound;

	if (feedbackSettings.IsWidgetOn() && feedbackSettings.interactionGuideWidgetClass && IsValid(interactionGuideComponent))
	{
		UUserWidget* widget = CreateWidget(GetWorld(), feedbackSettings.interactionGuideWidgetClass, FName("WidgetForGuide"));
		if (IsValid(widget))
		{
			interactionGuideComponent->SetWidget(widget);
			interactionGuideComponent->SetWidgetSpace(EWidgetSpace::World);
			interactionGuideComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			FAttachmentTransformRules attachRules(
			EAttachmentRule::SnapToTarget,  // Location
			EAttachmentRule::SnapToTarget,  // Rotation
			EAttachmentRule::KeepRelative,  // Scale
			false 
			);
			interactionGuideComponent->AttachToComponent(GetOwner()->GetRootComponent(), attachRules);
			interactionGuideComponent->SetRelativeLocation(feedbackSettings.widgetOffset);
			// true: world / false: parent => rotation controlled by player location(world)
			interactionGuideComponent->SetAbsolute(false, true, false);
			if (!interactionGuideComponent->IsRegistered()) {interactionGuideComponent->RegisterComponent();}
			interactionGuideComponent->SetVisibility(false);
		}
	}
	
	if (feedbackSettings.IsOutlineOn())
	{
		if (UpdateAvailablePrimitiveComponents())
		{
			feedbackSettings.outlinedMeshComponent->SetRenderCustomDepth(false);
			feedbackSettings.outlinedMeshComponent->SetCustomDepthStencilValue(0);
			feedbackSettings.outlinedMeshComponent->MarkRenderStateDirty();
		}
	}

	if (IsValid(interactionSphere))
	{
		interactionSphere->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		interactionSphere->RegisterComponent();
		
		interactionSphere->OnComponentBeginOverlap.AddDynamic(this, &UInteractableComponent::OnInteractionSphereBeginOverlap);
		interactionSphere->OnComponentEndOverlap.AddDynamic(this, &UInteractableComponent::OnInteractionSphereEndOverlap);
	}
}

void UInteractableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	clientState = EInteractableState::Default;
	Super::EndPlay(EndPlayReason);
}

void UInteractableComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (feedbackSettings.IsWidgetOn() && IsValid(interactionGuideComponent) && clientState == EInteractableState::Focused)
	{
		AActor* cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		interactionGuideComponent->SetWorldRotation(UKismetMathLibrary::MakeRotFromXZ(-cam->GetActorForwardVector(), cam->GetActorUpVector()));
	}
}

void UInteractableComponent::OutOfInteractableRange(APlayerController* playerController)
{
	if (!LIKELY(IsValid(playerController))) {return;}
	
	playerInRange = nullptr;
	clientState = EInteractableState::OutOfBound;
	onChangeState.Broadcast(playerController, EInteractableState::OutOfBound);
	Client_UpdateVisuals(playerController);
}

void UInteractableComponent::InInteractableRange(APlayerController* playerController)
{
	if (!LIKELY(IsValid(playerController))) {return;}
	
	playerInRange = playerController->GetPawn<AMainPlayer>();
	clientState = EInteractableState::InRange;
	onChangeState.Broadcast(playerController, EInteractableState::InRange);
	if (playerInRange->GetFocusedActor() == GetOwner())
	{
		TryActivateInteractable(playerController);
		return;
	}
	Client_UpdateVisuals(playerController);
}

void UInteractableComponent::TryDeactivateInteractable(APlayerController* playerController)
{
	if (!LIKELY(IsValid(playerController))) {return;}
	if (!EnumHasAnyFlags(clientState, EInteractableState::InRange | EInteractableState::Focused)) {return;}
	if (!playerController->IsLocalController()) {return;}
	
	clientState = EInteractableState::UnFocused;
	onChangeState.Broadcast(playerController, EInteractableState::UnFocused);
	Client_UpdateVisuals(playerController);
}

void UInteractableComponent::TryActivateInteractable(APlayerController* playerController)
{
	if (!LIKELY(IsValid(playerController))) {return;}
	if (!EnumHasAnyFlags(clientState, EInteractableState::InRange | EInteractableState::UnFocused)) {return;}
	if (!playerController->IsLocalController()) {return;}
	
	clientState = EInteractableState::Focused;
	onChangeState.Broadcast(playerController, EInteractableState::Focused);
	Client_UpdateVisuals(playerController);

	if (feedbackSettings.IsSoundOn()) {PlaySound(feedbackSettings.activatedSound);}

	return;
}

void UInteractableComponent::TryInteract(APlayerController* playerController)
{
	if (!LIKELY(IsValid(playerController))) {return;}
	if (clientState != EInteractableState::Focused) {return;}
	if (!playerController->IsLocalController()) {return;}
	
	clientState = EInteractableState::Interacting;
	onChangeState.Broadcast(playerController, EInteractableState::Interacting);
	Client_UpdateVisuals(playerController);

	if (feedbackSettings.IsSoundOn()) {PlaySound(feedbackSettings.interactedSound);}
	if (feedbackSettings.IsNiagaraOn()) {PlayEffect(feedbackSettings.interactedNiagaraVFX);}
	if (feedbackSettings.IsParticleOn()) {PlayEffect(feedbackSettings.interactedParticleVFX);}

	if (bPossessedByInteraction && !possessingPlayer)
	{
		//Server_TryInteract(playerController->GetPawn());
	}
}

void UInteractableComponent::Multicast_TryInteract_Implementation(APawn* player)
{
	onRequestInteraction.Broadcast(player);
}

void UInteractableComponent::PossessedByPlayer_Implementation(AMainPlayer* player)
{
	if (!IsValid(player)) {return;}
	possessingPlayer = player;
}

void UInteractableComponent::FinishInteracting(APlayerController* playerController, const EInteractableState& newState)
{
	if (!LIKELY(IsValid(playerController))) {return;}
	if (clientState != EInteractableState::Interacting) {return;}
	
	switch (newState)
	{
	case EInteractableState::InRange:
		InInteractableRange(playerController);
		return;
	case EInteractableState::OutOfBound:
		OutOfInteractableRange(playerController);
		return;
	default:
		clientState = newState;
		onChangeState.Broadcast(playerController, clientState);
		Client_UpdateVisuals(playerController);
		return;
	}
	
}

void UInteractableComponent::OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                             AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                             const FHitResult& SweepResult)
{
	AMainPlayer* player = Cast<AMainPlayer>(OtherActor);
	if (!IsValid(player)) {return;}
	
	APlayerController* pc = player->GetController<APlayerController>();
	if (IsValid(pc) && pc->IsLocalController())
	{
		if (playerInRange == player) {return;}
		InInteractableRange(pc);
	}
	
}

void UInteractableComponent::OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AMainPlayer* player = Cast<AMainPlayer>(OtherActor);
	if (!IsValid(player)) {return;}
	
	APlayerController* pc = player->GetController<APlayerController>();
	if (IsValid(pc) && pc->IsLocalController())
	{
		OutOfInteractableRange(pc);
	}
}

bool UInteractableComponent::UpdateAvailablePrimitiveComponents()
{
	AActor* owner = GetOwner();
	if (!IsValid(owner)) {return false;}
	TArray<UMeshComponent*> activeComponents;
	owner->GetComponents<UMeshComponent>(activeComponents);
	if (activeComponents.Num() == 0) {return false;}
	feedbackSettings.outlinedMeshComponent = activeComponents[0];
	UE_LOG(LogTemp, Warning, TEXT("Init Outline : %s"), *(feedbackSettings.outlinedMeshComponent.IsValid() ? feedbackSettings.outlinedMeshComponent->GetName() : "None"));
	return true;
}

void UInteractableComponent::Client_UpdateVisuals(APlayerController* playerController)
{
	if (!LIKELY(IsValid(playerController))) {return;}
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	if (pc != playerController) {return;}
	
	UpdateOutline();
	UpdateWidget();
}

void UInteractableComponent::UpdateOutline()
{
	UE_LOG(LogTemp, Warning, TEXT("Update Outline : %s"), *(feedbackSettings.outlinedMeshComponent.IsValid() ? feedbackSettings.outlinedMeshComponent->GetName() : "None"));
	if (!feedbackSettings.IsOutlineOn()) {return;}
	if (!feedbackSettings.outlinedMeshComponent.IsValid()) {return;}
	feedbackSettings.outlinedMeshComponent->SetRenderCustomDepth(clientState == EInteractableState::Focused);
	feedbackSettings.outlinedMeshComponent->SetCustomDepthStencilValue(
		clientState == EInteractableState::Focused ? feedbackSettings.outlineStencilValue : 0
	);
}

void UInteractableComponent::UpdateWidget()
{
	if (!feedbackSettings.IsWidgetOn()) {return;}
	if (!IsValid(interactionGuideComponent)) {return;}

	interactionGuideComponent->SetVisibility(clientState == EInteractableState::Focused);
}

void UInteractableComponent::PlaySound(USoundBase* sound)
{
	if (!IsValid(sound)) {return;}
	
	if (sound->IsPlayable())
	{
		UGameplayStatics::SpawnSoundAtLocation(GetWorld(), sound, GetOwner()->GetActorLocation());
	}
}

void UInteractableComponent::PlayEffect(UParticleSystem* effect)
{
	if (!IsValid(effect)) {return;}

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), effect, GetOwner()->GetActorLocation());
}

void UInteractableComponent::PlayEffect(UNiagaraSystem* effect)
{
	if (!IsValid(effect)) {return;}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), effect, GetOwner()->GetActorLocation());
}

