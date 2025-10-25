// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "PatrolArmyHangingFireNotifyState.generated.h"

/**
 * HangingFire 애니메이션의 횃불 장착 구간을 제어하는 AnimNotifyState
 * - NotifyBegin: 횃불 장착 (bIsNight = true)
 * - NotifyEnd: 애니메이션 완료, Patrol 상태로 전환 준비
 */
UCLASS()
class UE_TTTK_API UPatrolArmyHangingFireNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
