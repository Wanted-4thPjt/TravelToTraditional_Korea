#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/HeritageObjectData.h"
#include "HeritageInfoWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class UE_TTTK_API UHeritageInfoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

public:
	// Heritage 정보 설정 (외부에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Heritage UI")
	void SetHeritageData(const FHeritageObjectData& HeritageData, bool bIsFirstDiscovery);

	// 위젯 표시/숨김
	UFUNCTION(BlueprintCallable, Category = "Heritage UI")
	void ShowWidget();

	UFUNCTION(BlueprintCallable, Category = "Heritage UI")
	void HideWidget();

	// 거리가 멀어졌을 때 호출
	UFUNCTION(BlueprintCallable, Category = "Heritage UI")
	void OnPlayerMovedAway();

protected:
	// UI 요소들 (BindWidget으로 Blueprint와 연결)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ObjectNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DescriptionText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreText;

	// 닫기 버튼 (Optional)
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> CloseButton;

	// 첫 발견 알림 텍스트 (Optional)
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> FirstDiscoveryText;

	// 닫기 버튼 클릭 이벤트
	UFUNCTION()
	void OnCloseButtonClicked();
};
