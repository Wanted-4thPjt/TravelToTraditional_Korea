// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmyCrowdHangingFireTask.h"

#include "CrowdAiController.h"
#include "PatrolArmy.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/CharacterMovementComponent.h"

EStateTreeRunStatus UArmyCrowdHangingFireTask::EnterState(FStateTreeExecutionContext& Context,
                                                          const FStateTreeTransitionResult& Transition)
{
	OwnerArmyController = Cast<ACrowdAiController>(Context.GetOwner());
	OwnerArmy = Cast<APatrolArmy>(OwnerArmyController->GetPawn());

	if (OwnerArmy)
	{
		OwnerArmy->GetCharacterMovement()->StopMovementImmediately();
		// HangingFire 애니메이션 재생
		OwnerArmy->PlayHaningMontage();
	}

	return Super::EnterState(Context, Transition);
}

void UArmyCrowdHangingFireTask::ExitState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
}

void UArmyCrowdHangingFireTask::StateCompleted(FStateTreeExecutionContext& Context,
	const EStateTreeRunStatus CompletionStatus, const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

EStateTreeRunStatus UArmyCrowdHangingFireTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	return Super::Tick(Context, DeltaTime);
}
