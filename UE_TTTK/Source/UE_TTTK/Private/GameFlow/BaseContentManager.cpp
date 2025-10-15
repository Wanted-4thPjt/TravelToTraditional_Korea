#include "GameFlow/BaseContentManager.h"

#include "ToolContextInterfaces.h"
#include "Interaction/ContentEntryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"


ABaseContentManager::ABaseContentManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABaseContentManager::BeginPlay()
{
	Super::BeginPlay();
	InitializeConfig();
	
}

void ABaseContentManager::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseContentManager, contentState);
	DOREPLIFETIME(ABaseContentManager, remainingTime);
	DOREPLIFETIME(ABaseContentManager, contentPlayersData);
	DOREPLIFETIME(ABaseContentManager, finishedPlayersCount);
}

void ABaseContentManager::InitializeConfig_Implementation()
{
	if (!HasAuthority()) {return;}
	
	if (contentConfig.playerSpawnPoints.IsEmpty())
	{
		TArray<AActor*> spawnPointsByActor;
		//UGameplayStatics::GetAllActorsWithTag(GetWorld(), contentConfig.contentName, spawnPointsByActor);
		/*
		for (AActor* spawnPointByActor : spawnPointsByActor)
		{
			contentConfig.spawnPoints.Add(spawnPointByActor->GetActorTransform());
		}
		 */
	}

	ownerEntryComponent = GetAttachParentActor()->FindComponentByClass<UContentEntryComponent>();
}

void ABaseContentManager::InitializeContent_Implementation(const TArray<APlayerController*>& inPlayers)
{
	for (APlayerController* playerController : inPlayers)
	{
		FContentParticipatingPlayerData data;
		data.playerController = playerController;
		contentPlayersData.Add(data);
	}
}

void ABaseContentManager::StartContent_Implementation()
{
	if (!HasAuthority()) {return;}
	if (contentTimer.IsValid()) {return;}
	contentState = EContentState::Playing;

	//TeleportPlayersIntoContent(contentConfig/*.spawnPoints*/);
	
	if (contentConfig.timeLimit > 0.f)
	{
		remainingTime = contentConfig.timeLimit;
		GetWorld()->GetTimerManager().SetTimer(
			contentTimer, this, &ABaseContentManager::OnContentTimerExpired,
			contentConfig.timeLimit, false
		);
	}
}

void ABaseContentManager::UpdateContent_Implementation(const float& deltaTime)
{
	if (contentTimer.IsValid())
	{
		remainingTime = remainingTime > 0.f ? remainingTime - deltaTime : 0.f;
	}
}

void ABaseContentManager::EndContent_Implementation()
{
	contentState = EContentState::Finished;

	if (contentTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(contentTimer);
	}

	ReturnPlayersToLobby();

	for (const FContentParticipatingPlayerData& result : contentPlayersData)
	{
		if (IsValid(result.playerController))
		{
			// replace logs into UI on client hud
			if (contentConfig.bRecordingScore)
			{
				UE_LOG(LogTemp, Log, TEXT("  Score: %d"), result.recordedScore);
			}
			if (contentConfig.bRecordingTime)
			{
				UE_LOG(LogTemp, Log, TEXT("  Time: %.2f"), result.recordedTime);
			}
		}
	}
	if (ownerEntryComponent)
	{
		ownerEntryComponent->OnContentFinished();
	}
}

void ABaseContentManager::ClearContent_Implementation()
{
	contentState = EContentState::Ready;
	contentPlayersData.Empty();
}

void ABaseContentManager::OnContentTimerExpired_Implementation()
{
	EndContent();
}

void ABaseContentManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HasAuthority()) {return;}

	if (contentState == EContentState::Playing)
	{
		UpdateContent(DeltaTime);
	}
}


void ABaseContentManager::TeleportPlayersIntoContent()
{
	if (!HasAuthority()) {return;}

	int32 spawnIndex = 0;
	/*
	int32 spawnPointsCount = contentConfig.spawnPoints.Num();
	for (APlayerController* player : participatingPlayers)
	{
		FTransform spawnTransform = contentConfig.spawnPoints[spawnIndex++ % spawnPointsCount];
		player->GetPawn()->SetActorLocationAndRotation(spawnTransform.GetLocation(), spawnTransform.GetRotation());
	}
	*/
}

void ABaseContentManager::ReturnPlayersToLobby()
{
	if (!HasAuthority()) return;

	if (!ownerEntryComponent) return;

	AActor* lobbyActor = ownerEntryComponent->GetOwner();
	if (!IsValid(lobbyActor)) return;

	FVector lobbyLocation = lobbyActor->GetActorLocation();
	FRotator lobbyRotation = lobbyActor->GetActorRotation();

	for (const FContentParticipatingPlayerData& data  : contentPlayersData)
	{
		if (data.playerController && data.playerController->GetPawn())
		{
			FVector offset = FVector(FMath::RandRange(-200.f, 200.f),
									 FMath::RandRange(-200.f, 200.f),
									 0.f);
			data.playerController->GetPawn()->SetActorLocation(lobbyLocation + offset);
			data.playerController->GetPawn()->SetActorRotation(lobbyRotation);
		}
	}

}

void ABaseContentManager::OnPlayerAction(APlayerController* actionPlayer, FName actionName, FVector inData)
{
}

void ABaseContentManager::NotifyContentFinished_Implementation(FContentParticipatingPlayerData result)
{
	finishedPlayersCount++;
	for (FContentParticipatingPlayerData& data  : contentPlayersData)
	{
		if (data.playerController && data.playerController->GetPawn())
		{
			if (result.playerController->GetNetPushId() == data.playerController->GetNetPushId())
			{
				data = result;
			}
		}
	}
}

