#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/HeritageObjectData.h"
#include "HeritageDiscoveryManager.generated.h"

class UHeritageObjectComponent;
class UDataTable;
class APlayerController;

// 플레이어 발견 진행률 구조체
USTRUCT(BlueprintType)
struct FHeritageProgress
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 TotalCount = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 FoundCount = 0;

	UPROPERTY(BlueprintReadOnly)
	float CompletionRate = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	int32 TotalScore = 0;
};

// 발견 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHeritageDiscovered,
	APlayerController*, Player, const FString&, HeritageID, const FHeritageObjectData&, HeritageData, bool, bIsFirstDiscovery);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnProgressUpdated,
	APlayerController*, Player, const FHeritageProgress&, Progress);

// UI 닫기 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHeritageUIClose,
	APlayerController*, Player, const FString&, HeritageID);

UCLASS()
class UE_TTTK_API UHeritageDiscoveryManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// 서브시스템 초기화
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Heritage 데이터 테이블 (에디터에서 설정 가능)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heritage Settings")
	TObjectPtr<UDataTable> HeritageDataTableAsset;

	// Heritage 오브젝트 등록/해제
	UFUNCTION(BlueprintCallable, Category = "Heritage Discovery")
	void RegisterHeritageObject(UHeritageObjectComponent* HeritageComponent);

	UFUNCTION(BlueprintCallable, Category = "Heritage Discovery")
	void UnregisterHeritageObject(UHeritageObjectComponent* HeritageComponent);

	// 발견 처리 (핵심 함수)
	UFUNCTION(BlueprintCallable, Category = "Heritage Discovery")
	void ProcessHeritageDiscovery(APawn* DiscoveringPlayer, const FString& HeritageID);

	// 데이터 조회
	UFUNCTION(BlueprintPure, Category = "Heritage Discovery")
	FHeritageObjectData GetHeritageDataByID(const FString& HeritageID) const;

	UFUNCTION(BlueprintPure, Category = "Heritage Discovery")
	FHeritageProgress GetPlayerProgress(APlayerController* Player) const;

	UFUNCTION(BlueprintPure, Category = "Heritage Discovery")
	bool HasPlayerDiscovered(APlayerController* Player, const FString& HeritageID) const;

	// 레벨 초기화
	UFUNCTION(BlueprintCallable, Category = "Heritage Discovery")
	void InitializeLevel();

	// 이벤트 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Heritage Events")
	FOnHeritageDiscovered OnHeritageDiscovered;

	UPROPERTY(BlueprintAssignable, Category = "Heritage Events")
	FOnProgressUpdated OnProgressUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Heritage Events")
	FOnHeritageUIClose OnHeritageUIClose;

	// UI 닫기 요청
	UFUNCTION(BlueprintCallable, Category = "Heritage Discovery")
	void RequestCloseHeritageUI(APlayerController* Player, const FString& HeritageID);

protected:
	// 데이터 테이블 로딩
	void LoadHeritageDataTable();
	
	// 플레이어별 발견 목록 업데이트
	void UpdatePlayerProgress(APlayerController* Player, const FString& HeritageID, int32 Score);

private:
	// Heritage 데이터 테이블
	UPROPERTY()
	TObjectPtr<UDataTable> HeritageDataTable;

	// 레벨에 등록된 Heritage 오브젝트들
	UPROPERTY()
	TArray<TObjectPtr<UHeritageObjectComponent>> RegisteredHeritageObjects;

	// 플레이어별 발견한 Heritage ID 목록 (서버에서만 사용, 복제 안함)
	TMap<APlayerController*, TArray<FString>> PlayerDiscoveredObjects;

	// 플레이어별 총 점수 (서버에서만 사용, 복제 안함)
	TMap<APlayerController*, int32> PlayerTotalScores;
};
