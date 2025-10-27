#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HeritageHUDWidget.generated.h"

class UTextBlock;
struct FHeritageProgress;

/**
 * HUD 위젯 - 화면 상단에 누적 발견 점수 표시
 */
UCLASS()
class UE_TTTK_API UHeritageHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	// 진행률 업데이트 (점수, 발견 개수 등)
	UFUNCTION(BlueprintCallable, Category = "Heritage HUD")
	void UpdateProgress(const FHeritageProgress& Progress);

protected:
	// UI 요소들 (BindWidget으로 Blueprint와 연결)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CountText;

private:
	// 현재 표시 중인 점수
	int32 CurrentDisplayedScore = 0;
};
