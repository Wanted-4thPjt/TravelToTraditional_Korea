#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Data/HeritageObjectData.h"
#include "TTTK_PlayerState.generated.h"

// 플레이어 Heritage 진행률 (리플리케이션용)
USTRUCT(BlueprintType)
struct FPlayerHeritageData
{
	GENERATED_BODY()

	// 발견한 Heritage ID 목록
	UPROPERTY(BlueprintReadOnly)
	TArray<FString> DiscoveredHeritageIDs;

	// 총 발견 점수
	UPROPERTY(BlueprintReadOnly)
	int32 TotalDiscoveryScore = 0;

	// 현재 레벨 완료율
	UPROPERTY(BlueprintReadOnly)
	float CurrentLevelCompletionRate = 0.0f;

	FPlayerHeritageData()
	{
		DiscoveredHeritageIDs.Empty();
		TotalDiscoveryScore = 0;
		CurrentLevelCompletionRate = 0.0f;
	}
};

UCLASS()
class UE_TTTK_API ATTTK_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ATTTK_PlayerState();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

public:
	// Heritage 발견 추가 (서버에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Heritage")
	void AddDiscoveredHeritage(const FString& HeritageID, int32 DiscoveryScore);

	// Heritage 발견 여부 확인
	UFUNCTION(BlueprintPure, Category = "Heritage")
	bool HasDiscoveredHeritage(const FString& HeritageID) const;

	// 진행률 업데이트 (서버에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Heritage")
	void UpdateHeritageProgress(float CompletionRate);

	// Heritage 데이터 접근자
	UFUNCTION(BlueprintPure, Category = "Heritage")
	FPlayerHeritageData GetHeritageData() const { return HeritageData; }

	// 발견한 Heritage 개수
	UFUNCTION(BlueprintPure, Category = "Heritage")
	int32 GetDiscoveredCount() const { return HeritageData.DiscoveredHeritageIDs.Num(); }

protected:
	// Heritage 발견 데이터 (리플리케이션됨)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Heritage")
	FPlayerHeritageData HeritageData;

	// Discovery Manager와 연동
	void BindToDiscoveryManager();

	// 발견 이벤트 수신
	UFUNCTION()
	void OnHeritageDiscovered(APlayerController* Player, const FString& HeritageID, const FHeritageObjectData& Data, bool bIsFirstDiscovery);

	// 진행률 업데이트 이벤트 수신
	UFUNCTION()
	void OnProgressUpdated(APlayerController* Player, const struct FHeritageProgress& Progress);

private:
	// Discovery Manager 참조
	UPROPERTY()
	TObjectPtr<class UHeritageDiscoveryManager> DiscoveryManager;
};
