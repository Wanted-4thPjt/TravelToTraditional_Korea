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

bool ACrowdTargetPoint::IsAllArrived()
{
	if (ArrivedCount == NumPoints )
	{
		return true;
	}
	return false;
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

		// Crowd에 현재 TargetPoint 설정
		InCrowd->SetCurrentCrowdTargetPoint(this);

		UE_LOG(LogDialogue, Log, TEXT("[입장] Crowd 입장 - Index %d"), index);

		// 모든 자리가 다 찼는지 확인
		
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



void ACrowdTargetPoint::StartDialogue()
{
	UE_LOG(LogDialogue, Display, TEXT("[대화 시작] =========="));

	// DialogueRound 초기화
	DialogueRound = 0;

	// 0번 상인이 있는지 확인
	if (subTargets.IsValidIndex(0) && subTargets[0] && subTargets[0]->currentCrowd)
	{
		ACrowd* Merchant = subTargets[0]->currentCrowd;

		// 델리게이트 바인딩
		if (Merchant->AudioComp)
		{
			Merchant->AudioComp->OnAudioFinished.RemoveDynamic(this, &ACrowdTargetPoint::OnVoiceEnd);
			Merchant->AudioComp->OnAudioFinished.AddDynamic(this, &ACrowdTargetPoint::OnVoiceEnd);

			CurrentTalker = Merchant;
			CurrentTalkNumber = 0;

			UE_LOG(LogDialogue, Log, TEXT("[델리게이트] 상인(Index 0) 바인딩 완료"));
		}

		// Talk 이벤트 전송
		ACrowdAiController* MerchantController = Cast<ACrowdAiController>(Merchant->GetController());
		if (MerchantController && MerchantController->StateTreeComp)
		{
			// StateTree 상태 확인
			UE_LOG(LogDialogue, Warning, TEXT("[디버그] StateTree 실행 중: %s"),
				MerchantController->StateTreeComp->IsRunning() ? TEXT("예") : TEXT("아니오"));

			FStateTreeEvent TalkEvent;
			TalkEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.Talk");

			UE_LOG(LogDialogue, Warning, TEXT("[디버그] 전송할 이벤트 태그: %s"),
				*TalkEvent.Tag.ToString());

			MerchantController->StateTreeComp->SendStateTreeEvent(TalkEvent.Tag);

			UE_LOG(LogDialogue, Display, TEXT("[이벤트 전송] 상인에게 Talk 이벤트 전송 성공"));
		}
		else
		{
			UE_LOG(LogDialogue, Error, TEXT("[오류] 상인 Controller 또는 StateTreeComp가 nullptr!"));
			UE_LOG(LogDialogue, Error, TEXT("[오류] MerchantController: %s, StateTreeComp: %s"),
				MerchantController ? TEXT("OK") : TEXT("NULL"),
				(MerchantController && MerchantController->StateTreeComp) ? TEXT("OK") : TEXT("NULL"));
		}
	}
	else
	{
		UE_LOG(LogDialogue, Error, TEXT("[오류] subTargets[0]에 상인이 없음! 대화 시작 불가"));
	}
}

void ACrowdTargetPoint::OnVoiceEnd()
{
	int32 number = FindIndexByCrowd(CurrentTalker);

	UE_LOG(LogDialogue, Log, TEXT("[음성 종료] Speaker Index: %d, Round: %d"), number, DialogueRound);

	// 다음 말할 사람의 인덱스
	int32 NextSpeakerIndex = -1;

	// 대화 순서: 0(상인질문) → 1 → 2 → 3 → 0(상인답변) → 다시 1부터 반복
	if (DialogueRound == 0 && number == 0)  // 0번 상인 질문 끝
	{
		NextSpeakerIndex = 1;
		DialogueRound = 1;
		UE_LOG(LogDialogue, Display, TEXT("[대화 흐름] 상인 질문 끝 → 다음: 손님1 (index 1)"));
	}
	else if (DialogueRound == 1 && number == 1)  // 1번 손님 반응 끝
	{
		NextSpeakerIndex = 2;
		DialogueRound = 2;
		UE_LOG(LogDialogue, Display, TEXT("[대화 흐름] 손님1 반응 끝 → 다음: 손님2 (index 2)"));
	}
	else if (DialogueRound == 2 && number == 2)  // 2번 손님 반응 끝
	{
		NextSpeakerIndex = 3;
		DialogueRound = 3;
		UE_LOG(LogDialogue, Display, TEXT("[대화 흐름] 손님2 반응 끝 → 다음: 손님3 (index 3)"));
	}
	else if (DialogueRound == 3 && number == 3)  // 3번 손님 반응 끝
	{
		NextSpeakerIndex = 0;
		DialogueRound = 4;
		UE_LOG(LogDialogue, Display, TEXT("[대화 흐름] 손님3 반응 끝 → 다음: 상인 답변 (index 0)"));
	}
	else if (DialogueRound == 4 && number == 0)  // 0번 상인 답변 끝
	{
		NextSpeakerIndex = 1;
		DialogueRound = 1;  // 다시 손님1부터 반복
		UE_LOG(LogDialogue, Display, TEXT("[대화 흐름] 상인 답변 끝 → 다시 손님1부터 반복 (index 1)"));
	}
	else
	{
		UE_LOG(LogDialogue, Error, TEXT("[오류] 예상하지 못한 대화 상태! number: %d, Round: %d"), number, DialogueRound);
		return;
	}

	// 다음 화자에게 Talk 이벤트 전송
	if (NextSpeakerIndex >= 0 && subTargets.IsValidIndex(NextSpeakerIndex) && subTargets[NextSpeakerIndex])
	{
		ACrowd* NextCrowd = subTargets[NextSpeakerIndex]->currentCrowd;
		if (NextCrowd)
		{
			// 다음 화자의 델리게이트 바인딩 (중복 방지를 위해 먼저 언바인딩)
			if (NextCrowd->AudioComp)
			{
				NextCrowd->AudioComp->OnAudioFinished.RemoveDynamic(this, &ACrowdTargetPoint::OnVoiceEnd);
				NextCrowd->AudioComp->OnAudioFinished.AddDynamic(this, &ACrowdTargetPoint::OnVoiceEnd);

				// CurrentTalker 업데이트
				CurrentTalker = NextCrowd;
				CurrentTalkNumber = NextSpeakerIndex;

				UE_LOG(LogDialogue, Log, TEXT("[델리게이트] 다음 화자 바인딩 완료 - Index %d"), NextSpeakerIndex);
			}

			ACrowdAiController* NextController = Cast<ACrowdAiController>(NextCrowd->GetController());
			if (NextController && NextController->StateTreeComp)
			{
				FStateTreeEvent TalkEvent;
				TalkEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.Talk");
				NextController->StateTreeComp->SendStateTreeEvent(TalkEvent.Tag);

				UE_LOG(LogDialogue, Display, TEXT("[이벤트 전송] Talk 이벤트 전송 성공 → Index %d"), NextSpeakerIndex);
			}
			else
			{
				UE_LOG(LogDialogue, Error, TEXT("[오류] NextController 또는 StateTreeComp가 nullptr! Index: %d"), NextSpeakerIndex);
			}
		}
		else
		{
			UE_LOG(LogDialogue, Error, TEXT("[오류] subTargets[%d]에 Crowd가 없음!"), NextSpeakerIndex);
		}
	}
	else
	{
		UE_LOG(LogDialogue, Error, TEXT("[오류] 유효하지 않은 NextSpeakerIndex: %d"), NextSpeakerIndex);
	}

	// 현재 화자의 델리게이트 언바인딩
	if (CurrentTalker && CurrentTalker->AudioComp)
	{
		CurrentTalker->AudioComp->OnAudioFinished.RemoveDynamic(this, &ACrowdTargetPoint::OnVoiceEnd);
		UE_LOG(LogDialogue, Log, TEXT("[델리게이트] 언바인딩 완료 - Index %d"), number);
	}
}



