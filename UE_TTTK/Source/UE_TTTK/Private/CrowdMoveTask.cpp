// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdMoveTask.h"

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
	UE_LOG(LogTemp, Warning, TEXT("=== CrowdMoveTask EnterState 시작 ==="));

	// StateTreeAIComponent는 AIController에 붙어있으므로, Owner는 AIController입니다!
	AIController = Cast<ACrowdAiController>(Context.GetOwner());
	UE_LOG(LogTemp, Warning, TEXT("AIController (from Context.GetOwner()): %s"), AIController ? TEXT("OK") : TEXT("NULL"));

	if (!AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("AIController가 nullptr! Context.GetOwner() 실패"));
		return EStateTreeRunStatus::Failed;
	}

	// AIController로부터 Pawn(Crowd) 가져오기
	OwnerCrowd = Cast<ACrowd>(AIController->GetPawn());
	UE_LOG(LogTemp, Warning, TEXT("OwnerCrowd (from AIController->GetPawn()): %s"), OwnerCrowd ? TEXT("OK") : TEXT("NULL"));

	if (!OwnerCrowd)
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerCrowd가 nullptr!"));
		return EStateTreeRunStatus::Failed;
	}

	//타겟 정보 가져오기
	TargetPoint = OwnerCrowd->GetTargetByEnum();
	UE_LOG(LogTemp, Warning, TEXT("TargetPoint: %s"), TargetPoint ? TEXT("OK") : TEXT("NULL"));

	if (!TargetPoint)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetPoint가 nullptr! GetTargetByEnum() 실패"));
		return EStateTreeRunStatus::Failed;
	}

	TargetLocation = TargetPoint->FindEmptySubTarget();
	UE_LOG(LogTemp, Warning, TEXT("TargetLocation: %s"), *TargetLocation.ToString());

	if (TargetLocation == FVector::ZeroVector)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetLocation이 ZeroVector! SubTarget 없음"));
		return EStateTreeRunStatus::Failed;
	}

	AIController->WalkToLocation(TargetLocation);
	UE_LOG(LogTemp, Warning, TEXT("WalkToLocation 호출 완료"));

	TargetPoint->ProcessSubTargetIn(OwnerCrowd, TargetLocation);
	PathFollowingComponent = AIController->GetPathFollowingComponent();

	UE_LOG(LogTemp, Warning, TEXT("움직여야지 - EnterState 성공! Running 반환"));
	return EStateTreeRunStatus::Running;
}

void UCrowdMoveTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	Super::ExitState(Context, Transition);
	UE_LOG(LogTemp, Warning, TEXT("=== CrowdMoveTask ExitState 호출됨 ==="));

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
	UE_LOG(LogTemp, Log, TEXT("CrowdMoveTask Tick 호출됨!"));

	if (PathFollowingComponent)
	{
		EPathFollowingStatus::Type Status = PathFollowingComponent->GetStatus();
		UE_LOG(LogTemp, Log, TEXT("PathFollowing Status: %d (Idle=1)"), (int32)Status);

		if (Status == EPathFollowingStatus::Idle && OwnerCrowd->GetVelocity().Length() <= 0.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("=== 목적지 도착! ==="));

			AIController->StopMovement();
			FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(OwnerCrowd->GetActorLocation(), TargetPoint->GetActorLocation());
			TargetRotation = FRotator(0,TargetRotation.Yaw,0);
			OwnerCrowd->SetActorRotation(TargetRotation);

			UE_LOG(LogTemp, Warning, TEXT("FinishTask 호출 - 상태 전환"));
			FinishTask(true);
			
			return EStateTreeRunStatus::Succeeded;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PathFollowingComponent가 nullptr!"));
	}

	return EStateTreeRunStatus::Running;
}

UCrowdMoveTask::UCrowdMoveTask(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}


