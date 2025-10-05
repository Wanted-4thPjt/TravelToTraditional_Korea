#include "Interaction/InteractableComponent.h"

#include "MainPlayer.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Particles/ParticleSystem.h"


UInteractableComponent::UInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	interactionGuideComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionGuide"));
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	state = EInteractableState::OutOfBound;

	if (feedbackSettings.IsWidgetOn() && feedbackSettings.interactionGuideWidgetClass && IsValid(interactionGuideComponent))
	{
		UUserWidget* widget = CreateWidget(GetWorld(), feedbackSettings.interactionGuideWidgetClass, FName("WidgetForGuide"));
		if (IsValid(widget))
		{
			interactionGuideComponent->SetWidget(widget);
			interactionGuideComponent->SetWidgetSpace(EWidgetSpace::World);
			interactionGuideComponent->SetRelativeLocation(feedbackSettings.widgetOffset);
		}
	}
	if (feedbackSettings.IsOutlineOn() && IsValid(feedbackSettings.outlineComponent))
	{
		feedbackSettings.outlineComponent->SetRenderCustomDepth(false);
		feedbackSettings.outlineComponent->SetCustomDepthStencilValue(0);
	}
}

void UInteractableComponent::OutOfBound()
{
	state = EInteractableState::OutOfBound;
	onChangeState.Broadcast(state);
	UpdateVisuals();
}

void UInteractableComponent::ActivateInteractable()
{
	if (state == EInteractableState::Active) {return;}
	if (state == EInteractableState::Interacting) {return;}
	
	state = EInteractableState::Active;
	onChangeState.Broadcast(state);
	UpdateVisuals();

	if (feedbackSettings.IsSoundOn()) {PlaySound(feedbackSettings.activatedSound);}
}

void UInteractableComponent::DeactivateInteractable()
{
	if (state == EInteractableState::Inactive) {return;}
	if (state == EInteractableState::Interacting) {return;}
	
	state = EInteractableState::Inactive;
	onChangeState.Broadcast(state);
	UpdateVisuals();
}

bool UInteractableComponent::TryInteract(AMainPlayer* player)
{
	if (state != EInteractableState::Active || !IsValid(player)) {return false;}

	state = EInteractableState::Interacting;
	onChangeState.Broadcast(state);
	UpdateVisuals();
	
	onInteract.Broadcast(player);

	if (feedbackSettings.IsSoundOn()) {PlaySound(feedbackSettings.interactedSound);}
	if (feedbackSettings.IsNiagaraOn()) {PlayEffect(feedbackSettings.interactedNiagaraVFX);}
	if (feedbackSettings.IsParticleOn()) {PlayEffect(feedbackSettings.interactedParticleVFX);}
	
	return true;
	
}

void UInteractableComponent::FinishInteracting(const EInteractableState& newState)
{
	if (state != EInteractableState::Interacting) {return;}
	
	state = newState;
	onChangeState.Broadcast(state);
	UpdateVisuals();
}

void UInteractableComponent::UpdateVisuals()
{
	UpdateOutline();
	UpdateWidget();
}

void UInteractableComponent::UpdateOutline()
{
	if (!feedbackSettings.IsOutlineOn()) {return;}
	if (!IsValid(feedbackSettings.outlineComponent)) {return;}

	feedbackSettings.outlineComponent->SetRenderCustomDepth(state == EInteractableState::Active);
	feedbackSettings.outlineComponent->SetCustomDepthStencilValue(
		state == EInteractableState::Active ? feedbackSettings.outlineStencilValue : 0
	);
}

void UInteractableComponent::UpdateWidget()
{
	if (!feedbackSettings.IsWidgetOn()) {return;}
	if (!IsValid(interactionGuideComponent)) {return;}

	interactionGuideComponent->SetVisibility(state == EInteractableState::Active);
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

