// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdTargetPoint.h"

#include "Crowd.h"
#include "CrowdAiController.h"
#include "DrawDebugHelpers.h"
#include "GameplayTagsManager.h"
#include "Components/AudioComponent.h"
#include "Components/StateTreeAIComponent.h"

// 대화 시스템 전용 로그 카테고리 정의
DEFINE_LOG_CATEGORY(LogDialogue);


ACrowdTargetPoint::ACrowdTargetPoint()
{
	// 성능 최적화: Tick 비활성화 (디버그 드로우만 사용하고 매 프레임 실행 불필요)
	PrimaryActorTick.bCanEverTick = false;
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

	// 디버그 드로우는 개발 빌드에서만 실행 (성능 최적화)
#if WITH_EDITOR
	if (bEnableDebugDraw)
	{
		DrawDebugPoint();
	}
#endif
}

bool ACrowdTargetPoint::IsAllArrived()
{
	// 캐시된 값 반환 (성능 최적화)
	return bAllArrived;
}

void ACrowdTargetPoint::InitializeCrowdPoint_circle()
{
	subTargets.Empty(NumPoints);
    
	for (int32 i = 0; i < NumPoints; i++)
	{
		
		float Angle = (360.0f / NumPoints) * i;
		float AngleRad = FMath::DegreesToRadians(Angle);
        
	
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
		subTargets[index]->isArrived = false;  // 자리 예약만, 아직 도착 안함

		// Crowd에 현재 TargetPoint 설정
		InCrowd->SetCurrentCrowdTargetPoint(this);

		// 성능 최적화: 인덱스 캐싱 (FindIndexByCrowd 호출 제거)
		InCrowd->MyTargetPointIndex = index;
	}
}

void ACrowdTargetPoint::SetSubTargetArrived(class ACrowd* InCrowd)
{
	// 성능 최적화: 캐시된 인덱스 사용
	int32 index = InCrowd->MyTargetPointIndex;
	if (index != -1 && subTargets.IsValidIndex(index))
	{
		subTargets[index]->isArrived = true;

		// 모든 인원이 도착했는지 체크 (한 번만)
		if (!bAllArrived)
		{
			bool bCheckAllArrived = true;
			for (int i = 0; i < subTargets.Num(); i++)
			{
				if (subTargets[i] && !subTargets[i]->isArrived)
				{
					bCheckAllArrived = false;
					break;
				}
			}

			if (bCheckAllArrived)
			{
				bAllArrived = true;

				// 0번 상인에게 바로 대화 시작 지시 (WaitingTask Tick 제거)
				if (subTargets.IsValidIndex(0) && subTargets[0] && subTargets[0]->currentCrowd)
				{
					StartDialogue();
				}
			}
		}
	}
}

