#include "GameFlow/BaseContentManager.h"


ABaseContentManager::ABaseContentManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseContentManager::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseContentManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

