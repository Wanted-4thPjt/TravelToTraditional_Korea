// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdOuchTask.h"

#include "Crowd.h"
#include "CrowdAiController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus UCrowdOuchTask::EnterState(FStateTreeExecutionContext& Context,
                                               const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp, Error, TEXT("=== Crowd OuchState진입"));
	
	OwnerAiController = Cast<ACrowdAiController>(Context.GetOwner());
	OwnerCrowd = Cast<ACrowd>(OwnerAiController->GetPawn());
	OwnerAiController->Ouch();
	UE_LOG(LogTemp,Error,TEXT("====%s에서 Ouch Enter진입"),*(OwnerCrowd->GetCrowdCurrentState().ToString()))
	return EStateTreeRunStatus::Running;
}

void UCrowdOuchTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	
	Super::ExitState(Context, Transition);
	UE_LOG(LogTemp,Error,TEXT("===EXIT Ouch스테이트"));
	if (Transition.SourceState.IsValid()) // 이전 스텡이트 정보 가져오기
	{
		
		FGameplayTag PrevieousTag = FGameplayTag::RequestGameplayTag(OwnerCrowd->GetCrowdCurrentState());
		Context.SendEvent(PrevieousTag);
		FinishTask(true);
		
	}
}

void UCrowdOuchTask::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
	const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

EStateTreeRunStatus UCrowdOuchTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	// AnimNotify가 발생하여 플래그가 true로 설정되었는지 확인
	if (OwnerCrowd && OwnerCrowd->GetOuchAnimCompleted())
	{
		UE_LOG(LogTemp, Warning, TEXT("CrowdOuchTask: Ouch 애니메이션 완료 감지, 상태 전환"));
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}
UCrowdOuchTask::UCrowdOuchTask(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}
