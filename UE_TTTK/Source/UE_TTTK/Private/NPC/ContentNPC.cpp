#include "NPC/ContentNPC.h"

#include "Interaction/ContentEntryComponent.h"
#include "Interaction/InteractableComponent.h"
#include "MainPlayer.h"

AContentNPC::AContentNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	contentEntry = CreateDefaultSubobject<UContentEntryComponent>("ContentEntryComponent");
	interactableComponent = CreateDefaultSubobject<UInteractableComponent>("InteractableComponent");
	
	SetReplicates(true);
	SetReplicateMovement(false);
}

void AContentNPC::BeginPlay()
{
	Super::BeginPlay();

	interactableComponent->onChangeState.AddDynamic(this, &AContentNPC::OnInteractablePlayerStateChanged);
}


void AContentNPC::OnInteractablePlayerStateChanged(APlayerController* playerController, const EInteractableState& state)
{	
	switch (state)
	{
	case EInteractableState::Default:
	case EInteractableState::OutOfBound:
		{
			contentEntry->RequestLeaveLobby(playerController->GetPawn<AMainPlayer>());
		}
		break;
	case EInteractableState::InRange:
	case EInteractableState::UnFocused:
		break;
	case EInteractableState::Focused:
		break;
	case EInteractableState::Interacting:
		break;
	}
}
