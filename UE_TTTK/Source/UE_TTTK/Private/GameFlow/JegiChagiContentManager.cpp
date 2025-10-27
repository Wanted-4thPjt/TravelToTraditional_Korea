// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFlow/JegiChagiContentManager.h"
#include "MainPlayer.h"
#include "Net/UnrealNetwork.h"
#include "Content/Jegi/JegiChagiContentComponent.h"
#include "Content/Jegi/Jegi.h" 
#include "Kismet/GameplayStatics.h"

UJegiChagiContentManager::UJegiChagiContentManager()
{
	// Input Component 클래스 설정
	contentInputComponentClass = UJegiChagiContentComponent::StaticClass();
	inputContextPriority = 1;
}

void UJegiChagiContentManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// currentRound, totalRounds, currentPlayerIndex는 BaseContentManager에서 처리

	// JegiChagi 전용 Replication
	DOREPLIFETIME(UJegiChagiContentManager, spawnedJegis);
}

void UJegiChagiContentManager::InitializeConfig_Implementation()
{
	Super::InitializeConfig_Implementation();

	// Content Flag 설정
	contentConfig.contentType |= EContentFlags::HasRound;
	contentConfig.contentType |= EContentFlags::RecordCustomData;

	// Round 설정 (Blueprint에서 수정 가능)
	if (contentConfig.roundsPerPlayer <= 0)
	{
		contentConfig.roundsPerPlayer = 2;  // 기본값
	}

	// Custom Data Keys 설정
	contentConfig.recordedCustomKeys.Add("MaxCombo");
	contentConfig.recordedCustomKeys.Add("Perfect");
	contentConfig.recordedCustomKeys.Add("Great");
	contentConfig.recordedCustomKeys.Add("Good");
	contentConfig.recordedCustomKeys.Add("Missed");
	contentConfig.recordedCustomKeys.Add("TotalKick");

	UE_LOG(LogTemp, Log, TEXT("JegiChagiContentManager: InitializeConfig - RoundsPerPlayer: %d"),
	       contentConfig.roundsPerPlayer);
}

void UJegiChagiContentManager::InitializeContent_Implementation(const TArray<APlayerController*>& inPlayers)
{
	Super::InitializeContent_Implementation(inPlayers);

	if (!GetWorld()->GetAuthGameMode()) {return;}

	// BaseContentManager의 totalRounds 계산
	totalRounds = inPlayers.Num() * contentConfig.roundsPerPlayer;
	currentRound = -1;

	SpawnJegisForPlayers();

	// recordedCustomValues 초기화 (recordedCustomKeys와 같은 크기로)
	for (FContentParticipatingPlayerData& data : contentPlayersData)
	{
		data.recordedCustomValues.SetNum(contentConfig.recordedCustomKeys.Num());
		for (int32 i = 0; i < data.recordedCustomValues.Num(); ++i)
		{
			data.recordedCustomValues[i] = TEXT("0");
		}
	}

	UE_LOG(LogTemp, Log, TEXT("JegiChagiContentManager: InitializeContent - %d players, %d total rounds"),
	       inPlayers.Num(), totalRounds);
}

void UJegiChagiContentManager::StartContent_Implementation()
{
	Super::StartContent_Implementation();

	if (!GetWorld()->GetAuthGameMode()) {return;}

	ActivateInputForAllPlayers();
	StartNextRound();

	UE_LOG(LogTemp, Log, TEXT("JegiChagiContentManager: StartContent"));
}

void UJegiChagiContentManager::UpdateContent_Implementation(const float& deltaTime)
{
	Super::UpdateContent_Implementation(deltaTime);
}

