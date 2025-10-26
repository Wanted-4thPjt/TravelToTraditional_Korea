#include "NPC/ContentNPC.h"

#include "Interaction/ContentEntryComponent.h"
#include "Interaction/InteractableComponent.h"
#include "UI/InteractableWidget.h"

AContentNPC::AContentNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	contentEntry = CreateDefaultSubobject<UContentEntryComponent>("ContentEntryComponent");
	interactableComponent = CreateDefaultSubobject<UInteractableComponent>("InteractableComponent");
	interactableComponent->feedbackSettings.EnableNetwork(true);
	interactableComponent->feedbackSettings.EnableWidget(true, UInteractableWidget::StaticClass(), "Widget");
	
	SetReplicates(true);
	bReplicates = true;
}

void AContentNPC::BeginPlay()
{
	Super::BeginPlay();

	interactableComponent->OnClientInteraction.AddDynamic(this, &AContentNPC::OnInteractablePlayerStateChanged);
}


void AContentNPC::OnInteractablePlayerStateChanged(APlayerController* playerController)
{	
	

	return;
}
