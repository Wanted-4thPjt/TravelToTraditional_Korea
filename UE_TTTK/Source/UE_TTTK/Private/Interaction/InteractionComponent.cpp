#include "Interaction/InteractionComponent.h"

#include "Net/UnrealNetwork.h"
#include "Interaction/InteractableComponent.h"
#include "MainPlayer.h"
#include "Jegi/Jegi.h"


UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UInteractionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInteractionComponent, focusingActor);
	DOREPLIFETIME(UInteractionComponent, possessingInteractable);
}


void UInteractionComponent::InitializeComponent()
{
	Super::InitializeComponent();

}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	tempPC = GetOwner<APawn>()->GetController<APlayerController>();
}

void UInteractionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (possessingInteractable && tempPC)
	{
		if (tempPC->WasInputKeyJustPressed(EKeys::LeftMouseButton))
		{
			if (AJegi* jegi = Cast<AJegi>(possessingInteractable->GetOwner()))
			{
				jegi->TempKick();
			}
		}
		
	}
}

void UInteractionComponent::InteractKeyInput()
{
	if (IsValid(possessingInteractable))
	{
		possessingInteractable->FinishInteracting(GetOwner<AMainPlayer>()->GetController<APlayerController>(), EInteractableState::OutOfBound);
		Server_FinishInteraction();
		return;
	}
	
	if (IsValid(focusingActor))
	{
		if (UInteractableComponent* interactable = focusingActor->FindComponentByClass<UInteractableComponent>())
		{
			if (interactable->CanPossess())
			{
				Server_Interact(interactable);
			}
			interactable->TryInteract(GetOwner<AMainPlayer>()->GetController<APlayerController>());
		}
	}
}

void UInteractionComponent::FocusInteractableActor(const FHitResult& hitResult)
{
	AActor* hitActor = hitResult.GetActor();
	if (hitActor == focusingActor) {return;}
	
	//UE_LOG(LogTemp, Display, TEXT("===========Update Interactable actor==========="));
	if (focusingActor)
	{
		if (UInteractableComponent* interactable = focusingActor->FindComponentByClass<UInteractableComponent>())
		{
			interactable->TryDeactivateInteractable(GetOwner<AMainPlayer>()->GetController<APlayerController>());
		}
	}
	
	//UE_LOG(LogTemp, Display, TEXT("    Client : %s - %s"), *GetOwner()->GetActorNameOrLabel(), *(hitActor ? hitActor->GetActorNameOrLabel() : "NULL"));
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

void UInteractionComponent::Server_FinishInteraction_Implementation()
{
	if (!IsValid(possessingInteractable) || !IsValid(GetOwner<AMainPlayer>())) {return;}
	possessingInteractable->Multicast_FinishInteracting(GetOwner<AMainPlayer>());
	possessingInteractable = nullptr;
}

void UInteractionComponent::Server_Focus_Implementation(AActor* focusedActor)
{
	//UE_LOG(LogTemp, Display, TEXT("    Server - %s"), *(focusedActor ? focusedActor->GetActorNameOrLabel() : "NULL"));
	focusingActor = focusedActor;
}

void UInteractionComponent::Server_Interact_Implementation(UInteractableComponent* interactable)
{
	if (!IsValid(interactable) || !IsValid(GetOwner<AMainPlayer>())) {return;}
	interactable->Multicast_TryInteract(GetOwner<AMainPlayer>());
	possessingInteractable = interactable;
}


