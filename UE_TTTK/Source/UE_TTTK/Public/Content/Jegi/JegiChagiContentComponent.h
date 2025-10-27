// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Content/BaseContentComponent.h"
#include "JegiChagiContentComponent.generated.h"

class UJegiChagiContentManager;
class UInputAction;
class AJegi;
enum class EKickTiming : uint8;

/**
 * 제기차기 라운드별 기록
 */
USTRUCT(BlueprintType)
struct FJegiRoundRecord
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 maxCombo = 0;  // 최고 콤보

	UPROPERTY(BlueprintReadOnly)
	int32 perfectCount = 0;  // Perfect 횟수

	UPROPERTY(BlueprintReadOnly)
	int32 greatCount = 0;  // Great 횟수

	UPROPERTY(BlueprintReadOnly)
	int32 goodCount = 0;  // Good 횟수

	UPROPERTY(BlueprintReadOnly)
	int32 missedCount = 0;  // Missed 횟수

	UPROPERTY(BlueprintReadOnly)
	int32 totalKicks = 0;  // 총 킥 횟수

	UPROPERTY(BlueprintReadOnly)
	float playTime = 0.f;  // 플레이 시간
};

/**
 * 제기차기 Input 및 게임 로직 Component
 *
 * Client Authority 방식:
 * - Client에서 타이밍 판정, 콤보 계산, 물리 시뮬레이션 모두 수행
 * - Server에는 결과만 전송하여 검증 및 저장
 * - 지연 없는 즉각적 피드백
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UJegiChagiContentComponent : public UBaseContentComponent
{
	GENERATED_BODY()

public:
	UJegiChagiContentComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// ========== 생명주기 오버라이드 ==========

	virtual void ActivateContentInput_Implementation(UBaseContentManager* manager) override;
	virtual void DeactivateContentInput_Implementation() override;
	virtual void SetupInputBindings_Implementation(UEnhancedInputComponent* InputComponent) override;

	// ========== Public Interface ==========

	/** 제기 설정 (Manager에서 호출) */
	UFUNCTION(BlueprintCallable, Category="JegiChagi")
	void SetMyJegi(AJegi* jegi);

	/** 라운드 시작 (Manager에서 호출) */
	UFUNCTION(BlueprintCallable, Category="JegiChagi")
	void StartRound();

protected:
	// ========== Input 처리 (Client) ==========

	/** 오른발 킥 Input - AnimMontage 재생 */
	UFUNCTION()
	void OnKickRightInput();

	/** 왼발 킥 Input - AnimMontage 재생 */
	UFUNCTION()
	void OnKickLeftInput();

	/** 좌측 이동 Input */
	UFUNCTION()
	void OnMoveLeftInput();

	/** 우측 이동 Input */
	UFUNCTION()
	void OnMoveRightInput();

public:
	// ========== AnimNotify Callback ==========

	/** AnimNotify에서 호출 - 발이 제기에 닿는 순간 */
	UFUNCTION(BlueprintCallable, Category="JegiChagi|AnimNotify")
	void OnAnimNotify_KickContact();

	// ========== Client-Side 게임 로직 ==========

	/** Client에서 타이밍 판정 (로컬) */
	UFUNCTION()
	EKickTiming CalculateLocalTiming();

	/** Client에서 콤보 업데이트 (로컬) */
	UFUNCTION()
	void UpdateLocalCombo(EKickTiming timing);

	/** Client에서 UI/이펙트 재생 (로컬) */
	UFUNCTION()
	void PlayLocalFeedback(EKickTiming timing);

	/** 제기 물리 업데이트 (로컬) */
	UFUNCTION()
	void UpdateJegiPhysics(float deltaTime);

	/** 제기가 땅에 닿았을 때 (로컬) */
	UFUNCTION()
	void OnJegiHitGround();

	/** 경계 이탈 체크 (로컬) */
	UFUNCTION()
	void CheckOutOfBounds();

	// ========== Server RPC (결과 전송) ==========

	/** 킥 결과 전송 (Unreliable = 빠르지만 유실 가능, 통계용) */
	UFUNCTION(Server, Unreliable)
	void Server_ReportKick(EKickTiming timing, float timestamp);

	/** 경계 이탈 보고 */
	UFUNCTION(Server, Reliable)
	void Server_ReportOutOfBounds();

	/** 제기 땅 충돌 보고 */
	UFUNCTION(Server, Reliable)
	void Server_ReportJegiHitGround();

	/** 라운드 종료 시 최종 결과 전송 */
	UFUNCTION(Server, Reliable)
	void Server_SubmitRoundResult(const FJegiRoundRecord& result);

	// ========== Client RPC (검증 결과) ==========

	/** 서버가 결과를 거부했을 때 (치팅 감지) */
	UFUNCTION(Client, Reliable)
	void Client_OnValidationFailed(const FString& reason);

	/** 서버가 점수를 정정했을 때 */
	UFUNCTION(Client, Reliable)
	void Client_OnScoreCorrected(int32 correctedCombo, int32 correctedMaxCombo);

