// Fill out your copyright notice in the Description page of Project Settings.

#include "MusicManager.h"
#include "TTTK_GameState.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AMusicManager::AMusicManager()
{
	PrimaryActorTick.bCanEverTick = false;

	// AudioComponent 1개만 생성 (심플 버전)
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->bAutoActivate = false;
	AudioComponent->SetVolumeMultiplier(MaxVolume);
}

void AMusicManager::BeginPlay()
{
	Super::BeginPlay();

	// GameState 가져오기
	ATTTK_GameState* GameState = Cast<ATTTK_GameState>(UGameplayStatics::GetGameState(this));
	if (!GameState)
	{
		return;
	}

	// 시간 변경 델리게이트 구독
	GameState->OnTimeOfDayChanged.AddDynamic(this, &AMusicManager::OnTimeOfDayChanged);

	// 현재 시간 확인 (MusicManager 설정값 사용)
	float CurrentTime = GameState->GetCurrentTimeOfDay();
	bIsCurrentlyDaytime = IsMusicDaytime(CurrentTime);
	bWasDaytimeLastCheck = bIsCurrentlyDaytime;

	// 자동 재생 옵션이 켜져있으면 시작
	if (bAutoPlayOnStart)
	{
		PlayRandomMusic(bIsCurrentlyDaytime, true);
	}
}

void AMusicManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// GameState 델리게이트 해제
	ATTTK_GameState* GameState = Cast<ATTTK_GameState>(UGameplayStatics::GetGameState(this));
	if (GameState)
	{
		GameState->OnTimeOfDayChanged.RemoveDynamic(this, &AMusicManager::OnTimeOfDayChanged);
	}

	// 타이머 정리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(FadeOutCompleteTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void AMusicManager::OnTimeOfDayChanged(FTimeOfDayData TimeData)
{
	// MusicManager의 설정값으로 낮/밤 판단
	bool bShouldBeDaytime = IsMusicDaytime(TimeData.TimeInHours);

	// 시간대 전환 감지 (낮→밤 또는 밤→낮)
	if (bWasDaytimeLastCheck != bShouldBeDaytime)
	{
		bWasDaytimeLastCheck = bShouldBeDaytime;
		bIsCurrentlyDaytime = bShouldBeDaytime;

		PlayRandomMusic(bShouldBeDaytime, true);
	}
}

void AMusicManager::PlayRandomMusic(bool bIsDaytime, bool bUseFade)
{
	// 재생할 음악 리스트 선택
	const TArray<TObjectPtr<USoundBase>>& MusicList = bIsDaytime ? DayMusicList : NightMusicList;

	if (MusicList.Num() == 0)
	{
			bIsDaytime ? TEXT("Day") : TEXT("Night");
		return;
	}

	// 랜덤 음악 선택
	int32& LastPlayedIndex = bIsDaytime ? LastPlayedDayIndex : LastPlayedNightIndex;
	int32 SelectedIndex = GetRandomMusicIndex(MusicList, LastPlayedIndex);

	if (!MusicList.IsValidIndex(SelectedIndex))
	{
		return;
	}

	USoundBase* SelectedMusic = MusicList[SelectedIndex];
	if (!SelectedMusic)
	{
		return;
	}

	// 인덱스 업데이트
	LastPlayedIndex = SelectedIndex;

	// 디버깅 정보 업데이트
	CurrentTrackName = FString::Printf(TEXT("[%s] %s"),
		bIsDaytime ? TEXT("Day") : TEXT("Night"),
		*SelectedMusic->GetName());


	// 현재 재생 중이면 페이드아웃 후 재생
	if (bUseFade && AudioComponent->IsPlaying())
	{
		// 다음 재생할 음악 저장
		PendingMusic = SelectedMusic;

		// 페이드아웃 시작 (UE 내장 함수 사용!)
		AudioComponent->FadeOut(FadeOutDuration, 0.0f);

		// 페이드아웃 완료 후 새 음악 재생 예약
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(
				FadeOutCompleteTimerHandle,
				this,
				&AMusicManager::OnFadeOutComplete,
				FadeOutDuration,
				false
			);
		}
	}
	else
	{
		// 바로 재생 (첫 재생 또는 페이드 없이)
		AudioComponent->SetSound(SelectedMusic);
		AudioComponent->OnAudioFinished.Clear();
		AudioComponent->OnAudioFinished.AddDynamic(this, &AMusicManager::OnMusicFinished);

		if (bUseFade)
		{
			// 페이드인으로 재생 (UE 내장 함수 사용!)
			AudioComponent->FadeIn(FadeInDuration, MaxVolume);
		}
		else
		{
			// 페이드 없이 바로 재생
			AudioComponent->SetVolumeMultiplier(MaxVolume);
			AudioComponent->Play();
		}
	}
}

int32 AMusicManager::GetRandomMusicIndex(const TArray<TObjectPtr<USoundBase>>& MusicList, int32 LastPlayedIndex) const
{
	if (MusicList.Num() == 0)
	{
		return -1;
	}

	// 리스트에 1곡만 있으면 그것을 반환
	if (MusicList.Num() == 1)
	{
		return 0;
	}

	// 중복 방지 옵션이 켜져있고, 리스트가 2곡 이상일 때
	if (bAvoidRepeatingLastTrack && MusicList.Num() >= 2 && LastPlayedIndex != -1)
	{
		int32 RandomIndex;
		do
		{
			RandomIndex = FMath::RandRange(0, MusicList.Num() - 1);
		} while (RandomIndex == LastPlayedIndex);

		return RandomIndex;
	}

	// 일반 랜덤 선택
	return FMath::RandRange(0, MusicList.Num() - 1);
}

void AMusicManager::OnFadeOutComplete()
{
	// 페이드아웃 완료 후 새 음악 재생
	if (PendingMusic)
	{
		AudioComponent->Stop();
		AudioComponent->SetSound(PendingMusic);
		AudioComponent->OnAudioFinished.Clear();
		AudioComponent->OnAudioFinished.AddDynamic(this, &AMusicManager::OnMusicFinished);
		AudioComponent->FadeIn(FadeInDuration, MaxVolume);

		PendingMusic = nullptr;
	}
}

void AMusicManager::OnMusicFinished()
{
	// 곡이 끝나면 자동으로 다음 곡 재생
	PlayRandomMusic(bIsCurrentlyDaytime, false);
}

bool AMusicManager::IsMusicDaytime(float TimeInHours) const
{
	// DayMusicStartTime 이상, NightMusicStartTime 미만이면 낮
	return TimeInHours >= DayMusicStartTime && TimeInHours < NightMusicStartTime;
}
