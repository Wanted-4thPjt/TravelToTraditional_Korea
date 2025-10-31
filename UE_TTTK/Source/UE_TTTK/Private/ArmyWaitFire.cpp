// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmyWaitFire.h"

#include "CrowdAiController.h"
#include "PatrolArmy.h"
#include "StateTreeExecutionContext.h"
#include "Components/StateTreeAIComponent.h"

EStateTreeRunStatus UArmyWaitFire::EnterState(FStateTreeExecutionContext& Context,
                                              const FStateTreeTransitionResult& Transition)
{
	AiController = Cast<ACrowdAiController>(Context.GetOwner());
	if (AiController)
	{
		OwnerArmy = Cast<APatrolArmy>(AiController->GetPawn());
		//OwnerArmy->PlayHangFireAnimation();
	}
	return EStateTreeRunStatus::Running;
}

void UArmyWaitFire::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	
}

void UArmyWaitFire::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
	const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

EStateTreeRunStatus UArmyWaitFire::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
	UE_LOG(LogTemp,Warning,TEXT("불기다리기 틱 시작"));
	if (OwnerArmy)
	{
		if (OwnerArmy->CheckGoHomeTime() && !OwnerArmy -> bIsHangingFire)
		{
			
			FStateTreeEvent HangingFireEvent;
			HangingFireEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.HangingFire");
			UE_LOG(LogTemp,Warning,TEXT("불들 시간이다."));
			AiController->StateTreeComp->SendStateTreeEvent(HangingFireEvent.Tag);
			FinishTask(true);
			return EStateTreeRunStatus::Succeeded;
		}
	}
	return EStateTreeRunStatus::Running;
	
}
UArmyWaitFire::UArmyWaitFire(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}
