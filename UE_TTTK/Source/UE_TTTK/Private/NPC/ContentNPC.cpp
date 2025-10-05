#include "NPC/ContentNPC.h"

#include "Interaction/ContentEntryComponent.h"

AContentNPC::AContentNPC()
{
	PrimaryActorTick.bCanEverTick = false;

	contentEntry = CreateDefaultSubobject<UContentEntryComponent>("ContentEntryComponent");
	
	SetReplicates(true);
	SetReplicateMovement(false);
}

void AContentNPC::BeginPlay()
{
	Super::BeginPlay();

}

void AContentNPC::SetOutlineEnabled(bool bEnabled)
{
	if (IsValid(GetMesh()))
	{
		GetMesh()->SetRenderCustomDepth(bEnabled);
		GetMesh()->SetCustomDepthStencilValue(bEnabled ? contentEntry->GetOutlineDepthStencilValue() : 0);
	}
}
