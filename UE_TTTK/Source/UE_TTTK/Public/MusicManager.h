// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "TTTK_GameState.h"
#include "MusicManager.generated.h"

/**
 * 낮/밤 배경음악을 관리하는 Manager
 * - 시간대별 음악 리스트 관리
 * - 랜덤 재생 및 자동 연속 재생
 * - 시간 전환 시 페이드인/아웃 처리
 */
UCLASS()
class UE_TTTK_API AMusicManager : public AActor
{
	GENERATED_BODY()

public:
	AMusicManager();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// ==================== 음악 리스트 ====================

	/** 낮에 재생할 음악 리스트 */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Music|Day")
	TArray<TObjectPtr<USoundBase>> DayMusicList;

	/** 밤에 재생할 음악 리스트 */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Music|Night")
	TArray<TObjectPtr<USoundBase>> NightMusicList;

	// ==================== 시간 전환 설정 ====================

	/** 낮 음악으로 전환할 시간 (게임 내 시간, 0-24) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Time", meta = (ClampMin = "0", ClampMax = "24"))
	float DayMusicStartTime = 6.0f;

	/** 밤 음악으로 전환할 시간 (게임 내 시간, 0-24) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Time", meta = (ClampMin = "0", ClampMax = "24"))
	float NightMusicStartTime = 19.0f;

	// ==================== 페이드 설정 ====================

	/** 페이드인 지속 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Fade", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float FadeInDuration = 3.0f;

	/** 페이드아웃 지속 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Fade", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float FadeOutDuration = 3.0f;

	/** 최대 볼륨 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Volume", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxVolume = 0.5f;

	// ==================== 재생 옵션 ====================

	/** 바로 직전에 재생한 곡 제외 (리스트가 2곡 이상일 때만 적용) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Options")
	bool bAvoidRepeatingLastTrack = true;

	/** 게임 시작 시 자동 재생 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music|Options")
	bool bAutoPlayOnStart = true;

	// ==================== 디버깅 ====================

	/** 현재 재생 중인 음악 이름 (디버깅용) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Music|Debug")
	FString CurrentTrackName;

	/** 현재 낮 시간대인지 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Music|Debug")
	bool bIsCurrentlyDaytime = true;

private:
	// ==================== Audio Component ====================

	/** 오디오 컴포넌트 (단일) */
	UPROPERTY()
	TObjectPtr<UAudioComponent> AudioComponent;

	// ==================== 상태 추적 ====================

	/** 마지막으로 재생한 음악 인덱스 (중복 방지용) */
	int32 LastPlayedDayIndex = -1;
	int32 LastPlayedNightIndex = -1;

	/** 이전 프레임의 낮/밤 상태 (전환 감지용) */
	bool bWasDaytimeLastCheck = true;

	/** 다음 재생 대기 중인 음악 (페이드아웃 후 재생용) */
	UPROPERTY()
	TObjectPtr<USoundBase> PendingMusic = nullptr;

	// ==================== 내부 함수 ====================

	/** GameState의 시간 변경 델리게이트 콜백 */
	UFUNCTION()
	void OnTimeOfDayChanged(FTimeOfDayData TimeData);

	/** 음악 종료 콜백 */
	UFUNCTION()
	void OnMusicFinished();

	/** 랜덤 음악 선택 및 재생 */
	void PlayRandomMusic(bool bIsDaytime, bool bUseFade);

	/** 음악 리스트에서 랜덤 인덱스 선택 */
	int32 GetRandomMusicIndex(const TArray<TObjectPtr<USoundBase>>& MusicList, int32 LastPlayedIndex) const;

	/** 페이드아웃 완료 후 새 음악 재생 */
	void OnFadeOutComplete();

	/** 현재 시간이 낮 음악 시간대인지 확인 */
	bool IsMusicDaytime(float TimeInHours) const;

	// ==================== 타이머 핸들 ====================

	FTimerHandle FadeOutCompleteTimerHandle;
};
