#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/HeritageObjectData.h"
#include "HeritageUIController.generated.h"

class UHeritageInfoWidget;
class UHeritageHUDWidget;
class UHeritageDiscoveryManager;
struct FHeritageProgress;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UHeritageUIController : public UActorComponent
{
	GENERATED_BODY()

public:
	UHeritageUIController();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Heritage UI 위젯 클래스 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heritage UI")
	TSubclassOf<UHeritageInfoWidget> HeritageWidgetClass;

	// Heritage HUD 위젯 클래스 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heritage UI")
	TSubclassOf<UHeritageHUDWidget> HeritageHUDWidgetClass;

protected:
	// Discovery Manager의 이벤트 수신
	UFUNCTION()
	void OnHeritageDiscovered(APlayerController* Player, const FString& HeritageID, const FHeritageObjectData& HeritageData, bool bIsFirstDiscovery);

	// UI 닫기 이벤트 수신
	UFUNCTION()
	void OnHeritageUIClosed(APlayerController* Player, const FString& HeritageID);

	// 진행률 업데이트 이벤트 수신
	UFUNCTION()
	void OnProgressUpdated(APlayerController* Player, const FHeritageProgress& Progress);

	// Discovery Manager와 연동
	void BindToDiscoveryManager();

	// UI 생성
	void CreateHeritageWidget();
	void CreateHeritageHUD();

	// UI 표시
	void ShowHeritageInfo(const FHeritageObjectData& HeritageData, bool bIsFirstDiscovery);

private:
	// Heritage 정보 UI 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UHeritageInfoWidget> HeritageWidget;

	// Heritage HUD 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UHeritageHUDWidget> HeritageHUDWidget;

	// Discovery Manager 참조
	UPROPERTY()
	TObjectPtr<UHeritageDiscoveryManager> DiscoveryManager;

	// Owner PlayerController
	UPROPERTY()
	TObjectPtr<APlayerController> OwnerPC;
};
