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
	UE_LOG(LogTemp, Warning, TEXT("===== ArmyPatrolTask::EnterState 시작 ====="));

	AiController = Cast<ACrowdAiController>(Context.GetOwner());
	if (!AiController)
	{
		UE_LOG(LogTemp, Error, TEXT("ArmyPatrolTask::EnterState - AiController 캐스팅 실패!"));
		return EStateTreeRunStatus::Failed;
	}

	OwnerArmy = Cast<APatrolArmy>(AiController->GetPawn());
	if (!OwnerArmy)
	{
		UE_LOG(LogTemp, Error, TEXT("ArmyPatrolTask::EnterState - OwnerArmy 캐스팅 실패!"));
		return EStateTreeRunStatus::Failed;
	}

	UE_LOG(LogTemp, Warning, TEXT("ArmyPatrolTask::EnterState - Army: %s, Path: %s"),
		*OwnerArmy->GetName(),
		OwnerArmy->armyPath ? *OwnerArmy->armyPath->GetName() : TEXT("NULL"));

	AiController->Patrol();
	UE_LOG(LogTemp, Warning, TEXT("===== ArmyPatrolTask::EnterState 완료 - Running 상태 ====="));

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
	UE_LOG(LogTemp, Warning, TEXT("PatrolTick"));
	if (!OwnerArmy)
	{
		UE_LOG(LogTemp, Error, TEXT("ArmyPatrolTask::Tick - OwnerArmy is null!"));
		return EStateTreeRunStatus::Failed;
	}

	FPatrolPoint* currentPoint = OwnerArmy->GetcurrentPatrolPoint();
	if (!currentPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("ArmyPatrolTask::Tick - currentPoint is null!"));
		return EStateTreeRunStatus::Failed;
	}

	FVector armyLocation = OwnerArmy->GetActorLocation();
	FVector targetLocation = currentPoint->PatrolLocation;
	float dist = FVector::Dist(armyLocation, targetLocation);
	float velocity = OwnerArmy->GetVelocity().Length();

	

	// 도착 체크 (거리 임계값 증가)
	if (dist <=100.0f)  // 0.2f에서 100.0f로 임시 변경
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] ===== 목적지 도착! ====="), *OwnerArmy->GetName());
		UE_LOG(LogTemp, Warning, TEXT("거리: %.2f, 속도: %.2f"), dist, velocity);

		//AiController->StopMovement();
		UE_LOG(LogTemp, Warning, TEXT("[%s] UpdateIndex 호출 - idx: %d"), *OwnerArmy->GetName(), OwnerArmy->idx);
		
		if (OwnerArmy->bIsForward)
		{
			int32 nexidx = OwnerArmy->idx+1;
			OwnerArmy->SetIndex(nexidx);
		}
		else
		{
			OwnerArmy->SetIndex(OwnerArmy->idx--);
		}

		//UE_LOG(LogTemp, Warning, TEXT("[%s] UpdateIndex 호출 후 - idx:  %s"), *OwnerArmy->GetName(), *FString(OwnerArmy->bIsForward ? "Forward" : "Reverse"));

		// StateTree가 다시 순환하도록 Succeeded 반환
		UE_LOG(LogTemp, Warning, TEXT("[%s] Task 완료 - Succeeded 반환"), *OwnerArmy->GetName());
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

UArmyPatrolTask::UArmyPatrolTask(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}