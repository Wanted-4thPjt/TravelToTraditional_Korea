// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ClockWidget.generated.h"

class UTextBlock;
class ATTTK_GameState;

/**
 * 화면에 현재 시간을 표시하는 UI 위젯
 * - 24시간 형식 (HH:MM)
 * - GameState의 델리게이트를 구독하여 자동 업데이트
 */
UCLASS()
class UE_TTTK_API UClockWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** 시간 변경 시 호출되는 함수 */
	UFUNCTION()
	void OnTimeChanged(FTimeOfDayData TimeData);

public:
	//~=============================================================================
	// UI 요소 (BindWidget)
	//~=============================================================================

	/** 시간 텍스트 (형식: "08:45") */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimeText;

private:
	/** GameState 레퍼런스 (캐싱) */
	UPROPERTY()
	TObjectPtr<ATTTK_GameState> CachedGameState;
};
