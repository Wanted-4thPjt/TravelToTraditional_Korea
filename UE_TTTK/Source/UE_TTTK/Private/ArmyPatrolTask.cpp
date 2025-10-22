// Fill out your copyright notice in the Description page of Project Settings.


#include "ArmyPatrolTask.h"

#include "CrowdAiController.h"
#include "PatrolArmy.h"
#include "PatrolPath.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/KismetMathLibrary.h"



EStateTreeRunStatus UArmyPatrolTask::EnterState(FStateTreeExecutionContext& Context,
                                                const FStateTreeTransitionResult& Transition)
{

	 AiController = Cast<ACrowdAiController>(Context.GetOwner());
	 OwnerArmy = Cast<APatrolArmy>(AiController->GetPawn());
	if (OwnerArmy)
	{
		AiController->Patrol();
		UE_LOG(LogTemp,Warning,TEXT("=====ArmyPatrolTask::EnterState"));
	}
	
	return EStateTreeRunStatus::Running;
}

void UArmyPatrolTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp,Warning,TEXT("=====ArmyPatrolTask::EnterState"));
	Super::ExitState(Context, Transition);
}

void UArmyPatrolTask::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
	const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

EStateTreeRunStatus UArmyPatrolTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
	UE_LOG(LogTemp,Warning,TEXT("=====ArmyPatrolTask::Tick"));


	FVector armyLocation = OwnerArmy->GetActorLocation();
	FVector targetLocation = OwnerArmy->GetcurrentPatrolPoint()->PatrolLocation;
		
	float dist = FVector::Dist(armyLocation, targetLocation);
	if (OwnerArmy->GetVelocity().Length() <= 0.f && dist<0.2f)
	{
		UE_LOG(LogTemp, Warning, TEXT("=== 목적지 도착! ==="));
		
		AiController->StopMovement();
		UE_LOG(LogTemp, Warning, TEXT("FinishTask 호출 - 군인 상태 전환"));
		OwnerArmy->UpdateIndex();
		FinishTask(true);
		
		return EStateTreeRunStatus::Succeeded;
	}
	
	

	return EStateTreeRunStatus::Running;
}

UArmyPatrolTask::UArmyPatrolTask(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}