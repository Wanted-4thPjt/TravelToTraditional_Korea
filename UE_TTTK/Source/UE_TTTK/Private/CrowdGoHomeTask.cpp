// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdGoHomeTask.h"

#include "Crowd.h"
#include "CrowdAiController.h"
#include "CrowdTargetPoint.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/KismetMathLibrary.h"

EStateTreeRunStatus UCrowdGoHomeTask::EnterState(FStateTreeExecutionContext& Context,
                                                 const FStateTreeTransitionResult& Transition)
{
	
	AIController = Cast<ACrowdAiController>(Context.GetOwner());
	OwnerCrowd = Cast<ACrowd>(AIController->GetPawn());
	TargetPoint = OwnerCrowd -> GetGoHomeTargetPoint();
	//2.타겟지점 가져오기
	TargetPoint = OwnerCrowd -> GetGoHomeTargetPoint();
	AIController->WalkToLocation(TargetPoint->GetActorLocation());
	OwnerCrowd->SetCrowdCurrentState("Crowd.Event.GoHome");
	UE_LOG(LogTemp,Warning,TEXT("Owner Crowd : %s 집가기 시작"),*OwnerCrowd->GetName());
	return EStateTreeRunStatus::Running;
	
}

void UCrowdGoHomeTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
}

void UCrowdGoHomeTask::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
	const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

EStateTreeRunStatus UCrowdGoHomeTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	
	float dist = FVector::Dist(OwnerCrowd->GetActorLocation(),TargetPoint->GetActorLocation());
	if (OwnerCrowd->GetVelocity().Length() <= 0.1f && dist<0.2f)
	{
		UE_LOG(LogTemp, Warning, TEXT("=== 집 도착! ==="));
		
		//AIController->StopMovement();
		//otator TargetRotation = UKismetMathLibrary::FindLookAtRotation(OwnerCrowd->GetActorLocation(), TargetPoint->GetActorLocation());
		//TargetRotation = FRotator(0,TargetRotation.Yaw,0);
		//OwnerCrowd->SetActorRotation(TargetRotation);

		UE_LOG(LogTemp, Warning, TEXT("FinishTask 호출 - 상태 전환"));
		FinishTask(true);
		
		return EStateTreeRunStatus::Succeeded;
	}
	
	

	return EStateTreeRunStatus::Running;
}
UCrowdGoHomeTask::UCrowdGoHomeTask(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}

