// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseContentManager.h"

#include "JegiChagiContentManager.generated.h"

struct FJegiRoundRecord;
class AJegi;
enum class EKickTiming : uint8;

/**
 * 제기차기 Content Manager
 *
 * Server 역할:
 * - Client로부터 결과 수신 및 검증
 * - contentPlayersData에 저장
 * - Round 관리
 */
UCLASS()
class UE_TTTK_API UJegiChagiContentManager : public UBaseContentManager
{
	GENERATED_BODY()

public:
	UJegiChagiContentManager();

	virtual void InitializeConfig_Implementation() override;
	virtual void InitializeContent_Implementation(const TArray<APlayerController*>& inPlayers) override;
	virtual void StartContent_Implementation() override;
	virtual void UpdateContent_Implementation(const float& deltaTime) override;
	virtual void RoundStart_Implementation() override;
	virtual void RoundUpdate_Implementation() override;
	virtual void RoundEnd_Implementation() override;
	virtual void EndContent_Implementation() override;
	virtual void ClearContent_Implementation() override;
	
	// ========== Client로부터 호출받는 함수 (새로 추가) ==========

	/** Client로부터 킥 보고 받기 (통계용) */
	UFUNCTION()
	void OnPlayerKickReported(APlayerController* player, EKickTiming timing);

	/** Client로부터 라운드 결과 받기 */
	UFUNCTION()
	void SaveRoundResult(APlayerController* player, const FJegiRoundRecord& result, bool bSuccess);

	/** Client로부터 제기 땅 충돌 알림 받기 */
	UFUNCTION()
	void OnJegiHitGroundReported(APlayerController* player);

protected:
	virtual void GetLifetimeReplicatedProps(class TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// ========== 내부 로직 ==========

	/** 다음 라운드 시작 */
	UFUNCTION()
	void StartNextRound();

	/** 최종 순위 계산 */
	UFUNCTION()
	void CalculateFinalRankings();

	/** 제기 스폰 */
	UFUNCTION()
	void SpawnJegisForPlayers();

protected:

	/** 스폰된 제기들 (플레이어별) */
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Jegi")
	TArray<AJegi*> spawnedJegis;

	/** 제기 클래스 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Jegi")
	TSubclassOf<AJegi> jegiClass;
	
};
