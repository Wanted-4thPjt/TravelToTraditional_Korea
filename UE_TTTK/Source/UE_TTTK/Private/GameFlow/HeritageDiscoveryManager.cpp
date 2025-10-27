#include "GameFlow/HeritageDiscoveryManager.h"
#include "Interaction/HeritageObjectComponent.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

void UHeritageDiscoveryManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// 데이터 테이블 로딩
	LoadHeritageDataTable();
}

void UHeritageDiscoveryManager::LoadHeritageDataTable()
{
	// 에디터에서 설정한 데이터 테이블 사용
	if (HeritageDataTableAsset)
	{
		HeritageDataTable = HeritageDataTableAsset;
	}
	else
	{
		// 설정되지 않았으면 기본 경로에서 로딩 시도
		HeritageDataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/DT_HeritageObjects.DT_HeritageObjects"));

		if (!HeritageDataTable)
		{
			UE_LOG(LogTemp, Error, TEXT("[Heritage] 데이터 테이블 로드 실패: /Game/Data/DT_HeritageObjects"));
		}
	}
}

void UHeritageDiscoveryManager::RegisterHeritageObject(UHeritageObjectComponent* HeritageComponent)
{
	if (HeritageComponent && !RegisteredHeritageObjects.Contains(HeritageComponent))
	{
		RegisteredHeritageObjects.Add(HeritageComponent);
	}
}

void UHeritageDiscoveryManager::UnregisterHeritageObject(UHeritageObjectComponent* HeritageComponent)
{
	if (HeritageComponent)
	{
		RegisteredHeritageObjects.Remove(HeritageComponent);
	}
}

void UHeritageDiscoveryManager::ProcessHeritageDiscovery(APawn* DiscoveringPlayer, const FString& HeritageID)
{
	if (!DiscoveringPlayer || HeritageID.IsEmpty())
	{
		return;
	}

	APlayerController* PlayerController = DiscoveringPlayer->GetController<APlayerController>();
	if (!PlayerController)
	{
		return;
	}

	// 이미 발견했는지 확인
	bool bAlreadyDiscovered = HasPlayerDiscovered(PlayerController, HeritageID);

	// Heritage 데이터 가져오기
	FHeritageObjectData HeritageData = GetHeritageDataByID(HeritageID);
	if (HeritageData.ObjectID.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[Heritage] 데이터 없음: %s"), *HeritageID);
		return;
	}

	// 첫 발견인 경우에만 점수 및 카운트 추가
	if (!bAlreadyDiscovered)
	{
		UpdatePlayerProgress(PlayerController, HeritageID, HeritageData.DiscoveryScore);
		UE_LOG(LogTemp, Warning, TEXT("[Heritage] 첫 발견: %s (점수: %d)"), *HeritageID, HeritageData.DiscoveryScore);

		// 진행률 업데이트 이벤트
		FHeritageProgress Progress = GetPlayerProgress(PlayerController);
		OnProgressUpdated.Broadcast(PlayerController, Progress);
	}

	// 이벤트 방송 (첫 발견이든 재확인이든 UI는 표시)
	OnHeritageDiscovered.Broadcast(PlayerController, HeritageID, HeritageData, !bAlreadyDiscovered);
}

void UHeritageDiscoveryManager::UpdatePlayerProgress(APlayerController* Player, const FString& HeritageID, int32 Score)
{
	// 발견 목록에 추가
	TArray<FString>& DiscoveredArray = PlayerDiscoveredObjects.FindOrAdd(Player);
	if (!DiscoveredArray.Contains(HeritageID))
	{
		DiscoveredArray.Add(HeritageID);
	}

	// 점수 추가
	int32& TotalScore = PlayerTotalScores.FindOrAdd(Player);
	TotalScore += Score;
}

FHeritageObjectData UHeritageDiscoveryManager::GetHeritageDataByID(const FString& HeritageID) const
{
	if (!HeritageDataTable)
	{
		return FHeritageObjectData();
	}

	// 데이터 테이블에서 행 찾기
	FHeritageObjectData* FoundData = HeritageDataTable->FindRow<FHeritageObjectData>(
		FName(*HeritageID), TEXT("GetHeritageDataByID"));

	if (FoundData)
	{
		return *FoundData;
	}

	return FHeritageObjectData();
}

FHeritageProgress UHeritageDiscoveryManager::GetPlayerProgress(APlayerController* Player) const
{
	FHeritageProgress Progress;

	// 총 Heritage 오브젝트 수
	Progress.TotalCount = RegisteredHeritageObjects.Num();

	// 플레이어가 발견한 수
	if (const TArray<FString>* DiscoveredArray = PlayerDiscoveredObjects.Find(Player))
	{
		Progress.FoundCount = DiscoveredArray->Num();
	}

	// 완료율 계산
	if (Progress.TotalCount > 0)
	{
		Progress.CompletionRate = (float)Progress.FoundCount / (float)Progress.TotalCount;
	}

	// 총 점수
	if (const int32* TotalScore = PlayerTotalScores.Find(Player))
	{
		Progress.TotalScore = *TotalScore;
	}

	return Progress;
}

bool UHeritageDiscoveryManager::HasPlayerDiscovered(APlayerController* Player, const FString& HeritageID) const
{
	if (const TArray<FString>* DiscoveredArray = PlayerDiscoveredObjects.Find(Player))
	{
		return DiscoveredArray->Contains(HeritageID);
	}
	return false;
}

void UHeritageDiscoveryManager::InitializeLevel()
{
	// 플레이어 진행률 초기화 (새 레벨 시작 시)
	PlayerDiscoveredObjects.Empty();
	PlayerTotalScores.Empty();
}

void UHeritageDiscoveryManager::RequestCloseHeritageUI(APlayerController* Player, const FString& HeritageID)
{
	if (Player)
	{
		OnHeritageUIClose.Broadcast(Player, HeritageID);
	}
}
