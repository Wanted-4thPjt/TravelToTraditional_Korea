// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdOuchTask.h"

#include "Crowd.h"
#include "CrowdAiController.h"
#include "StateTreeExecutionContext.h"

EStateTreeRunStatus UCrowdOuchTask::EnterState(FStateTreeExecutionContext& Context,
                                               const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp, Error, TEXT("=== Crowd OuchState진입"));

	OwnerAiController = Cast<ACrowdAiController>(Context.GetOwner());
	OwnerCrowd = Cast<ACrowd>(OwnerAiController->GetPawn());

	if (OwnerCrowd)
	{
		// 횃불에 맞았을 때만 레그돌 활성화
		if (OwnerCrowd->WasHitBySwingingTorch())
		{
			UE_LOG(LogTemp, Warning, TEXT("[CrowdOuchTask] 횃불에 맞아 레그돌 활성화"));
			OwnerCrowd->EnableRagdoll();
		}
		else
		{
			// 횃불에 맞지 않았으면 일반 Ouch 애니메이션만 재생
			UE_LOG(LogTemp, Warning, TEXT("[CrowdOuchTask] 일반 부딪힘 - 애니메이션만 재생"));
			OwnerAiController->Ouch();
		}
	}

	UE_LOG(LogTemp,Error,TEXT("====%s에서 Ouch Enter진입"),*(OwnerCrowd->GetCrowdCurrentState().ToString()))
	return EStateTreeRunStatus::Running;
}

void UCrowdOuchTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogTemp,Error,TEXT("===EXIT Ouch스테이트"));

	// 레그돌 상태인 경우 ExitState를 무시 (레그돌 상태 영구 유지)
	if (OwnerCrowd && OwnerCrowd->IsRagdolled())
	{
		UE_LOG(LogTemp, Warning, TEXT("[CrowdOuchTask] 레그돌 상태 - Exit 무시"));
		return;
	}

	// 레그돌이 아닌 경우에만 이전 상태로 복귀
	if (Transition.SourceState.IsValid() && OwnerCrowd && !OwnerCrowd->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		FGameplayTag PrevieousTag = FGameplayTag::RequestGameplayTag(OwnerCrowd->GetCrowdCurrentState());
		Context.SendEvent(PrevieousTag);
		UE_LOG(LogTemp,Warning,TEXT("이전 스테이트 %s 행하는 사람 : %s"),*OwnerCrowd->GetCrowdCurrentState().ToString(),*OwnerCrowd->GetName());
		FinishTask(true);
	}
}

void UCrowdOuchTask::StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
	const FStateTreeActiveStates& CompletedActiveStates)
{
	Super::StateCompleted(Context, CompletionStatus, CompletedActiveStates);
}

EStateTreeRunStatus UCrowdOuchTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	// 레그돌 상태인 경우 계속 Running 유지 (다른 상태로 전환 안됨)
	if (OwnerCrowd && OwnerCrowd->IsRagdolled())
	{
		return EStateTreeRunStatus::Running;
	}

	// 레그돌이 아닌 경우 애니메이션 완료 확인
	if (OwnerCrowd && !OwnerCrowd->GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		UE_LOG(LogTemp, Warning, TEXT("CrowdOuchTask: Ouch 애니메이션 완료 감지, 상태 전환"));
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}
UCrowdOuchTask::UCrowdOuchTask(const FObjectInitializer& ObjectInitializer)  : Super(ObjectInitializer)
{
	bShouldCallTick = true;
	bShouldCallTickOnlyOnEvents = false;
}
