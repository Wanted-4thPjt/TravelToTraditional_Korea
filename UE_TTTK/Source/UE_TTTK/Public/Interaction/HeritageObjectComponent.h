#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/HeritageObjectData.h"
#include "Interaction/InteractableComponent.h"
#include "HeritageObjectComponent.generated.h"

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

	// Heritage 데이터 가져오기
	UFUNCTION(BlueprintPure, Category = "Heritage")
	FHeritageObjectData GetHeritageData() const;

	// 이 플레이어가 발견했는지 확인
	UFUNCTION(BlueprintPure, Category = "Heritage")
	bool HasPlayerDiscovered(APlayerController* Player) const;

protected:
	// InteractableComponent의 상호작용 이벤트 수신 (클라이언트)
	UFUNCTION()
	void OnClientInteraction(APlayerController* PlayerController);

	// InteractableComponent의 상호작용 이벤트 수신 (멀티캐스트)
	UFUNCTION()
	void OnMultiInteraction(APawn* InteractingPlayer);

	// InteractableComponent의 상태 변경 이벤트 수신
	UFUNCTION()
	void OnInteractableStateChanged(APlayerController* PlayerController, EInteractableState NewState);

	// 서버에서 발견 처리
	UFUNCTION(Server, Reliable)
	void Server_ProcessDiscovery(APlayerController* PlayerController, const FString& HeritageID);

	// 클라이언트에서 UI 표시
	UFUNCTION(Client, Reliable)
	void Client_ShowHeritageUI(APlayerController* PlayerController, const FString& HeritageID, const FHeritageObjectData& HeritageData, bool bIsFirstDiscovery);

	// UI 닫기 이벤트 수신
	UFUNCTION()
	void OnUICloseRequested(APlayerController* Player, const FString& HeritageID);

	// InteractableComponent와 연결
	void BindToInteractableComponent();

private:
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
