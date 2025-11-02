// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdMoveTask.h"

#include <gsl/pointers>

#include "AIController.h"
#include "Crowd.h"
#include "CrowdAiController.h"
#include "CrowdTargetPoint.h"
#include "StateTreeExecutionContext.h"
#include "Components/StateTreeAIComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"



EStateTreeRunStatus UCrowdMoveTask::EnterState(FStateTreeExecutionContext& Context,
                                               const FStateTreeTransitionResult& Transition)
{
	Super::EnterState(Context, Transition);
	


	AIController = Cast<ACrowdAiController>(Context.GetOwner());

	if (!AIController)
	{
		return EStateTreeRunStatus::Failed;
	}

	// AIController로부터 Pawn(Crowd) 가져오기
	OwnerCrowd = Cast<ACrowd>(AIController->GetPawn());

	if (!OwnerCrowd)
	{
		return EStateTreeRunStatus::Failed;
	}
	//타겟 정보 가져오기
	TargetPoint = OwnerCrowd->GetTargetByEnum();
	OwnerCrowd->SetGoWorkTargetPoint(TargetPoint);
	if (OwnerCrowd->GetCurrentTargetLocation() != FVector::ZeroVector)
	{
		TargetLocation = OwnerCrowd->GetCurrentTargetLocation();
	}
	else
	{
		TargetLocation = TargetPoint->FindEmptySubTarget();
		OwnerCrowd->currentTargetLocation = TargetLocation;
	}
	
	

	if (!TargetPoint)
	{
		return EStateTreeRunStatus::Failed;
	}
	
	
	

	OwnerCrowd -> SetGoWorkTargetPoint(TargetPoint);
	OwnerCrowd -> SetCurrentTargetLocation(TargetLocation);
	
	AIController->WalkToLocation(TargetLocation);

	// 자리 예약 (다른 NPC가 같은 자리로 가지 못하게)
	TargetPoint->ProcessSubTargetIn(OwnerCrowd, TargetLocation);

	PathFollowingComponent = AIController->GetPathFollowingComponent();
	OwnerCrowd->SetCrowdCurrentState("Crowd.Event.GoWork");
	return EStateTreeRunStatus::Running;
}

void UCrowdMoveTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);

	if (OwnerCrowd)
	{
		OwnerCrowd->SetIsMoving(false);
	}

	if (AIController)
	{
		AIController->StopMovement();
	}
}

void UCrowdMoveTask::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
	const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
	
}

EStateTreeRunStatus UCrowdMoveTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);

	if (PathFollowingComponent)
	{
		EPathFollowingStatus::Type Status = PathFollowingComponent->GetStatus();
		float dist = FVector::Dist(OwnerCrowd->GetActorLocation(),OwnerCrowd->GetTargetLocation());

		if (OwnerCrowd->GetVelocity().Length() <= 0.f && dist<0.2f)
		{

			// 도착 처리 (isArrived = true 설정 + 모두 도착 시 StartDialogue 자동 호출)
			TargetPoint->SetSubTargetArrived(OwnerCrowd);

			AIController->StopMovement();
			FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(OwnerCrowd->GetActorLocation(), TargetPoint->GetActorLocation());
			TargetRotation = FRotator(0,TargetRotation.Yaw,0);
			OwnerCrowd->SetActorRotation(TargetRotation);

			FinishTask(true);
			return EStateTreeRunStatus::Succeeded;
		}
	}
	else
	{
	}

	return EStateTreeRunStatus::Running;
}

UCrowdMoveTask::UCrowdMoveTask(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}


