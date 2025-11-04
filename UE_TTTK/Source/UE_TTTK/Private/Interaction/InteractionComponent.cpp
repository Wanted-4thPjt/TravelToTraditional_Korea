#include "Interaction/InteractionComponent.h"

#include "Data/InputMappingsSettings.h"
#include "Net/UnrealNetwork.h"
#include "Interaction/InteractableComponent.h"
#include "MainPlayer.h"
#include "Interaction/ContentEntryComponent.h"


UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	
	if (const FInputMappingData* data = UInputMappingsSettings::Get()->inputMappings.Find("IMC_Interaction"))
	{
		IMC_Interaction = data->inputMappingContext;
		IA_Interaction = data->inputActions["IA_Interaction"];
	}
	

	ComponentTags.Add("Interaction");
}

void UInteractionComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UInteractionComponent, focusingActor);
	DOREPLIFETIME(UInteractionComponent, possessingInteractedTarget);
}


void UInteractionComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	
}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (APawn* owner = GetOwner<APawn>())
	{
		if (owner->GetController<APlayerController>() && owner->IsLocallyControlled())
		{
			ownerPlayerController = owner->GetController<APlayerController>();
		}
	}
	if (IsValid(ownerPlayerController) && ownerPlayerController->IsLocalController())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ownerPlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_Interaction, 1);
		}
		if (UEnhancedInputComponent* eic = Cast<UEnhancedInputComponent>(ownerPlayerController->InputComponent))
		{
			eic->BindAction(IA_Interaction, ETriggerEvent::Triggered, this, &UInteractionComponent::InteractKeyInput);
		}
	}
}

void UInteractionComponent::InteractKeyInput(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Display, TEXT("InteractKeyInput"));
	if (IsValid(possessingInteractedTarget))
	{
		if (possessingInteractedTarget->IsContentRunning())
		{
			
		}
		else if (possessingInteractedTarget->IsLobbyActive())
		{
			
		}
		return;
	}
	if (IsValid(focusingActor))
	{
		if (UInteractableComponent* interactable = focusingActor->FindComponentByClass<UInteractableComponent>())
		{
			if (interactable->IsMultiPlayable())
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
	
	if (focusingActor)
	{
		if (UInteractableComponent* interactable = focusingActor->FindComponentByTag<UInteractableComponent>("Interactable"))
		{
			interactable->TryChangeState(GetOwner<AMainPlayer>()->GetController<APlayerController>(), EInteractableState::UnFocused);
		}
	}
	
	//UE_LOG(LogTemp, Display, TEXT("    Client : %s - %s"), *GetOwner()->GetActorNameOrLabel(), *(hitActor ? hitActor->GetActorNameOrLabel() : "NULL"));
	Server_Focus(hitActor);
	
	if (hitActor)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Actor Name : %s"), *hitActor->GetActorNameOrLabel());
		if (UInteractableComponent* interactable = hitActor->FindComponentByClass<UInteractableComponent>())
		{
			interactable->TryChangeState(GetOwner<AMainPlayer>()->GetController<APlayerController>(), EInteractableState::Focused);
		}
	}
}

void UInteractionComponent::Server_FinishInteraction_Implementation()
{
	if (!IsValid(possessingInteractedTarget) || !IsValid(GetOwner<AMainPlayer>())) {return;}
	possessingInteractedTarget->RequestFinishContent();
	possessingInteractedTarget = nullptr;
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
	possessingInteractedTarget = interactable->GetOwner()->FindComponentByTag<UContentEntryComponent>("Entry");
	if (possessingInteractedTarget)
	{
		possessingInteractedTarget->RequestEntry(GetOwner<AMainPlayer>());
	}
}