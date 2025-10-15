#include "Interaction/InteractionComponent.h"

#include "Net/UnrealNetwork.h"
#include "Interaction/InteractableComponent.h"
#include "MainPlayer.h"


UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicated(true);
}

void UInteractionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInteractionComponent, focusingActor)
}

void UInteractionComponent::Interaction()
{
	if (!IsValid(focusingActor)) {return;}

	
}

void UInteractionComponent::FocusInteractableActor(const FHitResult& hitResult)
{
	if (focusingActor)
	{
		if (hitResult.GetActor() == focusingActor) {return;}
		
		focusingActor->FindComponentByClass<UInteractableComponent>()->TryDeactivateInteractable(ownerController);
		focusingActor = nullptr;
	}
	
	if (!IsValid(hitResult.GetActor())) {return;}
	UE_LOG(LogTemp, Warning, TEXT("Actor Name : %s"), *hitResult.GetActor()->GetActorNameOrLabel());
	
	UInteractableComponent* interactable = hitResult.GetActor()->FindComponentByClass<UInteractableComponent>();
	if (!IsValid(interactable)) {return;}
	interactable->TryActivateInteractable(ownerController);
	focusingActor = hitResult.GetActor();
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UInteractionComponent::OnRegister()
{
	Super::OnRegister();

	ownerPlayer = GetOwner<AMainPlayer>();
	ownerController = ownerPlayer->GetController<APlayerController>();
}

void UInteractionComponent::Server_Interact_Implementation(UInteractableComponent* interactable)
{
	interactable->Multicast_TryInteract(ownerPlayer);
}


