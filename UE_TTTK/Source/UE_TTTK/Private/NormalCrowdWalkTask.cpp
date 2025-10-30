// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalCrowdWalkTask.h"

#include "Crowd.h"
#include "CrowdAiController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus UNormalCrowdWalkTask::EnterState(FStateTreeExecutionContext& Context,
                                                     const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp,Error,TEXT("==================== Normalwalk 진입 시작 ===================="));

	AiController = Cast<ACrowdAiController>(Context.GetOwner());

	if (!AiController)
	{
		UE_LOG(LogTemp,Error,TEXT("!!!! AiController가 nullptr! 실패!"));
		return EStateTreeRunStatus::Failed;
	}

	UE_LOG(LogTemp,Error,TEXT("====aicontroller OK"));

	OwnerCrowd = Cast<ACrowd>(AiController->GetPawn());
	if (!OwnerCrowd)
	{
		UE_LOG(LogTemp,Error,TEXT("!!!! OwnerCrowd가 nullptr! 실패!"));
		return EStateTreeRunStatus::Failed;
	}

	UE_LOG(LogTemp,Error,TEXT("====OwnerCrowd OK"));

	// bHasGoneHome 플래그 리셋 (일하러 나가는 상태이므로)
	OwnerCrowd->bHasGoneHome = false;
	UE_LOG(LogTemp,Error,TEXT("====bHasGoneHome 리셋 완료"));

	TargetLocation = AiController->FindRandomDestination();
	UE_LOG(LogTemp,Error,TEXT("====찾은 랜덤 위치: %s"), *TargetLocation.ToString());

	if (TargetLocation.IsZero())
	{
		UE_LOG(LogTemp,Error,TEXT("!!!! ZeroVector! 실패!"));
		return EStateTreeRunStatus::Failed;
	}

	AiController->WalkToLocation(TargetLocation);
	UE_LOG(LogTemp,Error,TEXT("==================== EnterState 완료: Running 반환 ===================="));
	return EStateTreeRunStatus::Running;
}

void UNormalCrowdWalkTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp,Error,TEXT("==================== Normalwalk 나가기 ===================="));
	Super::ExitState(Context, Transition);
}

void UNormalCrowdWalkTask::StateCompleted(FStateTreeExecutionContext& Context,
	const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

EStateTreeRunStatus UNormalCrowdWalkTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	// nullptr 체크 필수!
	if (!OwnerCrowd || !AiController)
	{
		UE_LOG(LogTemp, Error, TEXT("==== Tick: nullptr 발견!"));
		return EStateTreeRunStatus::Running;
	}

	float dist = FVector::Dist(OwnerCrowd->GetActorLocation(), TargetLocation);
	float velocity = OwnerCrowd->GetVelocity().Length();

	// 10프레임마다 한 번씩만 로그 (너무 많이 안 찍히게)
	static int32 TickCounter = 0;
	if (TickCounter % 10 == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("==== 거리: %.2f, 속도: %.2f"), dist, velocity);
	}
	TickCounter++;

	// 도착 조건 (거리를 더 작게!)
	if (velocity <= 0.1f && dist < 100.0f) // 200 -> 100으로 줄임
	{
		UE_LOG(LogTemp, Error, TEXT("=== 랜덤포인트 도착! 거리: %.2f, 속도: %.2f ==="), dist, velocity);

		TargetLocation = AiController->FindRandomDestination();
		UE_LOG(LogTemp, Error, TEXT("==== 새 목표: %s"), *TargetLocation.ToString());
		AiController->WalkToLocation(TargetLocation);
	}

	return EStateTreeRunStatus::Running;
}

UNormalCrowdWalkTask::UNormalCrowdWalkTask(const FObjectInitializer& ObjectInitializer)  :Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}
