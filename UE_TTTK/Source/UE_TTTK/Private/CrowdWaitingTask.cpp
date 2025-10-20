// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdWaitingTask.h"

#include "Crowd.h"
#include "CrowdAiController.h"
#include "StateTreeExecutionContext.h"
#include "Components/StateTreeAIComponent.h"

EStateTreeRunStatus UCrowdWaitingTask::EnterState(FStateTreeExecutionContext& Context,
                                                  const FStateTreeTransitionResult& Transition)
{
	OwnerAiController = Cast<ACrowdAiController>(Context.GetOwner());
	OwnerCrowd = Cast<ACrowd>(OwnerAiController->GetPawn());
	UE_LOG(LogTemp,Warning,TEXT("기다리자"));
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
	UE_LOG(LogTemp,Warning,TEXT("Tick이다~"));
	if (OwnerCrowd->CheckGoWorkTime())
	{
		FStateTreeEvent GoWorkEvent;
		GoWorkEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.GoWork");
		OwnerAiController->StateTreeComp->SendStateTreeEvent(GoWorkEvent.Tag );
		UE_LOG(LogTemp,Warning,TEXT("기다림을 끝내겟어"));
		FinishTask(true);
		return EStateTreeRunStatus::Succeeded;
	}
	return EStateTreeRunStatus::Running;
		
}
UCrowdWaitingTask::UCrowdWaitingTask(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}



