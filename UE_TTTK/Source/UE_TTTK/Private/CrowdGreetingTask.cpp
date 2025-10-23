// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdGreetingTask.h"

#include "Crowd.h"
#include "CrowdAiController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus UCrowdGreetingTask::EnterState(FStateTreeExecutionContext& Context,
                                                   const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp, Error, TEXT("=== CrowdGreetingTask EnterState 시작 ==="));

	//return Super::EnterState(Context, Transition);
	AIController = Cast<ACrowdAiController>(Context.GetOwner());
	UE_LOG(LogTemp, Warning, TEXT("AIController (from Context.GetOwner()): %s"), AIController ? TEXT("OK") : TEXT("NULL"));

	if (!AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("AIController가 nullptr!"));
		return EStateTreeRunStatus::Failed;
	}

	OwnerCrowd = Cast<ACrowd>(AIController->GetPawn());
	UE_LOG(LogTemp, Warning, TEXT("OwnerCrowd (from AIController->GetPawn()): %s"), OwnerCrowd ? TEXT("OK") : TEXT("NULL"));

	if (OwnerCrowd && AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Greeting 호출!"));
		AIController->Greeting();
		UE_LOG(LogTemp, Warning, TEXT("GreetingTask Succeeded 반환"));
		return EStateTreeRunStatus::Succeeded;
	}

	UE_LOG(LogTemp, Error, TEXT("GreetingTask Failed - OwnerCrowd 또는 AIController 없음"));
	return EStateTreeRunStatus::Failed;
}

void UCrowdGreetingTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp, Warning, TEXT("=== CrowdGreetingTask ExitState 호출됨 ==="));
	Super::ExitState(Context, Transition);
}

void UCrowdGreetingTask::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
	const FStateTreeActiveStates& CompletedActiveStates)
{
	UE_LOG(LogTemp, Warning, TEXT("=== CrowdGreetingTask StateCompleted 호출됨 ==="));
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

EStateTreeRunStatus UCrowdGreetingTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	UE_LOG(LogTemp, Log, TEXT("CrowdGreetingTask Tick 호출됨"));
	return Super::Tick(Context, DeltaTime);
}
