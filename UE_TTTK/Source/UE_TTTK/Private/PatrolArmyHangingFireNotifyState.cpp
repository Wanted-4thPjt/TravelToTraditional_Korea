// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolArmyHangingFireNotifyState.h"
#include "PatrolArmy.h"

void UPatrolArmyHangingFireNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
	{
		return;
	}

	
	APatrolArmy* OwnerArmy = Cast<APatrolArmy>(MeshComp->GetOwner());
	
	if (OwnerArmy && OwnerArmy->HasAuthority() && !OwnerArmy->bIsHangingFire)
	{
		// 횃불 장착 (bIsNight = true로 설정)
		OwnerArmy->bIsNight = true;
		OwnerArmy->EqiqueFire();
		
		UE_LOG(LogTemp, Warning, TEXT("HangingFireNotifyState: 횃불 장착 시작 (bIsNight = true)"));
	}
}

void UPatrolArmyHangingFireNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	// SkeletalMesh의 Owner인 PatrolArmy Actor 가져오기
	APatrolArmy* OwnerArmy = Cast<APatrolArmy>(MeshComp->GetOwner());
	if (OwnerArmy)
	{
		// HangingFire 애니메이션 완료 플래그 설정 -> Patrol 상태로 전환
		OwnerArmy->SetHangingFireAnimCompleted(true);
		UE_LOG(LogTemp, Warning, TEXT("HangingFireNotifyState: 횃불 장착 완료, Patrol 전환 준비"));
		
	}
}