protected:
	// ========== 참조 ==========

	UPROPERTY()
	UJegiChagiContentManager* jegiManager = nullptr;

	UPROPERTY()
	AJegi* myJegi = nullptr;

	// ========== Client-Side 상태 (Replicated 안 함!) ==========

	/** 현재 콤보 (로컬) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="JegiChagi|LocalState")
	int32 localCombo = 0;

	/** 최고 콤보 (로컬) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="JegiChagi|LocalState")
	int32 localMaxCombo = 0;

	/** Perfect/Great/Good/Missed 카운트 (로컬) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="JegiChagi|LocalState")
	int32 localPerfectCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="JegiChagi|LocalState")
	int32 localGreatCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="JegiChagi|LocalState")
	int32 localGoodCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="JegiChagi|LocalState")
	int32 localMissedCount = 0;

	/** 라운드 시작 시간 */
	float roundStartTime = 0.f;

	/** 마지막 킥 시간 (치팅 검증용) */
	float lastKickTime = 0.f;

	/** 좌우 위치 (-1.0 ~ 1.0) */
	float horizontalPosition = 0.f;

	/** 제기 속도 (Client-Side Physics) */
	FVector jegiVelocity = FVector::ZeroVector;

	/** 마지막 제기 위치 */
	FVector lastJegiPosition = FVector::ZeroVector;

	// ========== 설정값 (EditDefaultsOnly) ==========

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Timing")
	float perfectWindow = 0.05f;  // Perfect 판정 범위 (±0.05초)

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Timing")
	float greatWindow = 0.1f;  // Great 판정 범위

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Timing")
	float goodWindow = 0.15f;  // Good 판정 범위

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Physics")
	float baseGravity = 980.f;  // 기본 중력

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Physics")
	float kickForce = 500.f;  // 킥 힘

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Physics")
	float optimalKickHeight = 100.f;  // 최적 킥 높이

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Movement")
	float maxHorizontalRange = 300.f;  // 좌우 이동 범위

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Movement")
	float moveSpeed = 0.1f;  // 좌우 이동 속도

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Difficulty")
	float comboGravityIncrement = 0.2f;  // 콤보당 중력 증가량

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Difficulty")
	int32 comboPerGravityIncrease = 5;  // 몇 콤보마다 중력 증가

	// ========== Animation ==========

	/** 오른발 킥 애니메이션 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Animation")
	TObjectPtr<UAnimMontage> kickRightMontage;

	/** 왼발 킥 애니메이션 몽타주 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Settings|Animation")
	TObjectPtr<UAnimMontage> kickLeftMontage;

	// ========== Input Actions (InputMappingsSettings에서 로드) ==========

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Input|Action")
	TObjectPtr<UInputAction> IA_Kick_L;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Input|Action")
	TObjectPtr<UInputAction> IA_Kick_R;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Input|Action")
	TObjectPtr<UInputAction> IA_MoveLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Input|Action")
	TObjectPtr<UInputAction> IA_MoveRight;
};
