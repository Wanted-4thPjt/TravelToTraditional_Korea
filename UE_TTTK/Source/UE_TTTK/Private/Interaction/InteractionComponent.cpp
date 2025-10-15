#include "Interaction/InteractionComponent.h"

#include "Interaction/InteractableComponent.h"
#include "MainPlayer.h"


UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	ownerPlayer = GetOwner<AMainPlayer>();

	SetIsReplicated(true);
}


// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UInteractionComponent::Server_Interact_Implementation(UInteractableComponent* interactable)
{
	interactable->Multicast_TryInteract(ownerPlayer);
}


