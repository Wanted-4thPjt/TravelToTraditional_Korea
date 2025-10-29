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

	// 사운드 인덱스 선택 로직
	int32 SoundIndex = 0;
	ACrowdTargetPoint* TargetPoint = OwnerCrowd->GetCurrentCrowdTargetPoint();

	if (TargetPoint)
	{
		int32 MyIndex = TargetPoint->FindIndexByCrowd(OwnerCrowd);
		int32 CurrentRound = TargetPoint->DialogueRound;

		// 0번 상인 (질문: Round 0, 답변: Round 4) → 무조건 사운드 [0]
		if (MyIndex == 0 && (CurrentRound == 0 || CurrentRound == 4))
		{
			SoundIndex = 0;
			UE_LOG(LogDialogue, Display, TEXT("[TalkTask] 상인 대사 - 사운드 인덱스: 0 (고정)"));
		}
		// 1, 2, 3번 손님 → 사운드 [1] ~ [끝] 랜덤
		else
		{
			if (OwnerCrowd->talkSounds.Num() > 1)
			{
				SoundIndex = FMath::RandRange(1, OwnerCrowd->talkSounds.Num() - 1);
			}
			else
			{
				SoundIndex = 0;  // 사운드가 1개뿐이면 그거 재생
			}
			UE_LOG(LogDialogue, Display, TEXT("[TalkTask] 손님 대사 (Index %d) - 사운드 인덱스: %d (랜덤)"), MyIndex, SoundIndex);
		}
	}
	else
	{
		// TargetPoint 없으면 랜덤
		SoundIndex = FMath::RandRange(0, OwnerCrowd->talkSounds.Num() - 1);
		UE_LOG(LogDialogue, Warning, TEXT("[TalkTask] TargetPoint 없음 - 랜덤 사운드: %d"), SoundIndex);
	}

	// Talk 함수 호출
	OwnerCrowd->Talk(SoundIndex);

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
