// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdMoveTask.h"

#include "AIController.h"
#include "Crowd.h"
#include "CrowdTargetPoint.h"
#include "StateTreeExecutionContext.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Navigation/PathFollowingComponent.h"

EStateTreeRunStatus UCrowdMoveTask::EnterState(FStateTreeExecutionContext& Context,
                                               const FStateTreeTransitionResult& Transition) // Running을 반환하면 tick이 진행됨.
{
	UE_LOG(LogTemp, Warning, TEXT("=== EnterState 시작 ==="));

	//TODO: 1. 군중 애니메이션 제어
	OwenrCrowd = Cast<ACrowd>(Context.GetOwner());

	if (OwenrCrowd == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerCrowd가 nullptr!"));
		return EStateTreeRunStatus::Failed;
	}

	UE_LOG(LogTemp, Warning, TEXT("Crowd OK"));

	//TODO: 2. 목표 타겟 설정(각 타입에 따라)
	ACrowdTargetPoint* target = OwenrCrowd->GetTargetByEnum();

	if (target == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Target이 nullptr! CrowdData나 TargetPoint 확인!"));
		return EStateTreeRunStatus::Failed;
	}
	

	// SubTarget이 가득 찼는지 확인
	if (target->IsSubTargetFull())
	{
		UE_LOG(LogTemp, Error, TEXT("SubTarget이 모두 찼습니다! 대기 상태로 전환 필요"));
		return EStateTreeRunStatus::Failed;
	}
	Destination = target->GetActorLocation();
	// 비어있는 SubTarget 찾기
	FVector targetLocation = target->FindEmptySubTarget();

	if (targetLocation == FVector::ZeroVector)
	{
		UE_LOG(LogTemp, Error, TEXT("FindEmptySubTarget이 ZeroVector 반환!"));
		return EStateTreeRunStatus::Failed;
	}

	UE_LOG(LogTemp, Warning, TEXT("Target OK: %s, SubTarget: %s"), *target->GetActorLocation().ToString(), *targetLocation.ToString());

	AIController = Cast<AAIController>(OwenrCrowd->GetController());

	if (AIController == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("AIController가 nullptr!"));
		return EStateTreeRunStatus::Failed;
	}

	UE_LOG(LogTemp, Warning, TEXT("AI Controller OK, 이동 시작!"));

	OwenrCrowd->SetIsMoving(true);
	target->ProcessSubTargetIn(OwenrCrowd,targetLocation);
	AIController->MoveToLocation(targetLocation);
	PathFollowingComponent = AIController->GetPathFollowingComponent();

	return EStateTreeRunStatus::Running;
}

void UCrowdMoveTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)//다른 스테이트로 전환 될때 ,모드가 fail or succed 호출될때
{
	Super::ExitState(Context, Transition);
	OwenrCrowd->SetIsMoving(false);
	FRotator lookat = UKismetMathLibrary::FindLookAtRotation(OwenrCrowd->GetActorLocation(),Destination);
	OwenrCrowd->SetActorRotation(lookat);
	UE_LOG(LogTemp,Warning,TEXT("===============================도착 후 ExitState==========================="));
	return;
}

void UCrowdMoveTask::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
	const FStateTreeActiveStates& CompletedActiveStates) // 모든 자식 스테이트들이 끝나면 
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

EStateTreeRunStatus UCrowdMoveTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	if (PathFollowingComponent!= nullptr)
	{
		EPathFollowingStatus::Type currentStatus = PathFollowingComponent->GetStatus();
		if (currentStatus == EPathFollowingStatus::Idle && OwenrCrowd->GetVelocity().Length()<=0.f)
		{
			//TODO : CROWD 도착
			return EStateTreeRunStatus::Succeeded;
		}
	}
	return EStateTreeRunStatus::Running;
}
