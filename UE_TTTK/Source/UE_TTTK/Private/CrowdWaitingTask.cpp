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
	UE_LOG(LogDialogue,Log,TEXT("[WaitingTask] Wait State 진입"));
	OwnerCrowd->SetCrowdCurrentState(TEXT("Crowd.Event.Waiting"));

	// Running으로 돌림 (시간 체크를 위해 Tick 필요)
	return EStateTreeRunStatus::Running;
}

void UCrowdWaitingTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
}

void UCrowdWaitingTask::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
	const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

EStateTreeRunStatus UCrowdWaitingTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	// 시간 체크는 계속하되, 로그는 줄임
	if (OwnerCrowd->CheckGoWorkTime())
	{
		if (OwnerCrowd->CrowdData->GetCrowdType() == ECrowdType::Marketeer)
		{
			FStateTreeEvent GoWorkEvent;
			GoWorkEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.GoWork");
			OwnerAiController->StateTreeComp->SendStateTreeEvent(GoWorkEvent.Tag );
			UE_LOG(LogTemp,Warning,TEXT("출근 시간! GoWork 이벤트 전송"));
			FinishTask(true);
			return EStateTreeRunStatus::Succeeded;
		}
	}

	if (OwnerCrowd->CheckGoHomeTime())
	{
		FStateTreeEvent GoHomeEvent;
		GoHomeEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.GoHome");
		OwnerAiController->StateTreeComp->SendStateTreeEvent(GoHomeEvent.Tag);
		UE_LOG(LogTemp,Warning,TEXT("퇴근 시간! GoHome 이벤트 전송"));
		FinishTask(true);
		return EStateTreeRunStatus::Succeeded;
	}
	
	

	return EStateTreeRunStatus::Running;
}
UCrowdWaitingTask::UCrowdWaitingTask(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;  // 계속 Tick 호출 (시간 체크 필요)
}



