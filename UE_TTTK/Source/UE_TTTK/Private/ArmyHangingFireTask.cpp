// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmyHangingFireTask.h"

#include "CrowdAiController.h"
#include "PatrolArmy.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus UArmyHangingFireTask::EnterState(FStateTreeExecutionContext& Context,
                                                     const FStateTreeTransitionResult& Transition)
{

	AiController = Cast<ACrowdAiController>(Context.GetOwner());
	OwnerArmy = Cast<APatrolArmy>(AiController->GetPawn());
	//Todo : 애니메이션 재생
	UE_LOG(LogTemp,Warning,TEXT("불들엇따```` EnterState"));
	return EStateTreeRunStatus::Running;
}

void UArmyHangingFireTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{

	UE_LOG(LogTemp,Warning,TEXT("불들엇따```` ExitState"));
	Super::ExitState(Context, Transition);
}

void UArmyHangingFireTask::StateCompleted(FStateTreeExecutionContext& Context,
	const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

EStateTreeRunStatus UArmyHangingFireTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	//return Super::Tick(Context, DeltaTime);
	currentTime+=DeltaTime;
	if (currentTime>=2.0f)
	{
		currentTime=0.0f;
		FGameplayTag patrol = FGameplayTag::RequestGameplayTag("Crowd.Event.Patrol");
		Context.SendEvent(patrol);
		
		return EStateTreeRunStatus::Succeeded;
		
	}
	return EStateTreeRunStatus::Running;
}
