// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmyHangingFireTask.h"

#include "CrowdAiController.h"
#include "PatrolArmy.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/CharacterMovementComponent.h"

EStateTreeRunStatus UArmyHangingFireTask::EnterState(FStateTreeExecutionContext& Context,
                                                     const FStateTreeTransitionResult& Transition)
{
	AiController = Cast<ACrowdAiController>(Context.GetOwner());
	OwnerArmy = Cast<APatrolArmy>(AiController->GetPawn());

	if (OwnerArmy)
	{
		OwnerArmy->GetCharacterMovement()->StopMovementImmediately();
		OwnerArmy->PlayHaningMontage();
		UE_LOG(LogTemp, Warning, TEXT("HangingFireTask: 횃불 장착 애니메이션 시작"));
	}

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
	
	if (OwnerArmy && OwnerArmy->GetHangingFireAnimCompleted() && !OwnerArmy->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		UE_LOG(LogTemp, Warning, TEXT("HangingFireTask: 횃불 장착 완료 감지, Patrol 전환"));

		
		FGameplayTag patrol = FGameplayTag::RequestGameplayTag("Crowd.Event.Patrol");
		Context.SendEvent(patrol);

		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}
