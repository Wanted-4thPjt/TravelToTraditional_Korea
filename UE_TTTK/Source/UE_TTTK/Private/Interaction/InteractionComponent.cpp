#include "Interaction/InteractionComponent.h"

#include "Net/UnrealNetwork.h"
#include "Interaction/InteractableComponent.h"
#include "MainPlayer.h"


UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UInteractionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInteractionComponent, focusingActor);
}


void UInteractionComponent::InitializeComponent()
{
	Super::InitializeComponent();

}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UInteractionComponent::Interaction()
{
	if (!IsValid(focusingActor)) {return;}

	if (UInteractableComponent* interactable = focusingActor->FindComponentByClass<UInteractableComponent>())
	{
		if (interactable->CanPossess())
		{
			Server_Interact(interactable);
			return;
		}
		
		interactable->TryInteract(GetOwner<AMainPlayer>()->GetController<APlayerController>());
	}
}

void UInteractionComponent::FocusInteractableActor(const FHitResult& hitResult)
{
	AActor* hitActor = hitResult.GetActor();
	if (hitActor == focusingActor) {return;}
	
	if (focusingActor)
	{
		if (UInteractableComponent* interactable = focusingActor->FindComponentByClass<UInteractableComponent>())
		{
			interactable->TryDeactivateInteractable(GetOwner<AMainPlayer>()->GetController<APlayerController>());
		}
	}
	Server_Focus(hitActor);
	
	if (hitActor)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Actor Name : %s"), *hitActor->GetActorNameOrLabel());
		if (UInteractableComponent* interactable = hitActor->FindComponentByClass<UInteractableComponent>())
		{
			interactable->TryActivateInteractable(GetOwner<AMainPlayer>()->GetController<APlayerController>());
		}
	}
}

void UInteractionComponent::Server_Focus_Implementation(AActor* focusedActor)
{
	focusingActor = focusedActor;
}

void UInteractionComponent::Server_Interact_Implementation(UInteractableComponent* interactable)
{
	interactable->Multicast_TryInteract(GetOwner<AMainPlayer>());
}


