#include "GameFlow/BaseContentManager.h"

#include "Interaction/ContentEntryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "Content/BaseContentComponent.h"  // Content Input Component
#include "MainPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"


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

	DOREPLIFETIME(UBaseContentManager, contentConfig);
	DOREPLIFETIME(UBaseContentManager, contentState);
	DOREPLIFETIME(UBaseContentManager, currentRound);
	DOREPLIFETIME(UBaseContentManager, totalRounds);
	DOREPLIFETIME(UBaseContentManager, currentPlayerIndex);
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
			else if (contentConfig.IsContentTyped(EContentFlags::ChangeCameraView))
			{
				contentConfig.cameraTransform.Add(pointByActor->GetActorTransform());
			}
		}
	}

	/*
	currentRound = contentConfig.IsContentTyped(EContentFlags::HasRound);
	contentConfig.IsContentTyped(EContentFlags::RecordScore);
	*/

}

void UBaseContentManager::InitializeContent_Implementation(const TArray<APlayerController*>& inPlayers)
{
	for (APlayerController* playerController : inPlayers)
	{
		FContentParticipatingPlayerData data;
		data.playerController = playerController;
		data.prevTransform = playerController->GetLevelTransform();
		contentPlayersData.Add(data);
	}

	if (contentConfig.IsContentTyped(EContentFlags::ChangeCameraView) && contentConfig.cameraTransform.Num() > 0)
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
			if (contentConfig.IsContentTyped(EContentFlags::RecordScore))
			{
				UE_LOG(LogTemp, Log, TEXT("  Score: %d"), result.recordedScore);
			}
			if (contentConfig.IsContentTyped(EContentFlags::RecordTime))
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
	currentRound = -1;
	totalRounds = 0;
	currentPlayerIndex = -1;
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

	for (const FContentParticipatingPlayerData& data  : contentPlayersData)
	{
		if (data.playerController && data.playerController->GetPawn())
		{
			data.playerController->GetPawn()->SetActorLocation(data.prevTransform.GetLocation());
			data.playerController->GetPawn()->SetActorRotation(data.prevTransform.GetRotation());
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

// ========== Content Input 관리 (새로 추가) ==========

void UBaseContentManager::ActivateInputForPlayer(APlayerController* player)
{
	if (!player || !HasCustomInput()) return;

	AMainPlayer* mainPlayer = Cast<AMainPlayer>(player->GetPawn());
	if (!mainPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("BaseContentManager: Player pawn is not AMainPlayer"));
		return;
	}

	// 1. Content Input Component 활성화 (Lazy Creation)
	mainPlayer->ActivateContentInput(this);

	// 2. Component의 InputMappingContext 추가
	UBaseContentComponent* contentComp = mainPlayer->GetActiveContentInput();
	if (contentComp && contentComp->GetInputMappingContext())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(player->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(contentComp->GetInputMappingContext(), inputContextPriority);
			UE_LOG(LogTemp, Log, TEXT("BaseContentManager: Added InputContext '%s' for player %s"),
			       *contentComp->GetInputMappingContext()->GetName(), *player->GetName());
		}
	}
}

void UBaseContentManager::DeactivateInputForPlayer(APlayerController* player)
{
	if (!player || !HasCustomInput()) return;

	AMainPlayer* mainPlayer = Cast<AMainPlayer>(player->GetPawn());
	if (!mainPlayer) return;

	// 1. Component의 InputMappingContext 제거
	UBaseContentComponent* contentComp = mainPlayer->GetActiveContentInput();
	if (contentComp && contentComp->GetInputMappingContext())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(player->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(contentComp->GetInputMappingContext());
			UE_LOG(LogTemp, Log, TEXT("BaseContentManager: Removed InputContext '%s' for player %s"),
			       *contentComp->GetInputMappingContext()->GetName(), *player->GetName());
		}
	}

	// 2. Content Input Component 비활성화
	mainPlayer->DeactivateContentInput();
}

void UBaseContentManager::ActivateInputForAllPlayers()
{
	for (FContentParticipatingPlayerData& data : contentPlayersData)
	{
		if (data.playerController)
		{
			ActivateInputForPlayer(data.playerController);
		}
	}
}

void UBaseContentManager::DeactivateInputForAllPlayers()
{
	for (FContentParticipatingPlayerData& data : contentPlayersData)
	{
		if (data.playerController)
		{
			DeactivateInputForPlayer(data.playerController);
		}
	}
}
