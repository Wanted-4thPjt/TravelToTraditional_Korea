#include "Interaction/HeritageUIController.h"
#include "UI/HeritageInfoWidget.h"
#include "UI/HeritageHUDWidget.h"
#include "GameFlow/HeritageDiscoveryManager.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"

UHeritageUIController::UHeritageUIController()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHeritageUIController::BeginPlay()
{
	Super::BeginPlay();

	// Owner PlayerController 가져오기
	OwnerPC = Cast<APlayerController>(GetOwner());
	if (!OwnerPC)
	{
		UE_LOG(LogTemp, Error, TEXT("[Heritage] UIController는 PlayerController에 부착되어야 합니다"));
		return;
	}

	// 로컬 플레이어만 UI 생성
	if (OwnerPC->IsLocalController())
	{
		// Discovery Manager와 연동
		BindToDiscoveryManager();

		// UI 위젯 생성
		CreateHeritageWidget();
		CreateHeritageHUD();
	}
}

void UHeritageUIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Discovery Manager 이벤트 해제
	if (DiscoveryManager)
	{
		DiscoveryManager->OnHeritageDiscovered.RemoveDynamic(
			this, &UHeritageUIController::OnHeritageDiscovered);
		DiscoveryManager->OnHeritageUIClose.RemoveDynamic(
			this, &UHeritageUIController::OnHeritageUIClosed);
		DiscoveryManager->OnProgressUpdated.RemoveDynamic(
			this, &UHeritageUIController::OnProgressUpdated);
	}

	Super::EndPlay(EndPlayReason);
}

void UHeritageUIController::BindToDiscoveryManager()
{
	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		DiscoveryManager = GameInstance->GetSubsystem<UHeritageDiscoveryManager>();

		if (DiscoveryManager)
		{
			// 발견 이벤트에 바인딩
			DiscoveryManager->OnHeritageDiscovered.AddDynamic(
				this, &UHeritageUIController::OnHeritageDiscovered);

			// UI 닫기 이벤트에 바인딩
			DiscoveryManager->OnHeritageUIClose.AddDynamic(
				this, &UHeritageUIController::OnHeritageUIClosed);

			// 진행률 업데이트 이벤트에 바인딩
			DiscoveryManager->OnProgressUpdated.AddDynamic(
				this, &UHeritageUIController::OnProgressUpdated);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Heritage] DiscoveryManager를 찾을 수 없습니다"));
		}
	}
}

void UHeritageUIController::CreateHeritageWidget()
{
	if (!HeritageWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Heritage] HeritageWidgetClass가 설정되지 않았습니다"));
		return;
	}

	// 위젯 생성
	HeritageWidget = CreateWidget<UHeritageInfoWidget>(OwnerPC, HeritageWidgetClass);
	if (HeritageWidget)
	{
		HeritageWidget->AddToViewport(100); // 높은 Z-Order로 추가
	}
}

void UHeritageUIController::OnHeritageDiscovered(APlayerController* Player, const FString& HeritageID, const FHeritageObjectData& HeritageData, bool bIsFirstDiscovery)
{
	// 이 컨트롤러가 관리하는 플레이어인지 확인
	if (Player != OwnerPC)
	{
		return;
	}

	// UI 표시
	ShowHeritageInfo(HeritageData, bIsFirstDiscovery);
}

void UHeritageUIController::ShowHeritageInfo(const FHeritageObjectData& HeritageData, bool bIsFirstDiscovery)
{
	if (!HeritageWidget)
	{
		return;
	}

	// 위젯에 데이터 설정
	HeritageWidget->SetHeritageData(HeritageData, bIsFirstDiscovery);

	// 위젯 표시
	HeritageWidget->ShowWidget();
}

void UHeritageUIController::OnHeritageUIClosed(APlayerController* Player, const FString& HeritageID)
{
	// 이 컨트롤러가 관리하는 플레이어인지 확인
	if (Player != OwnerPC)
	{
		return;
	}

	// UI 닫기
	if (HeritageWidget)
	{
		HeritageWidget->HideWidget();
	}
}

void UHeritageUIController::OnProgressUpdated(APlayerController* Player, const FHeritageProgress& Progress)
{
	// 이 컨트롤러가 관리하는 플레이어인지 확인
	if (Player != OwnerPC)
	{
		return;
	}

	// HUD 업데이트
	if (HeritageHUDWidget)
	{
		HeritageHUDWidget->UpdateProgress(Progress);
	}
}

void UHeritageUIController::CreateHeritageHUD()
{
	if (!HeritageHUDWidgetClass)
	{
		return;
	}

	// HUD 위젯 생성
	HeritageHUDWidget = CreateWidget<UHeritageHUDWidget>(OwnerPC, HeritageHUDWidgetClass);
	if (HeritageHUDWidget)
	{
		HeritageHUDWidget->AddToViewport(10); // 낮은 Z-Order로 배경에 표시

		// 현재 진행률로 초기화
		if (DiscoveryManager)
		{
			FHeritageProgress Progress = DiscoveryManager->GetPlayerProgress(OwnerPC);
			HeritageHUDWidget->UpdateProgress(Progress);
		}
	}
}
