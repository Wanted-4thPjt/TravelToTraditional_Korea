// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdOuchAnimNotify.h"
#include "Crowd.h"

void UCrowdOuchAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	if (!MeshComp)
	{
		return;
	}

	// SkeletalMesh의 Owner인 Crowd Actor 가져오기
	ACrowd* OwnerCrowd = Cast<ACrowd>(MeshComp->GetOwner());
	if (OwnerCrowd)
	{
		// Ouch 애니메이션 완료 플래그 설정
		OwnerCrowd->SetOuchAnimCompleted(true);
		UE_LOG(LogTemp, Warning, TEXT("CrowdOuchAnimNotify: Ouch 애니메이션 완료 알림"));
	}
}
