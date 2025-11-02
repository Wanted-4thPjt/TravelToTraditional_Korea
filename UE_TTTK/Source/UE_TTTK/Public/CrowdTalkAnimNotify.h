// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CrowdTalkAnimNotify.generated.h"

/**
 * Talk 애니메이션 시작 시 사운드를 재생하는 AnimNotify
 * 멀티플레이 동기화를 위해 Crowd의 MulitCast_Talk 함수 호출
 */
UCLASS()
class UE_TTTK_API UCrowdTalkAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