void UJegiChagiContentManager::RoundStart_Implementation()
{
	Super::RoundStart_Implementation();

	if (!GetWorld()->GetAuthGameMode()) {return;}

	currentPlayerIndex = currentRound % contentPlayersData.Num();
	APlayerController* currentPC = contentPlayersData[currentPlayerIndex].playerController;

	if (!currentPC) {return;}

	if (spawnedJegis.IsValidIndex(currentPlayerIndex))
	{
		AJegi* jegi = spawnedJegis[currentPlayerIndex];
		if (jegi)
		{
			if (AMainPlayer* mainPlayer = Cast<AMainPlayer>(currentPC->GetPawn()))
			{
				if (UJegiChagiContentComponent* comp = mainPlayer->GetComponentByClass<UJegiChagiContentComponent>())
				{
					comp->SetMyJegi(jegi);
					comp->StartRound();
				}
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("JegiChagiContentManager: RoundStart - Round %d"), currentRound);
}

void UJegiChagiContentManager::RoundUpdate_Implementation()
{
	Super::RoundUpdate_Implementation();
}

void UJegiChagiContentManager::RoundEnd_Implementation()
{
	Super::RoundEnd_Implementation();

	if (!GetWorld()->GetAuthGameMode()) {return;}

	UE_LOG(LogTemp, Log, TEXT("JegiChagiContentManager: RoundEnd - Round %d"), currentRound);

	FTimerHandle timerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		timerHandle,
		this,
		&UJegiChagiContentManager::StartNextRound,
		2.0f,
		false
	);
}

void UJegiChagiContentManager::EndContent_Implementation()
{
	if (!GetWorld()->GetAuthGameMode()) {return;}

	CalculateFinalRankings();
	DeactivateInputForAllPlayers();

	for (AJegi* jegi : spawnedJegis)
	{
		if (jegi) jegi->Destroy();
	}
	spawnedJegis.Empty();

	UE_LOG(LogTemp, Log, TEXT("JegiChagiContentManager: EndContent"));

	Super::EndContent_Implementation();
}

void UJegiChagiContentManager::ClearContent_Implementation()
{
	Super::ClearContent_Implementation();

	// BaseContentManager에서 이미 초기화: currentRound, totalRounds, currentPlayerIndex, contentPlayersData
	// JegiChagi 전용 데이터만 정리
	spawnedJegis.Empty();
}

// ========== Client로부터 호출받는 함수 ==========

void UJegiChagiContentManager::OnPlayerKickReported(APlayerController* player, EKickTiming timing)
{
	UE_LOG(LogTemp, Verbose, TEXT("[SERVER] Player %s kicked: %s"),
	       *player->GetName(), *UEnum::GetValueAsString(timing));
}

void UJegiChagiContentManager::SaveRoundResult(APlayerController* player, const FJegiRoundRecord& result, bool bSuccess)
{
	if (!GetWorld()->GetAuthGameMode()) {return;}

	int32 playerIdx = -1;
	for (int32 i = 0; i < contentPlayersData.Num(); ++i)
	{
		if (contentPlayersData[i].playerController == player)
		{
			playerIdx = i;
			break;
		}
	}

	if (playerIdx == -1) {return;}

	FContentParticipatingPlayerData& data = contentPlayersData[playerIdx];

	// recordedCustomValues 갱신 (Keys: MaxCombo, Perfect, Great, Good, Missed, TotalKick)
	if (data.recordedCustomValues.Num() == contentConfig.recordedCustomKeys.Num())
	{
		// 0: MaxCombo - 최대값 갱신
		int32 currentMaxCombo = FCString::Atoi(*data.recordedCustomValues[0]);
		if (result.maxCombo > currentMaxCombo)
		{
			data.recordedCustomValues[0] = FString::FromInt(result.maxCombo);
			data.recordedScore = result.maxCombo;  // recordedScore도 갱신
		}

		// 1: Perfect - 누적
		int32 currentPerfect = FCString::Atoi(*data.recordedCustomValues[1]);
		data.recordedCustomValues[1] = FString::FromInt(currentPerfect + result.perfectCount);

		// 2: Great - 누적
		int32 currentGreat = FCString::Atoi(*data.recordedCustomValues[2]);
		data.recordedCustomValues[2] = FString::FromInt(currentGreat + result.greatCount);

		// 3: Good - 누적
		int32 currentGood = FCString::Atoi(*data.recordedCustomValues[3]);
		data.recordedCustomValues[3] = FString::FromInt(currentGood + result.goodCount);

		// 4: Missed - 누적
		int32 currentMissed = FCString::Atoi(*data.recordedCustomValues[4]);
		data.recordedCustomValues[4] = FString::FromInt(currentMissed + result.missedCount);

		// 5: TotalKick - 누적
		int32 currentTotalKick = FCString::Atoi(*data.recordedCustomValues[5]);
		data.recordedCustomValues[5] = FString::FromInt(currentTotalKick + result.totalKicks);
	}

	UE_LOG(LogTemp, Log, TEXT("[SERVER] Round result saved for %s - Combo: %d, Perfect: %d, Total: %s"),
	       *player->GetName(), result.maxCombo, result.perfectCount, *data.recordedCustomValues[5]);
}

void UJegiChagiContentManager::OnJegiHitGroundReported(APlayerController* player)
{
	if (!GetWorld()->GetAuthGameMode()) {return;}

	UE_LOG(LogTemp, Log, TEXT("[SERVER] Jegi hit ground - Round End"));
	RoundEnd();
}

void UJegiChagiContentManager::StartNextRound()
{
	if (!GetWorld()->GetAuthGameMode()) {return;}

	currentRound++;

	if (currentRound >= totalRounds)
	{
		UE_LOG(LogTemp, Log, TEXT("All rounds finished!"));
		EndContent();
		return;
	}

	RoundStart();
}

void UJegiChagiContentManager::CalculateFinalRankings()
{
	if (!GetWorld()->GetAuthGameMode()) {return;}

	// recordedCustomValues는 이미 SaveRoundResult에서 누적되어 있음
	// 최종 통계 로그 출력
	for (FContentParticipatingPlayerData& data : contentPlayersData)
	{
		if (data.recordedCustomValues.Num() == contentConfig.recordedCustomKeys.Num())
		{
			int32 maxCombo = FCString::Atoi(*data.recordedCustomValues[0]);
			int32 totalPerfect = FCString::Atoi(*data.recordedCustomValues[1]);
			int32 totalKicks = FCString::Atoi(*data.recordedCustomValues[5]);

			UE_LOG(LogTemp, Log, TEXT("[SERVER] Final stats for %s - MaxCombo: %d, Perfect: %d, Total: %d"),
			       *data.playerController->GetName(), maxCombo, totalPerfect, totalKicks);
		}
	}

	// 순위 정렬 (recordedScore = MaxCombo 기준)
	contentPlayersData.Sort([](const FContentParticipatingPlayerData& A, const FContentParticipatingPlayerData& B)
	{
		return A.recordedScore > B.recordedScore;
	});

	// 순위 할당
	for (int32 i = 0; i < contentPlayersData.Num(); ++i)
	{
		contentPlayersData[i].rank = i + 1;
	}
}

void UJegiChagiContentManager::SpawnJegisForPlayers()
{
	if (!GetWorld()->GetAuthGameMode()) {return;}

	spawnedJegis.Empty();

	for (int32 i = 0; i < contentPlayersData.Num(); ++i)
	{
		FVector spawnLocation = FVector::ZeroVector;
		if (contentConfig.playerSpawnPoints.IsValidIndex(i))
		{
			spawnLocation = contentConfig.playerSpawnPoints[i].GetLocation();
			spawnLocation.Z += 100.f;
		}

		FActorSpawnParameters spawnParams;
		spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		// TSubclassOf를 UClass*로 명시적 변환
		UClass* SpawnClass = jegiClass ? jegiClass.Get() : AJegi::StaticClass();
		AJegi* jegi = GetWorld()->SpawnActor<AJegi>(
			SpawnClass,
			spawnLocation,
			FRotator::ZeroRotator,
			spawnParams
		);

		if (jegi)
		{
			jegi->SetActorHiddenInGame(true);
			jegi->SetActorEnableCollision(false);
			spawnedJegis.Add(jegi);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Spawned %d Jegis"), spawnedJegis.Num());
}
