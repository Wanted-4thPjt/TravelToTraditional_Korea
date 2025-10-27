#include "GameFlow/TTTK_PlayerState.h"
#include "GameFlow/HeritageDiscoveryManager.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"

ATTTK_PlayerState::ATTTK_PlayerState()
{
	// 네트워크 업데이트 빈도 설정
	NetUpdateFrequency = 10.0f;
}

void ATTTK_PlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Heritage 데이터 리플리케이션
	DOREPLIFETIME(ATTTK_PlayerState, HeritageData);
}

void ATTTK_PlayerState::BeginPlay()
{
	Super::BeginPlay();

	// Discovery Manager와 연동 (서버에서만)
	if (HasAuthority())
	{
		BindToDiscoveryManager();
	}
}

void ATTTK_PlayerState::BindToDiscoveryManager()
{
	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		DiscoveryManager = GameInstance->GetSubsystem<UHeritageDiscoveryManager>();

		if (DiscoveryManager)
		{
			// 발견 이벤트에 바인딩
			DiscoveryManager->OnHeritageDiscovered.AddDynamic(
				this, &ATTTK_PlayerState::OnHeritageDiscovered);

			// 진행률 업데이트 이벤트에 바인딩
			DiscoveryManager->OnProgressUpdated.AddDynamic(
				this, &ATTTK_PlayerState::OnProgressUpdated);

			UE_LOG(LogTemp, Log, TEXT("PlayerState bound to Discovery Manager"));
		}
	}
}

void ATTTK_PlayerState::AddDiscoveredHeritage(const FString& HeritageID, int32 DiscoveryScore)
{
	// 서버에서만 실행
	if (!HasAuthority())
	{
		return;
	}

	// 중복 체크
	if (HeritageData.DiscoveredHeritageIDs.Contains(HeritageID))
	{
		UE_LOG(LogTemp, Warning, TEXT("Heritage already discovered: %s"), *HeritageID);
		return;
	}

	// 발견 목록에 추가
	HeritageData.DiscoveredHeritageIDs.Add(HeritageID);
	HeritageData.TotalDiscoveryScore += DiscoveryScore;

	UE_LOG(LogTemp, Log, TEXT("Heritage added to PlayerState: %s (Score: %d, Total: %d)"),
		*HeritageID, DiscoveryScore, HeritageData.TotalDiscoveryScore);
}

void ATTTK_PlayerState::UpdateHeritageProgress(float CompletionRate)
{
	// 서버에서만 실행
	if (!HasAuthority())
	{
		return;
	}

	HeritageData.CurrentLevelCompletionRate = CompletionRate;
}

bool ATTTK_PlayerState::HasDiscoveredHeritage(const FString& HeritageID) const
{
	return HeritageData.DiscoveredHeritageIDs.Contains(HeritageID);
}

void ATTTK_PlayerState::OnHeritageDiscovered(APlayerController* Player, const FString& HeritageID, const FHeritageObjectData& Data, bool bIsFirstDiscovery)
{
	// 이 PlayerState의 소유자가 발견한 경우에만 처리
	// 첫 발견인 경우에만 점수 추가
	if (Player == GetPlayerController() && bIsFirstDiscovery)
	{
		AddDiscoveredHeritage(HeritageID, Data.DiscoveryScore);
	}
}

void ATTTK_PlayerState::OnProgressUpdated(APlayerController* Player, const FHeritageProgress& Progress)
{
	// 이 PlayerState의 소유자의 진행률인 경우에만 처리
	if (Player == GetPlayerController())
	{
		UpdateHeritageProgress(Progress.CompletionRate);
	}
}
