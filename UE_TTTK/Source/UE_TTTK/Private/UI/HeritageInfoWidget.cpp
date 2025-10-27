#include "UI/HeritageInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UHeritageInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 초기 상태: 숨김
	SetVisibility(ESlateVisibility::Hidden);

	// 닫기 버튼 이벤트 연결
	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UHeritageInfoWidget::OnCloseButtonClicked);
	}
}

void UHeritageInfoWidget::NativeDestruct()
{
	// 닫기 버튼 이벤트 해제
	if (CloseButton)
	{
		CloseButton->OnClicked.RemoveDynamic(this, &UHeritageInfoWidget::OnCloseButtonClicked);
	}

	Super::NativeDestruct();
}

FReply UHeritageInfoWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	// ESC 키로 닫기
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		HideWidget();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UHeritageInfoWidget::SetHeritageData(const FHeritageObjectData& HeritageData, bool bIsFirstDiscovery)
{
	// 텍스트 설정
	if (ObjectNameText)
	{
		ObjectNameText->SetText(HeritageData.ObjectName);
	}

	if (DescriptionText)
	{
		DescriptionText->SetText(HeritageData.Description);
	}

	if (ScoreText)
	{
		if (bIsFirstDiscovery)
		{
			// 첫 발견시 점수 표시
			FText ScoreDisplayText = FText::Format(
				FText::FromString(TEXT("+{0}점")),
				FText::AsNumber(HeritageData.DiscoveryScore)
			);
			ScoreText->SetText(ScoreDisplayText);
			ScoreText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			// 재확인시 점수 표시 안함
			ScoreText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 첫 발견 알림 표시
	if (FirstDiscoveryText)
	{
		if (bIsFirstDiscovery)
		{
			FirstDiscoveryText->SetText(FText::FromString(TEXT("새로운 유물 발견!")));
			FirstDiscoveryText->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			FirstDiscoveryText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UHeritageInfoWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);

	// 입력 모드 설정 (UI에 포커스)
	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}
}

void UHeritageInfoWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Hidden);

	// 입력 모드 복원 (게임으로)
	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = false;
	}
}

void UHeritageInfoWidget::OnPlayerMovedAway()
{
	// 플레이어가 멀어지면 UI 닫기
	HideWidget();
}

void UHeritageInfoWidget::OnCloseButtonClicked()
{
	HideWidget();
}
