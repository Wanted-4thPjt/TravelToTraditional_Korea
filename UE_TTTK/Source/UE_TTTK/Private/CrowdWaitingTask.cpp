// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdWaitingTask.h"

#include "Crowd.h"
#include "CrowdAiController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus UCrowdWaitingTask::EnterState(FStateTreeExecutionContext& Context,
                                                  const FStateTreeTransitionResult& Transition)
{
	OwnerAiController = Cast<ACrowdAiController>(Context.GetOwner());
	OwnerCrowd = Cast<ACrowd>(OwnerAiController);
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
	
	switch (WaitingExitReason)
	{
		case EWaitingExitReason::GoWork:
			{
				FGameplayTag GoWorkTag = FGameplayTag::RequestGameplayTag(TEXT("GoHome"));
				Context.SendEvent(GoWorkTag);
				return EStateTreeRunStatus::Succeeded;
			}
		case EWaitingExitReason::GoHome:
			{
				FGameplayTag GoHomeTag = FGameplayTag::RequestGameplayTag(TEXT("GoHome"));
				Context.SendEvent(GoHomeTag);
				return  EStateTreeRunStatus::Succeeded;
			}
			
		case EWaitingExitReason::Talk:
			{
				FGameplayTag TalkTag = FGameplayTag::RequestGameplayTag(TEXT("GoHome"));
				Context.SendEvent(TalkTag);
				return EStateTreeRunStatus::Succeeded;
			}
	}
	return EStateTreeRunStatus::Running;
		
}



