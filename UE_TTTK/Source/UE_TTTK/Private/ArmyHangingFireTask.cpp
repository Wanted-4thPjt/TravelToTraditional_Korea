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

	if (OwnerArmy)
	{
		// HangingFire 애니메이션 재생 (PlayHaningMontage에서 플래그 초기화 포함)
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
	// AnimNotifyState가 발생하여 플래그가 true로 설정되었는지 확인
	if (OwnerArmy && OwnerArmy->GetHangingFireAnimCompleted())
	{
		UE_LOG(LogTemp, Warning, TEXT("HangingFireTask: 횃불 장착 완료 감지, Patrol 전환"));

		// Patrol 상태로 전환하기 위한 이벤트 발송
		FGameplayTag patrol = FGameplayTag::RequestGameplayTag("Crowd.Event.Patrol");
		Context.SendEvent(patrol);

		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}
