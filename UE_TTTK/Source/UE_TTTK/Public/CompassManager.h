// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CompassManager.generated.h"

class UHeritageDiscoveryManager;

// 나침반에 표시할 유물 정보
USTRUCT(BlueprintType)
struct FCompassHeritageInfo
{
	GENERATED_BODY()

	// Heritage ID
	UPROPERTY(BlueprintReadOnly)
	FString HeritageID;

	// 플레이어 정면 기준 각도 (-180 ~ 180)
	// 0도 = 정면, -90도 = 왼쪽, 90도 = 오른쪽, 180도/-180도 = 뒤
	UPROPERTY(BlueprintReadOnly)
	float AngleDegrees = 0.0f;

	// 유물까지의 거리 (cm)
	UPROPERTY(BlueprintReadOnly)
	float Distance = 0.0f;

	// 월드 좌표 (디버깅용)
	UPROPERTY(BlueprintReadOnly)
	FVector WorldLocation = FVector::ZeroVector;
};

/**
 * Compass HUD를 위한 관리자
 * - 플레이어 기준으로 유물의 방향과 거리 계산
 * - Blueprint/UMG에서 사용
 */
UCLASS()
class UE_TTTK_API ACompassManager : public AActor
{
	GENERATED_BODY()

public:
	ACompassManager();

protected:
	virtual void BeginPlay() override;

public:
	// ==================== 설정 ====================

	/** 나침반에 표시할 최대 거리 (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compass")
	float MaxDisplayDistance = 500000.0f; // 5000m

	/** 디버그 표시 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Compass|Debug")
	bool bShowDebugArrows = false;

	// ==================== Blueprint 함수 ====================

	/** 플레이어 기준 모든 유물 정보 가져오기 */
	UFUNCTION(BlueprintCallable, Category = "Compass")
	TArray<FCompassHeritageInfo> GetHeritagesForPlayer(APlayerController* Player) const;

	/** 월드의 모든 유물 개수 */
	UFUNCTION(BlueprintPure, Category = "Compass")
	int32 GetTotalHeritageCount() const;

private:
	/** HeritageDiscoveryManager 참조 */
	UPROPERTY()
	TObjectPtr<UHeritageDiscoveryManager> DiscoveryManager;

	/** 디버그 시각화 */
	virtual void Tick(float DeltaTime) override;
};
