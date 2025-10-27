#include "Interaction/InteractableComponent.h"

#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Interaction/InteractionComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"


UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
		
	ComponentTags.Add(TEXT("Interactable"));

	bWantsInitializeComponent = true;
}

void UInteractableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void UInteractableComponent::InitializeComponent()
{
	Super::InitializeComponent();

	AActor* owner = GetOwner();
	if (!IsValid(owner)) { return; }

	interactionSphere = NewObject<USphereComponent>(owner, USphereComponent::StaticClass(), TEXT("InteractionSphere"));
	if (!IsValid(interactionSphere)) { return; }
	
	owner->AddInstanceComponent(interactionSphere);
	if (IsValid(owner->GetRootComponent()))
	{
		interactionSphere->AttachToComponent(owner->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		interactionSphere->SetSphereRadius(interactionRadius);
		interactionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		interactionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		interactionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		interactionSphere->SetGenerateOverlapEvents(true);
		interactionSphere->RegisterComponent();
		interactionSphere->OnComponentBeginOverlap.AddDynamic(this, &UInteractableComponent::OnInteractionSphereBeginOverlap);
		interactionSphere->OnComponentEndOverlap.AddDynamic(this, &UInteractableComponent::OnInteractionSphereEndOverlap);
	}
	
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
	
	clientState = EInteractableState::OutOfBound;

	if (IsValid(interactionSphere))
	{
		interactionSphere->SetSphereRadius(interactionRadius);
		interactionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		interactionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		interactionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}

	// MeshComponent 확인만 수행 (아웃라인 초기화 제거)
	UpdateAvailablePrimitiveComponents();
	
	if (feedbackSettings.IsWidgetOn() && feedbackSettings.interactionGuideWidgetClass)
	{
		interactionGuideComponent = NewObject<UWidgetComponent>(GetOwner(), UWidgetComponent::StaticClass(), TEXT("GuideWidgetComponent"));
		if (IsValid(interactionGuideComponent))
		{
			GetOwner()->AddInstanceComponent(interactionGuideComponent);
			
			UUserWidget* widget = CreateWidget(GetWorld(), feedbackSettings.interactionGuideWidgetClass, FName("WidgetForGuide"));
			if (IsValid(widget))
			{
				FAttachmentTransformRules attachRules(
				EAttachmentRule::SnapToTarget,  // Location
				EAttachmentRule::SnapToTarget,  // Rotation
				EAttachmentRule::KeepRelative,  // Scale
				false 
				);
				attachRules.bWeldSimulatedBodies = false;
				if (feedbackSettings.widgetSocketName != NAME_None)
				{
					TArray<USceneComponent*> components;
					GetOwner()->GetComponents(components);
					for (USceneComponent* component : components)
					{
						if (component->DoesSocketExist(feedbackSettings.widgetSocketName))
						{
							interactionGuideComponent->AttachToComponent(component, attachRules, feedbackSettings.widgetSocketName);
							break;
						}
					}
				}
				if (interactionGuideComponent->GetAttachSocketName() == NAME_None)
				{
					interactionGuideComponent->AttachToComponent(GetOwner()->GetRootComponent(), attachRules);	
				}
				// true: world / false: parent => rotation controlled by player location(world)
				interactionGuideComponent->SetWidget(widget);
				interactionGuideComponent->SetWidgetSpace(EWidgetSpace::World);
				interactionGuideComponent->SetAbsolute(false, true, true);
				interactionGuideComponent->RegisterComponent();

				interactionGuideComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				interactionGuideComponent->SetVisibility(false);
				interactionGuideComponent->SetCastShadow(false);
				interactionGuideComponent->SetReceivesDecals(false);
			}
		}
	}

	if (feedbackSettings.IsNetworkOn())
	{
	}
}

void UInteractableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	interactionSphere->OnComponentBeginOverlap.Clear();
	interactionSphere->OnComponentEndOverlap.Clear();
	playerInRange = nullptr;
	TryChangeState(GetWorld()->GetFirstPlayerController(), EInteractableState::Default);
	OnClientInteraction.Clear();
	OnMultiInteraction.Clear();
	Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UInteractableComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UInteractableComponent, interactionRadius))
	{
		if (IsValid(interactionSphere))
		{
			interactionSphere->SetSphereRadius(interactionRadius);
		}
	}
}
#endif //WITH_EDITOR

void UInteractableComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (feedbackSettings.IsWidgetOn() && IsValid(interactionGuideComponent) && clientState == EInteractableState::Focused)
	{
		AActor* cam = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
		if (!IsValid(cam)) {return;}
		interactionGuideComponent->SetWorldRotation(UKismetMathLibrary::MakeRotFromXZ(-cam->GetActorForwardVector(), cam->GetActorUpVector()));
	}
}

void UInteractableComponent::TryChangeState(APlayerController* playerController, EInteractableState newState)
{
	if (!LIKELY(IsValid(playerController))) {return;}
	if (!playerController->IsLocalController()) {return;}
	if (clientState == EInteractableState::Default) {return;}
	
	switch (newState)
	{
	case EInteractableState::Default:
		clientState = EInteractableState::Default;
		break;
		
	case EInteractableState::UnFocused:
	case EInteractableState::InRange:
		{
			if (!IsValid(playerInRange)) {return TryChangeState(playerController, EInteractableState::OutOfBound);}
			UInteractionComponent* interactionComp = playerInRange->FindComponentByTag<UInteractionComponent>("Interaction");
			if (IsValid(interactionComp))
			{
				clientState = EInteractableState::InRange;			
			}
			if (interactionComp->GetFocusedActor() != GetOwner()) {break;}
		}

	case EInteractableState::Focused:
		{
			if (!EnumHasAnyFlags(clientState, EInteractableState::InRange)) {return;}
			clientState = EInteractableState::Focused;
			break;
		}
	
	case EInteractableState::OutOfBound:
		{
			playerInRange = nullptr;
			clientState = EInteractableState::OutOfBound;
			break;
		}
	}

	// 상태 변경 이벤트 브로드캐스트
	if (OnStateChanged.IsBound())
	{
		OnStateChanged.Broadcast(playerController, clientState);
	}

	UpdateVisuals(playerController);
}

void UInteractableComponent::TryInteract(APlayerController* playerController)
{
	if (!LIKELY(IsValid(playerController))) {return;}
	if (!playerController->IsLocalController()) {return;}
	if (clientState != EInteractableState::Focused) {return;}

	if (OnClientInteraction.IsBound())
	{
		OnClientInteraction.Broadcast(playerController);
		PlayEffects(true);
	}
}

void UInteractableComponent::Multicast_TryInteract_Implementation(APawn* player)
{
	if (!IsValid(player) || !feedbackSettings.IsNetworkOn()) {return;}
	if (OnMultiInteraction.IsBound())
	{
		OnMultiInteraction.Broadcast(player);
		PlayEffects(true);
		if (APlayerController* pc = player->GetController<APlayerController>())
		{
			FinishInteracting(pc, true);
		}
	}
	
}

void UInteractableComponent::FinishInteracting(APlayerController* playerController, bool bSuccess)
{
	if (!LIKELY(IsValid(playerController))) {return;}
	if (!playerController->IsLocalController()) {return;}
	
	if (bSuccess)
	{
		TryChangeState(playerController, EInteractableState::OutOfBound);
		return;
	}
	TryChangeState(playerController, EInteractableState::InRange);
}

void UInteractableComponent::OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                             AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                             const FHitResult& SweepResult)
{
	APawn* player = Cast<APawn>(OtherActor);
	if (!IsValid(player)) {return;}
	
	APlayerController* pc = player->GetController<APlayerController>();
	if (IsValid(pc) && pc->IsLocalController())
	{
		if (playerInRange == player) {return;}
		playerInRange = player;
		TryChangeState(pc, EInteractableState::InRange);
	}
	
}

void UInteractableComponent::OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* player = Cast<APawn>(OtherActor);
	if (!IsValid(player)) {return;}
	
	APlayerController* pc = player->GetController<APlayerController>();
	if (IsValid(pc) && pc->IsLocalController())
	{
		playerInRange = nullptr;
		TryChangeState(pc, EInteractableState::OutOfBound);
	}
}

