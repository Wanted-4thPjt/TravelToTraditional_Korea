#include "TimeOfDayManager.h"
#include "TTTK_GameState.h"
#include "Kismet/GameplayStatics.h"

ATimeOfDayManager::ATimeOfDayManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATimeOfDayManager::BeginPlay()
{
	Super::BeginPlay();

	ValidateTimeSettings();

	ATTTK_GameState* GameState = Cast<ATTTK_GameState>(UGameplayStatics::GetGameState(this));
	if (!GameState)
	{
		UE_LOG(LogTemp, Error, TEXT("TimeOfDayManager: TTTK_GameState not found"));
		return;
	}

	GameState->OnTimeOfDayChanged.AddDynamic(this, &ATimeOfDayManager::OnTimeOfDayChanged);
	UpdateLighting(GameState->GetCurrentTimeOfDay());
}

void ATimeOfDayManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ATTTK_GameState* GameState = Cast<ATTTK_GameState>(UGameplayStatics::GetGameState(this));
	if (GameState)
	{
		GameState->OnTimeOfDayChanged.RemoveDynamic(this, &ATimeOfDayManager::OnTimeOfDayChanged);
	}

	Super::EndPlay(EndPlayReason);
}

// 델리게이트 콜백
void ATimeOfDayManager::OnTimeOfDayChanged(FTimeOfDayData TimeData)
{
	UpdateLighting(TimeData.TimeInHours);
}

// 시간대 설정 유효성 검증
void ATimeOfDayManager::ValidateTimeSettings() const
{
	if (DawnStartTime >= DayStartTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("TimeOfDayManager: DawnStartTime(%.1f) must be < DayStartTime(%.1f)"), DawnStartTime, DayStartTime);
	}

	if (DayStartTime >= DuskStartTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("TimeOfDayManager: DayStartTime(%.1f) must be < DuskStartTime(%.1f)"), DayStartTime, DuskStartTime);
	}

	if (DuskStartTime >= NightStartTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("TimeOfDayManager: DuskStartTime(%.1f) must be < NightStartTime(%.1f)"), DuskStartTime, NightStartTime);
	}

	if (NightStartTime <= DawnStartTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("TimeOfDayManager: NightStartTime(%.1f) must be > DawnStartTime(%.1f)"), NightStartTime, DawnStartTime);
	}
}

// 시간대별 태양 각도 계산
float ATimeOfDayManager::CalculatePitch(float TimeOfDay) const
{
	const float DuskDuration = NightStartTime - DuskStartTime;
	const float DawnDuration = DayStartTime - DawnStartTime;
	const float DayDuration = DuskStartTime - DayStartTime;
	const float NightDuration = (24.0f - NightStartTime) + DawnStartTime;

	// 낮
	if (TimeOfDay >= DayStartTime && TimeOfDay < DuskStartTime)
	{
		const float Progress = (TimeOfDay - DayStartTime) / DayDuration;
		return Progress * -170.0f;
	}
	// 황혼
	else if (TimeOfDay >= DuskStartTime && TimeOfDay < NightStartTime)
	{
		const float Progress = (TimeOfDay - DuskStartTime) / DuskDuration;
		return -170.0f + (Progress * -10.0f);
	}
	// 밤
	else if (TimeOfDay >= NightStartTime || TimeOfDay < DawnStartTime)
	{
		const float AdjustedTime = TimeOfDay >= NightStartTime ?
			TimeOfDay - NightStartTime : TimeOfDay + (24.0f - NightStartTime);
		const float Progress = AdjustedTime / NightDuration;
		return Progress * -170.0f;
	}
	// 새벽
	else if (TimeOfDay >= DawnStartTime && TimeOfDay < DayStartTime)
	{
		const float Progress = (TimeOfDay - DawnStartTime) / DawnDuration;
		return -170.0f + (Progress * -10.0f);
	}

	return 0.0f;
}

