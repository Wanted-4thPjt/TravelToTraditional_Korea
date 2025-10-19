#include "UI/ClockWidget.h"
#include "TTTK_GameState.h"
#include "Components/TextBlock.h"

void UClockWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!TimeText)
	{
		UE_LOG(LogTemp, Error, TEXT("ClockWidget: TimeText not bound"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	CachedGameState = World->GetGameState<ATTTK_GameState>();
	if (!CachedGameState)
	{
		UE_LOG(LogTemp, Warning, TEXT("ClockWidget: TTTK_GameState not found"));
		return;
	}

	// 델리게이트 구독
	CachedGameState->OnTimeOfDayChanged.AddDynamic(this, &UClockWidget::OnTimeChanged);

	// 초기 시간 표시
	FTimeOfDayData InitialData;
	int32 Hour, Minute;
	CachedGameState->GetCurrentTime(Hour, Minute);
	InitialData.TimeInHours = CachedGameState->GetCurrentTimeOfDay();
	InitialData.Hour = Hour;
	InitialData.Minute = Minute;
	InitialData.NormalizedTime = InitialData.TimeInHours / 24.0f;
	InitialData.bIsDaytime = CachedGameState->IsDaytime();

	OnTimeChanged(InitialData);
}

void UClockWidget::NativeDestruct()
{
	if (CachedGameState)
	{
		CachedGameState->OnTimeOfDayChanged.RemoveDynamic(this, &UClockWidget::OnTimeChanged);
	}

	Super::NativeDestruct();
}

// 시간 변경 콜백 - UI 텍스트 업데이트
void UClockWidget::OnTimeChanged(FTimeOfDayData TimeData)
{
	if (!TimeText) return;

	FString TimeString = FString::Printf(TEXT("%02d:%02d"), TimeData.Hour, TimeData.Minute);
	TimeText->SetText(FText::FromString(TimeString));
}
