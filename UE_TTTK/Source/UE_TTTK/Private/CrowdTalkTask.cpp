// Fill out your copyright notice in the Description page of Project Settings.

#include "CrowdTalkTask.h"
#include "Crowd.h"
#include "CrowdAiController.h"
#include "CrowdTargetPoint.h"
#include "StateTreeExecutionContext.h"
#include "Components/AudioComponent.h"

EStateTreeRunStatus UCrowdTalkTask::EnterState(FStateTreeExecutionContext& Context,
                                                const FStateTreeTransitionResult& Transition)
{
	AIController = Cast<ACrowdAiController>(Context.GetOwner());
	if (!AIController)
	{
		UE_LOG(LogDialogue, Error, TEXT("[TalkTask] AIController가 nullptr!"));
		return EStateTreeRunStatus::Failed;
	}

	OwnerCrowd = Cast<ACrowd>(AIController->GetPawn());
	if (!OwnerCrowd)
	{
		UE_LOG(LogDialogue, Error, TEXT("[TalkTask] OwnerCrowd가 nullptr!"));
		return EStateTreeRunStatus::Failed;
	}

	// 말하는 상태로 설정 (애니메이션 블루프린트에서 사용)
	OwnerCrowd->SetIsTalking(true);

	// Running 상태로 반환 - 애니메이션 노티파이에서 사운드 재생 후 오디오가 끝나면 Tick에서 완료 처리
	return EStateTreeRunStatus::Running;
}

void UCrowdTalkTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	// bIsTalking 상태 초기화
	if (OwnerCrowd)
	{
		OwnerCrowd->SetIsTalking(false);
	}
	Super::ExitState(Context, Transition);
}

EStateTreeRunStatus UCrowdTalkTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	// Tick 완전 비활성화 (델리게이트로 처리)
	// AudioComp의 OnAudioFinished 델리게이트가 자동으로 Succeeded 처리
	return EStateTreeRunStatus::Running;
}

UCrowdTalkTask::UCrowdTalkTask(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// 델리게이트 사용으로 Tick 불필요 (성능 최적화)
	bShouldCallTick = false;
}
