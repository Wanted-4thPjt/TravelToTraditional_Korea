#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/HeritageObjectData.h"
#include "HeritageObjectComponent.generated.h"

class UInteractableComponent;
class UHeritageDiscoveryManager;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UHeritageObjectComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHeritageObjectComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// 블루프린트에서 설정할 Heritage ID
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heritage")
	FString HeritageObjectID;

	// 커스텀 아웃라인 머티리얼 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heritage|Custom Outline")
	bool bUseCustomOutline = false;

	// 오버레이 방식 사용 (true: Overlay Material, false: 머티리얼 교체)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heritage|Custom Outline", meta=(EditCondition="bUseCustomOutline"))
	bool bUseOverlayMaterial = true;

	// 커스텀 아웃라인 머티리얼 (설정 시 메시에 오버라이드 또는 오버레이)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heritage|Custom Outline", meta=(EditCondition="bUseCustomOutline"))
	TObjectPtr<UMaterialInterface> CustomOutlineMaterial;

	// Heritage 데이터 가져오기
	UFUNCTION(BlueprintPure, Category = "Heritage")
	FHeritageObjectData GetHeritageData() const;

	// 이 플레이어가 발견했는지 확인
	UFUNCTION(BlueprintPure, Category = "Heritage")
	bool HasPlayerDiscovered(APlayerController* Player) const;

protected:
	// InteractableComponent의 상호작용 이벤트 수신
	UFUNCTION()
	void OnInteraction(APawn* InteractingPlayer);

	// InteractableComponent의 상태 변경 이벤트 수신 (거리 체크용)
	UFUNCTION()
	void OnStateChanged(APlayerController* PlayerController, const EInteractableState& NewState);

	// UI 닫기 이벤트 수신 (재확인 가능하도록 상호작용 종료)
	UFUNCTION()
	void OnUICloseRequested(APlayerController* Player, const FString& HeritageID);

	// 서버에서 상호작용 종료 처리
	UFUNCTION(Server, Reliable)
	void Server_FinishInteraction(APlayerController* Player);

	// 발견 처리
	void ProcessDiscovery(APawn* Player);

	// InteractableComponent와 연결
	void BindToInteractableComponent();

	// 커스텀 아웃라인 업데이트
	void UpdateCustomOutline(bool bShowOutline);

private:
	// 원본 머티리얼 백업 (커스텀 아웃라인 사용 시)
	UPROPERTY()
	TArray<TObjectPtr<UMaterialInterface>> OriginalMaterials;
	// 같은 액터의 InteractableComponent 참조
	UPROPERTY()
	TObjectPtr<UInteractableComponent> CachedInteractableComponent;

	// Discovery Manager 참조
	UPROPERTY()
	TObjectPtr<UHeritageDiscoveryManager> DiscoveryManager;

	// 현재 UI를 보고 있는 플레이어들
	UPROPERTY()
	TSet<APlayerController*> ViewingPlayers;

	// 현재 상호작용 중인지 여부
	bool bIsInteracting = false;
};