// 시간대별 빛 강도 계산
float ATimeOfDayManager::CalculateIntensity(float TimeOfDay) const
{
	const float DuskDuration = NightStartTime - DuskStartTime;
	const float DawnDuration = DayStartTime - DawnStartTime;
	const float ResetFadeHours = ResetFadeDuration / 60.0f;

	// 낮 (일출 페이드 인)
	if (TimeOfDay >= DayStartTime && TimeOfDay < DuskStartTime)
	{
		if (TimeOfDay < DayStartTime + ResetFadeHours)
		{
			const float Progress = (TimeOfDay - DayStartTime) / ResetFadeHours;
			return FMath::Lerp(0.0f, SunIntensity, Progress);
		}
		return SunIntensity;
	}
	// 황혼 (페이드 아웃)
	else if (TimeOfDay >= DuskStartTime && TimeOfDay < NightStartTime)
	{
		const float Progress = (TimeOfDay - DuskStartTime) / DuskDuration;
		return FMath::Lerp(SunIntensity, 0.0f, Progress);
	}
	// 밤 시작 (페이드 인)
	else if (TimeOfDay >= NightStartTime && TimeOfDay < NightStartTime + ResetFadeHours)
	{
		const float Progress = (TimeOfDay - NightStartTime) / ResetFadeHours;
		return FMath::Lerp(0.0f, MoonIntensity, Progress);
	}
	// 밤
	else if (TimeOfDay >= NightStartTime || TimeOfDay < DawnStartTime)
	{
		return MoonIntensity;
	}
	// 새벽 (페이드 아웃)
	else if (TimeOfDay >= DawnStartTime && TimeOfDay < DayStartTime)
	{
		const float Progress = (TimeOfDay - DawnStartTime) / DawnDuration;
		return FMath::Lerp(MoonIntensity, 0.0f, Progress);
	}

	return 0.0f;
}

// 시간대별 빛 색상 계산
FLinearColor ATimeOfDayManager::CalculateColor(float TimeOfDay) const
{
	const float DuskDuration = NightStartTime - DuskStartTime;
	const float DawnDuration = DayStartTime - DawnStartTime;
	const float DayDuration = DuskStartTime - DayStartTime;

	// 낮 전반 (일출 → 정오)
	if (TimeOfDay >= DayStartTime && TimeOfDay < DayStartTime + (DayDuration / 2.0f))
	{
		const float Progress = (TimeOfDay - DayStartTime) / (DayDuration / 2.0f);
		return FMath::Lerp(SunriseColor, NoonColor, Progress);
	}
	// 낮 후반 (정오 유지)
	else if (TimeOfDay >= DayStartTime + (DayDuration / 2.0f) && TimeOfDay < DuskStartTime)
	{
		return NoonColor;
	}
	// 황혼 (정오 → 일몰 → 달빛)
	else if (TimeOfDay >= DuskStartTime && TimeOfDay < NightStartTime)
	{
		const float Progress = (TimeOfDay - DuskStartTime) / DuskDuration;

		if (Progress < 0.5f)
		{
			return FMath::Lerp(NoonColor, SunsetColor, Progress * 2.0f);
		}
		else
		{
			return FMath::Lerp(SunsetColor, MoonColor, (Progress - 0.5f) * 2.0f);
		}
	}
	// 밤
	else if (TimeOfDay >= NightStartTime || TimeOfDay < DawnStartTime)
	{
		return MoonColor;
	}
	// 새벽 (달빛 → 일출)
	else if (TimeOfDay >= DawnStartTime && TimeOfDay < DayStartTime)
	{
		const float Progress = (TimeOfDay - DawnStartTime) / DawnDuration;
		return FMath::Lerp(MoonColor, SunriseColor, Progress);
	}

	return FLinearColor::White;
}

// 라이팅 업데이트
void ATimeOfDayManager::UpdateLighting(float TimeOfDay)
{
	if (!SunLight) return;

	UDirectionalLightComponent* LightComp = SunLight->GetComponent();
	if (!LightComp) return;

	const float Pitch = CalculatePitch(TimeOfDay);
	const float Intensity = CalculateIntensity(TimeOfDay);
	const FLinearColor Color = CalculateColor(TimeOfDay);

	SunLight->SetActorRotation(FRotator(Pitch, -90.0f, 0.0f));
	LightComp->SetIntensity(Intensity);
	LightComp->SetLightColor(Color);
}
