// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "TTTK_GameState.generated.h"

/** 시간 데이터 구조체 - 델리게이트로 쉽게 전달 */
USTRUCT(BlueprintType)
struct FTimeOfDayData
{
	GENERATED_BODY()

	/** 현재 시간 (0.0 ~ 24.0) */
	UPROPERTY(BlueprintReadOnly, Category = "Time")
	float TimeInHours = 0.0f;

	/** 시 (0 ~ 23) */
	UPROPERTY(BlueprintReadOnly, Category = "Time")
	int32 Hour = 0;

	/** 분 (0 ~ 59) */
	UPROPERTY(BlueprintReadOnly, Category = "Time")
	int32 Minute = 0;

	/** 정규화된 시간 (0.0 ~ 1.0) */
	UPROPERTY(BlueprintReadOnly, Category = "Time")
	float NormalizedTime = 0.0f;

	/** 낮 시간인지 여부 */
	UPROPERTY(BlueprintReadOnly, Category = "Time")
	bool bIsDaytime = true;

	FTimeOfDayData() {}

	FTimeOfDayData(float InTimeInHours, int32 InHour, int32 InMinute, float InNormalizedTime, bool bInIsDaytime)
		: TimeInHours(InTimeInHours)
		, Hour(InHour)
		, Minute(InMinute)
		, NormalizedTime(InNormalizedTime)
		, bIsDaytime(bInIsDaytime)
	{}
};

// 시간 변경 델리게이트 - 블루프린트에서 구독 간능
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeOfDayChanged, FTimeOfDayData, TimeData);

/**
  24시간 주기 시스템을 관리하는 GameState
  - 타이머 기반 업데이트 (성능 오버헤드날까바 Tick 사용 안 함)
  - 0.5초마다 델리게이트 브로드캐스트
 */
UCLASS()
class UE_TTTK_API ATTTK_GameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	ATTTK_GameState();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 시간 업데이트 함수 - 서버만 실행 */
	UFUNCTION()
	void UpdateTimeOfDay();

	/** 델리게이트 브로드캐스트 함수 */
	UFUNCTION()
	void BroadcastTimeChange();

public:
	//~=============================================================================
	// 블루프린트 노출 프로퍼티
	//~=============================================================================

	/** 게임 시작 시간 (0.0 ~ 24.0) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Time", meta = (ClampMin = "0.0", ClampMax = "24.0"))
	float DefaultStartTime = 8.0f;

	/** 시간 배율 (현실 1초 = 인게임 TimeScale초) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Time", meta = (ClampMin = "1.0", ClampMax = "3600.0"))
	float TimeScale = 120.0f;

	/** 델리게이트 브로드캐스트 간격 (초) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Time", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float BroadcastInterval = 0.5f;

	//~=============================================================================
	// 델리게이트
	//~=============================================================================

	/** 시간 변경 시 호출되는 델리게이트 */
	UPROPERTY(BlueprintAssignable, Category = "Time")
	FOnTimeOfDayChanged OnTimeOfDayChanged;

	//~=============================================================================
	// Public API
	//~=============================================================================

	/** 현재 시간을 float으로 반환 (0.0 ~ 24.0) */
	UFUNCTION(BlueprintPure, Category = "Time")
	float GetCurrentTimeOfDay() const { return CurrentTimeOfDay; }

	/** 현재 시간을 시/분으로 반환 */
	UFUNCTION(BlueprintPure, Category = "Time")
	void GetCurrentTime(int32& OutHour, int32& OutMinute) const;

	/** 현재가 낮인지 확인 (06:00 ~ 20:00) */
	UFUNCTION(BlueprintPure, Category = "Time")
	bool IsDaytime() const;

	/** 시간 설정 (디버깅용) */
	UFUNCTION(BlueprintCallable, Category = "Time")
	void SetTimeOfDay(float NewTime);

	//~=============================================================================
	// 콘솔 명령어
	//~=============================================================================

	/** 콘솔 명령어: Time.Set [시간] */
	static void ConsoleCommand_SetTime(const TArray<FString>& Args, UWorld* World);

protected:
	/** 복제된 시간 값이 변경될 때 호출 (클라이언트) */
	UFUNCTION()
	void OnRep_CurrentTimeOfDay();

private:
	//~=============================================================================
	// 내부 상태
	//~=============================================================================

	/** 현재 시간 (0.0 ~ 24.0), 복제됨 */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentTimeOfDay)
	float CurrentTimeOfDay;

	/** 타이머 핸들 - 시간 업데이트용 */
	FTimerHandle TimeUpdateTimerHandle;

	/** 타이머 핸들 - 델리게이트 브로드캐스트용 */
	FTimerHandle BroadcastTimerHandle;

	/** 마지막으로 브로드캐스트한 분 (중복 방지) */
	int32 LastBroadcastMinute;
};