bool UInteractableComponent::UpdateAvailablePrimitiveComponents()
{
	if (feedbackSettings.ownerMeshComponent.IsValid()) {return true;}
	
	AActor* owner = GetOwner();
	if (!IsValid(owner)) {return false;}
	
	TArray<UMeshComponent*> activeComponents;
	owner->GetComponents<UMeshComponent>(activeComponents);
	if (activeComponents.Num() == 0) {return false;}
	
	feedbackSettings.ownerMeshComponent = activeComponents[0];
	return feedbackSettings.ownerMeshComponent.IsValid();
}

void UInteractableComponent::UpdateVisuals(APlayerController* playerController)
{
	if (!LIKELY(IsValid(playerController))) {return;}
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	if (pc != playerController) {return;}

	bool bShouldShowOutline = (clientState == EInteractableState::Focused);

	// 커스텀 머티리얼 아웃라인
	if (feedbackSettings.IsOutlineOn())
	{
		UpdateCustomOutline(bShouldShowOutline);
	}

	// Widget
	if (feedbackSettings.IsWidgetOn() && IsValid(interactionGuideComponent))
	{
		interactionGuideComponent->SetVisibility(bShouldShowOutline);
	}
}

void UInteractableComponent::PlayEffects(bool bSuccess)
{
	if (feedbackSettings.IsSoundOn()) {PlaySound(feedbackSettings.interactedSound);}
	if (feedbackSettings.IsNiagaraOn()) {PlayEffect(feedbackSettings.interactedNiagaraVFX);}
	if (feedbackSettings.IsParticleOn()) {PlayEffect(feedbackSettings.interactedParticleVFX);}
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

void UInteractableComponent::UpdateCustomOutline(bool bShowOutline)
{
	if (!feedbackSettings.customOutlineMaterial)
	{
		return;
	}

	AActor* owner = GetOwner();
	if (!owner)
	{
		return;
	}

	// 모든 MeshComponent 가져오기
	TArray<UMeshComponent*> meshComponents;
	owner->GetComponents<UMeshComponent>(meshComponents);

	if (meshComponents.Num() == 0)
	{
		return;
	}

	if (bShowOutline)
	{
		if (feedbackSettings.bUseOverlayMaterial)
		{
			// Overlay Material 방식 (기존 머티리얼 유지, 위에 오버레이)
			for (UMeshComponent* meshComp : meshComponents)
			{
				if (meshComp)
				{
					meshComp->SetOverlayMaterial(feedbackSettings.customOutlineMaterial);
				}
			}
		}
		else
		{
			// 머티리얼 교체 방식 (기존 방식)
			originalMaterials.Empty();

			for (UMeshComponent* meshComp : meshComponents)
			{
				if (!meshComp) continue;

				// 원본 머티리얼 백업
				int32 numMaterials = meshComp->GetNumMaterials();
				for (int32 i = 0; i < numMaterials; i++)
				{
					originalMaterials.Add(meshComp->GetMaterial(i));
				}

				// 커스텀 아웃라인 머티리얼로 교체
				for (int32 i = 0; i < numMaterials; i++)
				{
					meshComp->SetMaterial(i, feedbackSettings.customOutlineMaterial);
				}
			}
		}
	}
	else
	{
		if (feedbackSettings.bUseOverlayMaterial)
		{
			// Overlay Material 제거
			for (UMeshComponent* meshComp : meshComponents)
			{
				if (meshComp)
				{
					meshComp->SetOverlayMaterial(nullptr);
				}
			}
		}
		else
		{
			// 원본 머티리얼 복원
			if (originalMaterials.Num() > 0)
			{
				int32 materialIndex = 0;
				for (UMeshComponent* meshComp : meshComponents)
				{
					if (!meshComp) continue;

					int32 numMaterials = meshComp->GetNumMaterials();
					for (int32 i = 0; i < numMaterials && materialIndex < originalMaterials.Num(); i++)
					{
						meshComp->SetMaterial(i, originalMaterials[materialIndex]);
						materialIndex++;
					}
				}

				originalMaterials.Empty();
			}
		}
	}
}

