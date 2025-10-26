#include "GameFlow/BaseContentManager.h"

#include "Interaction/ContentEntryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"


UBaseContentManager::UBaseContentManager()
{
}

void UBaseContentManager::PostLoad()
{
	UObject::PostLoad();
	InitializeConfig();
}


void UBaseContentManager::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBaseContentManager, contentState);
	DOREPLIFETIME(UBaseContentManager, remainingTime);
	DOREPLIFETIME(UBaseContentManager, contentPlayersData);
	DOREPLIFETIME(UBaseContentManager, finishedPlayersCount);
}

void UBaseContentManager::InitializeConfig_Implementation()
{
	if (!GetWorld()->GetAuthGameMode()) {return;}
	
	if (contentConfig.playerSpawnPoints.IsEmpty())
	{
		TArray<AActor*> pointsByActor;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), contentConfig.contentName, pointsByActor);
		
		for (AActor* pointByActor : pointsByActor)
		{
			if (!pointByActor->ActorHasTag("Camera"))
			{
				contentConfig.playerSpawnPoints.Add(pointByActor->GetActorTransform());
			}
			else if (contentConfig.bChangeCameraView)
			{
				contentConfig.cameraTransform.Add(pointByActor->GetActorTransform());
			}
		}
		
	}

	//ownerEntryComponent = ->FindComponentByClass<UContentEntryComponent>();
}

void UBaseContentManager::InitializeContent_Implementation(const TArray<APlayerController*>& inPlayers)
{
	for (APlayerController* playerController : inPlayers)
	{
		FContentParticipatingPlayerData data;
		data.playerController = playerController;
		contentPlayersData.Add(data);
	}

	if (contentConfig.bChangeCameraView && contentConfig.cameraTransform.Num() > 0)
	{
		for (const FTransform& cameraTransform : contentConfig.cameraTransform)
		{
			AActor* tempCameraActor = GetWorld()->SpawnActor(AActor::StaticClass(), &cameraTransform);
			contentCameras.Add(tempCameraActor);
		}
		for (FContentParticipatingPlayerData& data : contentPlayersData)
		{
			data.originalViewTarget = data.playerController->GetViewTarget();
		}
	}
}

void UBaseContentManager::StartContent_Implementation()
{
	if (!GetWorld()->GetAuthGameMode()) {return;}
	if (contentTimer.IsValid()) {return;}
	
	contentState = EContentState::Playing;
	finishedPlayersCount = 0;
	TeleportPlayersIntoContent();
	
	if (contentConfig.contentTimeLimit > 1.f)
	{
		startTime = GetWorld()->GetTimeSeconds();
		remainingTime = contentConfig.contentTimeLimit;
		
		GetWorld()->GetTimerManager().SetTimer(
			contentTimer, this, &UBaseContentManager::OnUpdateTimer,
			0.1f, true
		);
	}

	
}

void UBaseContentManager::UpdateContent_Implementation(const float& deltaTime)
{
}

void UBaseContentManager::RoundStart_Implementation()
{
}

void UBaseContentManager::RoundUpdate_Implementation()
{
}

void UBaseContentManager::RoundEnd_Implementation()
{
}

void UBaseContentManager::EndContent_Implementation()
{
	contentState = EContentState::Finished;

	if (contentTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(contentTimer);
	}

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
			if (result.originalViewTarget)
			{
				result.playerController->SetViewTargetWithBlend(result.originalViewTarget, 1.0f, VTBlend_EaseInOut);
			}
		}
	}

	for (AActor* contentCamera : contentCameras)
	{
		contentCamera->Destroy();
	}
	contentCameras.Empty();

	// TODO: 결과창 종료 누르면 이동하게 변경
	ReturnPlayersToLobby();
}

void UBaseContentManager::ClearContent_Implementation()
{
	contentState = EContentState::Ready;
	contentPlayersData.Empty();
}

void UBaseContentManager::OnUpdateTimer_Implementation()
{
	if (!GetWorld()->GetAuthGameMode()) {return;}
	if (contentState != EContentState::Playing) {return;}
	
	float currentTime = GetWorld()->GetTimeSeconds();
	float elapsedTime = currentTime - startTime;
	remainingTime = contentConfig.contentTimeLimit - elapsedTime;

	if (remainingTime <= 0.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(contentTimer);
		// TODO: UI Update To Zero
		EndContent();
		return;
	}
	// TODO: UI Update To remaining Time
}

void UBaseContentManager::TeleportPlayersIntoContent()
{
	if (!GetWorld()->GetAuthGameMode()) {return;}

	int32 spawnIndex = 0;
	int32 spawnPointsCount = contentConfig.playerSpawnPoints.Num();
	for (FContentParticipatingPlayerData playerData : contentPlayersData)
	{
		FTransform spawnTransform = contentConfig.playerSpawnPoints[spawnIndex++ % spawnPointsCount];
		if (playerData.playerController && playerData.playerController->GetPawn())
		{
			playerData.playerController->GetPawn()->SetActorLocationAndRotation(spawnTransform.GetLocation(), spawnTransform.GetRotation());
		}
	}
}

void UBaseContentManager::ReturnPlayersToLobby()
{
	if (!GetWorld()->GetAuthGameMode()) {return;}

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

void UBaseContentManager::OnPlayerAction(APlayerController* actionPlayer, FName actionName, FVector inData)
{
}

void UBaseContentManager::ContentFinished(const FContentParticipatingPlayerData& result)
{
	if (!GetWorld()->GetAuthGameMode() || !IsValid(result.playerController)) {return;}
	
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

