#include "UI/HeritageHUDWidget.h"
#include "Components/TextBlock.h"
#include "GameFlow/HeritageDiscoveryManager.h"

void UHeritageHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기 상태: 0점 표시
	if (ScoreText)
	{
		ScoreText->SetText(FText::FromString(TEXT("0점")));
	}

	if (CountText)
	{
		CountText->SetText(FText::FromString(TEXT("0/0")));
	}
}

void UHeritageHUDWidget::UpdateProgress(const FHeritageProgress& Progress)
{
	// 점수 업데이트
	if (ScoreText)
	{
		FText ScoreDisplayText = FText::Format(
			FText::FromString(TEXT("{0}점")),
			FText::AsNumber(Progress.TotalScore)
		);
		ScoreText->SetText(ScoreDisplayText);
		CurrentDisplayedScore = Progress.TotalScore;
	}

	// 발견 개수 업데이트 (Optional)
	if (CountText)
	{
		FText CountDisplayText = FText::Format(
			FText::FromString(TEXT("{0}/{1}")),
			FText::AsNumber(Progress.FoundCount),
			FText::AsNumber(Progress.TotalCount)
		);
		CountText->SetText(CountDisplayText);
	}
}
