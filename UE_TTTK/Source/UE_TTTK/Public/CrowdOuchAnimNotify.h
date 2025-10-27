// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "CrowdOuchAnimNotify.generated.h"

/**
 * Ouch 애니메이션 완료 시점을 알리는 AnimNotify
 * 몽타주의 원하는 프레임에 배치하여 상태 전환 타이밍 제어
 */
UCLASS()
class UE_TTTK_API UCrowdOuchAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
