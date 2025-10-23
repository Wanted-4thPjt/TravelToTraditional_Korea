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
	UE_LOG(LogTemp,Warning,TEXT("===Waiting EnterState"));
	OwnerCrowd->SetCrowdCurrentState(TEXT("Crowd.Event.Waiting"));
	UE_LOG(LogTemp,Error,TEXT("===Waiting EnterState"));
	UE_LOG(LogTemp,Error,TEXT("===이전 스테이트  %s"),*(OwnerCrowd->GetCrowdCurrentState().ToString()));
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
	UE_LOG(LogTemp,Warning,TEXT("Waiting Tick 호출"));
	if (OwnerCrowd->CheckGoWorkTime())
	{
		FStateTreeEvent GoWorkEvent;
		GoWorkEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.GoWork");
		OwnerAiController->StateTreeComp->SendStateTreeEvent(GoWorkEvent.Tag );
		UE_LOG(LogTemp,Warning,TEXT("기다림을 끝내겟어"));
		FinishTask(true);
		return EStateTreeRunStatus::Succeeded;
	}
	if (OwnerCrowd->CheckGoHomeTime())
	{
		FStateTreeEvent GoHomeEvent;
		GoHomeEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.GoHome");
		UE_LOG(LogTemp,Warning,TEXT("집갈 시간이다"));
		OwnerAiController->StateTreeComp->SendStateTreeEvent(GoHomeEvent.Tag);
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



