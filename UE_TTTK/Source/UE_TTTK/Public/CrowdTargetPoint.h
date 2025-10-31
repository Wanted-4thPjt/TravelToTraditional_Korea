// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "CrowdTargetPoint.generated.h"

// 대화 시스템 전용 로그 카테고리
DECLARE_LOG_CATEGORY_EXTERN(LogDialogue, Log, All);


class ACrowd;
enum class ECrowdType : uint8;

USTRUCT()
struct FCrowdPoint
{
	GENERATED_BODY()

	FVector location;
	bool inCrowd = false;
	class ACrowd* currentCrowd = nullptr;
	bool isArrived =false;
	FCrowdPoint(){}

	// 매개변수 생성자
	FCrowdPoint(FVector InLocation, bool bInCrowd, ACrowd* InCrowd)
		: location(InLocation), inCrowd(bInCrowd), currentCrowd(InCrowd), isArrived(false) {}
};
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnVoiceEnd, ACrowd*, int32);
USTRUCT()
struct FCrowdHome
{
	GENERATED_BODY()
	ACrowd* HomeOwner = nullptr;
	
};
UCLASS()
class UE_TTTK_API ACrowdTargetPoint : public ATargetPoint
{
	GENERATED_BODY()
public:
	ACrowdTargetPoint();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString targetName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECrowdType Crowdtype;
	TArray<struct FCrowdPoint*> subTargets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float radius;
	UPROPERTY(EditAnywhere)
	bool bisHome = false;
	UPROPERTY()
	FCrowdHome Homevar;
	
	int32 ArrivedCount =0;
	bool IsAllArrived();
		
	UFUNCTION()
	void InitializeCrowdPoint_circle();
	UFUNCTION()
	bool IsSubTargetFull();
	UFUNCTION()
	FVector FindEmptySubTarget();
	UFUNCTION()
	void ProcessSubTargetIn(class ACrowd* InCrowd,FVector location);
	UFUNCTION()
	void SetSubTargetArrived(class ACrowd* InCrowd);
	UFUNCTION()
	void ProcessSubTargetOut(class ACrowd* OutCrowd);
	UFUNCTION()
	int32 FindIndexByCrowd(class ACrowd* crowd);
	UFUNCTION()
	int32 FindIndexByLocation(FVector location);
	UFUNCTION()
	void DrawDebugPoint();
	UFUNCTION()
	FVector FindLocationByCrowd(class ACrowd* crowd);
	UFUNCTION()
	bool IsHome();
	UFUNCTION()
	void SetHomeOwner(ACrowd* HomeOwner);
	ACrowd* GetHomeOwner();
	void SetCurrentTalkerCrowd(class ACrowd* CurrentTalkerCrowd);

	// 오디오 종료 콜백을 위한 멤버 변수
	UPROPERTY()
	ACrowd* CurrentTalker;

	int32 CurrentTalkNumber;

	// 대화 라운드 추적 (0:상인질문, 1-3:손님반응, 4:상인답변, -1:대기중)
	int32 DialogueRound = 0;

	// 타이머 핸들
	FTimerHandle DialogueRestartTimerHandle;

	// 대화 재시작 함수 (3초 후 호출)
	UFUNCTION()
	void RestartDialogueAfterDelay();

	// 실제 음성 종료 처리 함수
	UFUNCTION()
	void OnVoiceEnd();

	// 대화 시작 함수 (0번 상인부터)
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue();

	// 듣는 상태 관리 함수
	UFUNCTION()
	void SetListeningStateForOthers(int32 CurrentSpeakerIndex);

	// 모든 Crowd의 듣는 상태 초기화
	UFUNCTION()
	void ResetAllListeningState();


};
