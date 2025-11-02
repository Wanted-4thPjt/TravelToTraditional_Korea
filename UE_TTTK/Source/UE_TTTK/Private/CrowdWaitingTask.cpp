// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdWaitingTask.h"

#include "Crowd.h"
#include "CrowdAiController.h"
#include "CrowdTargetPoint.h"
#include "StateTreeExecutionContext.h"
#include "Components/StateTreeAIComponent.h"

EStateTreeRunStatus UCrowdWaitingTask::EnterState(FStateTreeExecutionContext& Context,
                                                  const FStateTreeTransitionResult& Transition)
{
	OwnerAiController = Cast<ACrowdAiController>(Context.GetOwner());
	OwnerCrowd = Cast<ACrowd>(OwnerAiController->GetPawn());
	OwnerCrowd->SetCrowdCurrentState(TEXT("Crowd.Event.Waiting"));

	// 타이머로 1초마다 시간 체크 (Tick 대신)
	if (OwnerCrowd && OwnerCrowd->GetWorld())
	{
		OwnerCrowd->GetWorld()->GetTimerManager().SetTimer(
			TimeCheckHandle,
			this,
			&UCrowdWaitingTask::CheckTimeOnTimer,
			1.0f,
			true  // 반복
		);
	}

	// Running으로 돌림 (타이머가 계속 체크함)
	return EStateTreeRunStatus::Running;
}

void UCrowdWaitingTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	// 타이머 정리
	if (OwnerCrowd && OwnerCrowd->GetWorld())
	{
		OwnerCrowd->GetWorld()->GetTimerManager().ClearTimer(TimeCheckHandle);
	}
	Super::ExitState(Context, Transition);
}

void UCrowdWaitingTask::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
	const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

EStateTreeRunStatus UCrowdWaitingTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	// Tick 완전 비활성화 (타이머가 모든 체크 수행)
	return EStateTreeRunStatus::Running;
}

void UCrowdWaitingTask::CheckTimeOnTimer()
{
	if (!OwnerCrowd || !OwnerAiController || !OwnerAiController->StateTreeComp)
	{
		return;
	}

	// 출근 시간 체크 (중복 실행 방지)
	if (!OwnerCrowd->bHasGoneWork && OwnerCrowd->CheckGoWorkTime())
	{
		if (OwnerCrowd->CrowdData->GetCrowdType() == ECrowdType::Marketeer)
		{
			OwnerCrowd->bHasGoneWork = true;
			FStateTreeEvent GoWorkEvent;
			GoWorkEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.GoWork");
			OwnerAiController->StateTreeComp->SendStateTreeEvent(GoWorkEvent.Tag);
		}
		return;
	}

	// 퇴근 시간 체크 (중복 실행 방지)
	if (!OwnerCrowd->bHasGoneHome && OwnerCrowd->CheckGoHomeTime())
	{
		OwnerCrowd->bHasGoneHome = true;
		FStateTreeEvent GoHomeEvent;
		GoHomeEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.GoHome");
		OwnerAiController->StateTreeComp->SendStateTreeEvent(GoHomeEvent.Tag);
	}
}
UCrowdWaitingTask::UCrowdWaitingTask(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer)
{
	// 타이머 사용으로 Tick 불필요 (성능 최적화)
	bShouldCallTick = false;
}