void ACrowdTargetPoint::ProcessSubTargetOut(class ACrowd* InCrowd)
{
	// 성능 최적화: 캐시된 인덱스 사용
	int32 index = InCrowd->MyTargetPointIndex;
	if (index != -1 && subTargets.IsValidIndex(index))
	{
		subTargets[index]->inCrowd = false;
		subTargets[index]->currentCrowd = nullptr;
		subTargets[index]->isArrived = false;

		// 인덱스 초기화
		InCrowd->MyTargetPointIndex = -1;

		// 누군가 떠나면 도착 플래그 리셋
		bAllArrived = false;
		bDialogueStarted = false;
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



void ACrowdTargetPoint::SetListeningStateForOthers(int32 CurrentSpeakerIndex)
{
	// 현재 말하는 사람을 제외한 나머지는 듣는 상태로 설정
	for (int32 i = 0; i < subTargets.Num(); i++)
	{
		if (subTargets[i] && subTargets[i]->currentCrowd)
		{
			if (i == CurrentSpeakerIndex)
			{
				// 현재 화자는 듣지 않음
				subTargets[i]->currentCrowd->SetIsListening(false);
			}
			else
			{
				// 나머지는 듣는 중
				subTargets[i]->currentCrowd->SetIsListening(true);
			}
		}
	}
}

void ACrowdTargetPoint::ResetAllListeningState()
{
	// 모든 Crowd의 듣는 상태 초기화
	for (int32 i = 0; i < subTargets.Num(); i++)
	{
		if (subTargets[i] && subTargets[i]->currentCrowd)
		{
			subTargets[i]->currentCrowd->SetIsListening(false);
		}
	}
}

void ACrowdTargetPoint::RestartDialogueAfterDelay()
{

	// 집에 가는 시간 체크
	bool bShouldGoHome = false;

	for (int32 i = 0; i < subTargets.Num(); i++)
	{
		if (subTargets[i] && subTargets[i]->currentCrowd)
		{
			ACrowd* CurrentCrowd = subTargets[i]->currentCrowd;

			// 집에 간 적이 없으면서 집에 가는 시간이 됐는지 체크
			if (!CurrentCrowd->bHasGoneHome && CurrentCrowd->CheckGoHomeTime())
			{
				bShouldGoHome = true;

				// bHasGoneHome 플래그 설정
				CurrentCrowd->bHasGoneHome = true;

				// GoHome 이벤트 전송
				ACrowdAiController* CrowdController = Cast<ACrowdAiController>(CurrentCrowd->GetController());
				if (CrowdController && CrowdController->StateTreeComp)
				{
					FStateTreeEvent GoHomeEvent;
					GoHomeEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.GoHome");
					CrowdController->StateTreeComp->SendStateTreeEvent(GoHomeEvent.Tag);

				}
			}
		}
	}

	// 집에 가는 시간이 아니면 대화 재시작
	if (!bShouldGoHome)
	{
		bDialogueStarted = false; // 재시작 전 플래그 리셋
		StartDialogue();  // 직접 대화 시작
	}
	else
	{
		bDialogueStarted = false;
	}
}

void ACrowdTargetPoint::StartDialogue()
{
	// 대화 시작 플래그 설정 (중복 시작 방지)
	bDialogueStarted = true;

	// DialogueRound 초기화
	DialogueRound = 0;

	// 0번 상인이 있는지 확인
	if (subTargets.IsValidIndex(0) && subTargets[0] && subTargets[0]->currentCrowd)
	{
		ACrowd* Merchant = subTargets[0]->currentCrowd;

		// 0번 화자가 말할 때, 나머지는 듣는 상태로 설정
		SetListeningStateForOthers(0);

		// 델리게이트 바인딩
		if (Merchant->AudioComp)
		{
			Merchant->AudioComp->OnAudioFinished.RemoveDynamic(this, &ACrowdTargetPoint::OnVoiceEnd);
			Merchant->AudioComp->OnAudioFinished.AddDynamic(this, &ACrowdTargetPoint::OnVoiceEnd);

			CurrentTalker = Merchant;
			CurrentTalkNumber = 0;
		}

		// Talk 이벤트 전송
		ACrowdAiController* MerchantController = Cast<ACrowdAiController>(Merchant->GetController());
		if (MerchantController && MerchantController->StateTreeComp)
		{
			FStateTreeEvent TalkEvent;
			TalkEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.Talk");
			MerchantController->StateTreeComp->SendStateTreeEvent(TalkEvent.Tag);
		}
	}
}

void ACrowdTargetPoint::OnVoiceEnd()
{
	// 이전 화자 저장 (언바인딩을 위해)
	ACrowd* PreviousTalker = CurrentTalker;

	// 성능 최적화: 캐시된 인덱스 사용
	int32 PreviousNumber = PreviousTalker ? PreviousTalker->MyTargetPointIndex : -1;


	// 0번 상인만 대화하므로, 0번이 끝나면 3초 대기 후 다시 시작
	if (PreviousNumber == 0)
	{
		// 한 사이클 종료 - 3초 대기 후 다시 시작
		DialogueRound = -1;  // 대기 중 상태

		// 모든 Crowd의 듣는 상태 초기화 (대기 중)
		ResetAllListeningState();

		// 3초 타이머 시작
		GetWorld()->GetTimerManager().SetTimer(
			DialogueRestartTimerHandle,
			this,
			&ACrowdTargetPoint::RestartDialogueAfterDelay,
			3.0f,
			false
		);

		// 이전 화자의 델리게이트 언바인딩
		if (PreviousTalker && PreviousTalker->AudioComp)
		{
			PreviousTalker->AudioComp->OnAudioFinished.RemoveDynamic(this, &ACrowdTargetPoint::OnVoiceEnd);
		}

		return;
	}
	else
	{
		return;
	}
}



