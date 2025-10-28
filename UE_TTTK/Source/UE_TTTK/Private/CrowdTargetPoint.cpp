// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdTargetPoint.h"

#include "Crowd.h"
#include "CrowdAiController.h"
#include "DrawDebugHelpers.h"
#include "GameplayTagsManager.h"
#include "Components/AudioComponent.h"
#include "Components/StateTreeAIComponent.h"


ACrowdTargetPoint::ACrowdTargetPoint()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACrowdTargetPoint::BeginPlay()
{
	Super::BeginPlay();
	if (!bisHome)
	{
		InitializeCrowdPoint_circle();
		
	}
	
}

void ACrowdTargetPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	DrawDebugPoint();
}

void ACrowdTargetPoint::InitializeCrowdPoint_circle()
{
	subTargets.Empty(NumPoints);
    
	for (int32 i = 0; i < NumPoints; i++)
	{
		// 각도 계산 (360도를 NumPoints로 나눔)
		float Angle = (360.0f / NumPoints) * i;
		float AngleRad = FMath::DegreesToRadians(Angle);
        
		// Forward와 Right를 회전시켜서 원 위의 점 계산
		float F = FMath::Cos(AngleRad);
		float R = FMath::Sin(AngleRad);
        
		FVector Point = GetActorLocation() + (GetActorForwardVector() * F * radius) + (GetActorRightVector() * R * radius);
		FCrowdPoint* crowdPoint = new FCrowdPoint(Point, false, nullptr);
		subTargets.Add(crowdPoint);
	}
    
	
}

bool ACrowdTargetPoint::IsSubTargetFull()
{
	for (int i=0;i<subTargets.Num();i++)
	{
		if (!subTargets[i]->inCrowd)
		{
			return false;  // 비어있는 게 있으면 Full이 아님
		}
	}
	return true;  // 모두 차있으면 Full
}

FVector ACrowdTargetPoint::FindEmptySubTarget()
{
	for (int i=0;i<subTargets.Num();i++)
	{
		if (subTargets[i]->inCrowd == false)
		{
			return subTargets[i]->location;
		}
	}
	return FVector::ZeroVector;
}

void ACrowdTargetPoint::ProcessSubTargetIn(class ACrowd* InCrowd,FVector location)
{
	int32 index =  FindIndexByLocation(location);
	if (index!=-1)
	{
		subTargets[index]->inCrowd = true;
		subTargets[index]->currentCrowd = InCrowd;

		// 현재 말하는 Crowd 정보 저장
		CurrentTalker = InCrowd;
		CurrentTalkNumber = index;

		// 파라미터 없는 중간 콜백 바인딩
		if (InCrowd->AudioComp)
		{
			InCrowd->AudioComp->OnAudioFinished.AddDynamic(this, &ACrowdTargetPoint::OnVoiceEnd);
		}
	}
}

void ACrowdTargetPoint::ProcessSubTargetOut(class ACrowd* InCrowd)
{
	int32 index =  FindIndexByCrowd(InCrowd);
	if (index!=-1)
	{
		subTargets[index]->inCrowd = false;
		subTargets[index]->currentCrowd =nullptr;
	}
}

int32 ACrowdTargetPoint::FindIndexByCrowd(class ACrowd* crowd)
{
	for (int i=0;i<subTargets.Num();i++)
	{
		if (subTargets[i]->currentCrowd == crowd)
		{
			return i;
		}
	}
	return -1;
}

int32 ACrowdTargetPoint::FindIndexByLocation(FVector location)
{
	for (int i=0;i<subTargets.Num();i++)
	{
		if (subTargets[i]->location == location)
		{
			return i;
		}
	}
	return -1;
}



void ACrowdTargetPoint::DrawDebugPoint()
{
	if (!GetWorld()) return;

	// 중앙 포인트 (TargetPoint 자체)
	DrawDebugSphere(GetWorld(), GetActorLocation(), 30.0f, 12, FColor::Yellow, false, -1.0f, 0, 2.0f);

	// 각 SubTarget 포인트
	for (int32 i = 0; i < subTargets.Num(); i++)
	{
		if (subTargets[i])
		{
			// 사용 중이면 빨강, 비어있으면 초록
			FColor PointColor = subTargets[i]->inCrowd ? FColor::Red : FColor::Green;

			DrawDebugSphere(GetWorld(), subTargets[i]->location, 20.0f, 12, PointColor, false, -1.0f, 0, 1.5f);

			// 중앙에서 SubTarget으로 선 그리기
			DrawDebugLine(GetWorld(), GetActorLocation(), subTargets[i]->location, FColor::Cyan, false, -1.0f, 0, 1.0f);
		}
	}
}

FVector ACrowdTargetPoint::FindLocationByCrowd(class ACrowd* crowd)
{
	int32 index = FindIndexByCrowd(crowd);
	if (index!=-1)
	{
		return subTargets[index]->location;
	}
	return FVector::ZeroVector;
}

bool ACrowdTargetPoint::IsHome()
{
	return bisHome;
}

void ACrowdTargetPoint::SetHomeOwner(ACrowd* HomeOwner)
{
	Homevar.HomeOwner = HomeOwner; 
}
ACrowd* ACrowdTargetPoint::GetHomeOwner()
{
	return Homevar.HomeOwner;
}

void ACrowdTargetPoint::SetCurrentTalkerCrowd(class ACrowd* CurrentTalkerCrowd)
{
	CurrentTalker = CurrentTalkerCrowd;
}



void ACrowdTargetPoint::OnVoiceEnd()
{
	int32 number = FindIndexByCrowd(CurrentTalker);
	if (CurrentTalker->bIsMarketeer /*시장 상인의 첫번째 대답이라면*/)
	{
		//1,2,3번이 랜덤한 순서로 대화를 실행
		ACrowdAiController* aicontroller = Cast<ACrowdAiController>(subTargets[1]->currentCrowd->GetController());
		if (aicontroller)
		{
			FStateTreeEvent TalkEvent;
			TalkEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.Talk");
			aicontroller->StateTreeComp->SendStateTreeEvent(TalkEvent.Tag);
		}
		
	}
	if (number==1)
	{
		ACrowdAiController* aicontroller = Cast<ACrowdAiController>(subTargets[1]->currentCrowd->GetController());
		if (aicontroller)
		{
			FStateTreeEvent TalkEvent;
			TalkEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.Talk");
			aicontroller->StateTreeComp->SendStateTreeEvent(TalkEvent.Tag);
		}
	}
	if (number==2)
	{
		ACrowdAiController* aicontroller = Cast<ACrowdAiController>(subTargets[1]->currentCrowd->GetController());
		if (aicontroller)
		{
			FStateTreeEvent TalkEvent;
			TalkEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.Talk");
			aicontroller->StateTreeComp->SendStateTreeEvent(TalkEvent.Tag);
		}
	}
}



