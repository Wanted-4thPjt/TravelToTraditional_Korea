// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdOuchTask.h"

#include "Crowd.h"
#include "CrowdAiController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus UCrowdOuchTask::EnterState(FStateTreeExecutionContext& Context,
                                               const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp, Warning, TEXT("OuchState진입"));
	OwnerAiController = Cast<ACrowdAiController>(Context.GetOwner());
	OwnerCrowd = Cast<ACrowd>(OwnerAiController->GetPawn());
	OwnerAiController->Ouch();
	
	return EStateTreeRunStatus::Running;
}

void UCrowdOuchTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	if (Transition.SourceState.IsValid())
	{
		Context.RequestTransition(Transition.SourceState);
	}
	Super::ExitState(Context, Transition);
}

void UCrowdOuchTask::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
	const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

EStateTreeRunStatus UCrowdOuchTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	return Super::Tick(Context, DeltaTime);
}
