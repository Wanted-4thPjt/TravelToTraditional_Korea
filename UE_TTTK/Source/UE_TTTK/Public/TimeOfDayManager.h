// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DirectionalLight.h"
#include "Components/DirectionalLightComponent.h"
#include "TTTK_GameState.h"
#include "TimeOfDayManager.generated.h"

UCLASS()
class UE_TTTK_API ATimeOfDayManager : public AActor
{
	GENERATED_BODY()

public:
	// 생성자
	ATimeOfDayManager();

protected:
	// 게임 시작 시 호출
	virtual void BeginPlay() override;

	// 게임 종료 시 호출
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// ==================== 라이팅 레퍼런스 ====================

	/** 태양/달빛을 표현할 디렉셔널 라이트 */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Lighting")
	TObjectPtr<ADirectionalLight> SunLight;

	// ==================== 시간대 설정 ====================

	/** 낮 시작 시간 (일출) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Settings", meta = (ClampMin = "0", ClampMax = "24"))
	float DayStartTime = 6.0f;

	/** 황혼 시작 시간 (일몰 시작) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Settings", meta = (ClampMin = "0", ClampMax = "24"))
	float DuskStartTime = 17.0f;

	/** 밤 시작 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Settings", meta = (ClampMin = "0", ClampMax = "24"))
	float NightStartTime = 19.0f;

	/** 새벽 시작 시간 (일출 전) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time Settings", meta = (ClampMin = "0", ClampMax = "24"))
	float DawnStartTime = 5.0f;

	// ==================== 태양(낮) 설정 ====================

	/** 태양 밝기 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Settings", meta = (ClampMin = "0", ClampMax = "100"))
	float SunIntensity = 10.0f;

	/** 일출 색상 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Settings")
	FLinearColor SunriseColor = FLinearColor(1.0f, 0.6f, 0.3f);

	/** 한낮 색상 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Settings")
	FLinearColor NoonColor = FLinearColor(1.0f, 1.0f, 0.97f);

	/** 일몰 색상 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sun Settings")
	FLinearColor SunsetColor = FLinearColor(1.0f, 0.5f, 0.2f);

	// ==================== 달(밤) 설정 ====================

	/** 달 밝기 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moon Settings", meta = (ClampMin = "0", ClampMax = "10"))
	float MoonIntensity = 0.6f;

	/** 달 색상 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moon Settings")
	FLinearColor MoonColor = FLinearColor(0.6f, 0.7f, 1.0f);

	// ==================== 전환 설정 ====================

	/** 황혼/새벽 지속 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition Settings", meta = (ClampMin = "0.1", ClampMax = "5.0"))
	float TransitionDuration = 2.0f;

	/** 리셋 시 페이드 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transition Settings", meta = (ClampMin = "0.1", ClampMax = "2.0"))
	float ResetFadeDuration = 0.5f;

private:
	// ==================== 계산 함수 ====================

	/** 시간에 따른 Pitch 각도 계산 */
	float CalculatePitch(float TimeOfDay) const;

	/** 시간에 따른 빛 강도 계산 */
	float CalculateIntensity(float TimeOfDay) const;

	/** 시간에 따른 빛 색상 계산 */
	FLinearColor CalculateColor(float TimeOfDay) const;

	/** 라이팅 업데이트 */
	void UpdateLighting(float TimeOfDay);

	/** 시간 변경 델리게이트 콜백 */
	UFUNCTION()
	void OnTimeOfDayChanged(FTimeOfDayData TimeData);

	/** 시간대 설정 유효성 검증 */
	void ValidateTimeSettings() const;
};
