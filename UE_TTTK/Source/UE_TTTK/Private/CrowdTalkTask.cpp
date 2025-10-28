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
	UE_LOG(LogTemp, Warning, TEXT("[TalkTask] EnterState 시작"));

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

	// 사운드 배열이 비어있는지 확인
	if (OwnerCrowd->talkSounds.Num() == 0)
	{
		UE_LOG(LogDialogue, Warning, TEXT("[TalkTask] talkSounds 배열이 비어있음! 사운드 없이 대화 진행"));
		// 사운드 없어도 계속 진행 (테스트용)
		return EStateTreeRunStatus::Succeeded;
	}

	// 랜덤 사운드 선택 (0 ~ talkSounds.Num()-1)
	int32 RandomIndex = FMath::RandRange(0, OwnerCrowd->talkSounds.Num() - 1);

	UE_LOG(LogDialogue, Display, TEXT("[TalkTask] 대화 시작 - 사운드 인덱스: %d"), RandomIndex);

	// Talk 함수 호출
	OwnerCrowd->Talk(RandomIndex);

	// Running 상태로 반환 - 오디오가 끝나면 델리게이트에서 다음으로 진행
	return EStateTreeRunStatus::Running;
}

void UCrowdTalkTask::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition)
{
	UE_LOG(LogDialogue, Log, TEXT("[TalkTask] ExitState 호출됨"));
	Super::ExitState(Context, Transition);
}

EStateTreeRunStatus UCrowdTalkTask::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	UE_LOG(LogTemp, Warning, TEXT("[TalkTask] Tick 시작"));
	// AudioComp가 재생 중인지 확인
	if (OwnerCrowd && OwnerCrowd->AudioComp)
	{
		if (OwnerCrowd->AudioComp->IsPlaying())
		{
			return EStateTreeRunStatus::Running;
		}
		else
		{
			// 재생 완료
			UE_LOG(LogDialogue, Display, TEXT("[TalkTask] 오디오 재생 완료 - Succeeded"));
			return EStateTreeRunStatus::Succeeded;
		}
	}

	// AudioComp가 없으면 바로 완료
	return EStateTreeRunStatus::Succeeded;
}
