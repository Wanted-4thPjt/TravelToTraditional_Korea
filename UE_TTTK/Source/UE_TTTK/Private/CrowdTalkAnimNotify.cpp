// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdTalkAnimNotify.h"
#include "Crowd.h"
#include "CrowdTargetPoint.h"
#include "Components/AudioComponent.h"

void UCrowdTalkAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp)
	{
		return;
	}

	// SkeletalMesh의 Owner인 Crowd Actor 가져오기
	ACrowd* OwnerCrowd = Cast<ACrowd>(MeshComp->GetOwner());
	if (!OwnerCrowd)
	{
		return;
	}

	// 사운드 배열이 비어있는지 확인
	if (OwnerCrowd->talkSounds.Num() == 0)
	{
		return;
	}

	// 0번 사운드만 재생 (0번 상인만 대화함)
	int32 SoundIndex = 0;

	// MulitCast_Talk 함수 호출 (멀티플레이 사운드 동기화)
	OwnerCrowd->MulitCast_Talk(SoundIndex);

}
