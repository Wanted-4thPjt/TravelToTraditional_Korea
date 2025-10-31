// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdAiController.h"

#include "Crowd.h"
#include "CrowdTargetPoint.h"
#include "NavigationSystem.h"
#include "PatrolArmy.h"
#include "PatrolPath.h"
#include "TargetPointManager.h"
#include "Components/StateTreeAIComponent.h"
#include "NavMesh/NavMeshBoundsVolume.h"
#include "Kismet/GameplayStatics.h"

ACrowdAiController::ACrowdAiController()
{
	StateTreeComp= CreateDefaultSubobject<UStateTreeAIComponent>("StateTreeAI");
}

void ACrowdAiController::OnPossess(APawn* InPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("=== CrowdAiController::OnPossess 호출됨 ==="));
	UE_LOG(LogTemp, Warning, TEXT("Possessing Pawn: %s"), *InPawn->GetName());

	Super::OnPossess(InPawn);

	OwnerCrowd = Cast<ACrowd>(InPawn);
	UE_LOG(LogTemp, Warning, TEXT("OwnerCrowd Cast: %s"), OwnerCrowd ? TEXT("OK") : TEXT("FAILED"));

	if (StateTreeComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("StateTreeComp 존재함 - StartLogic 호출"));
		StateTreeComp->StartLogic();
		UE_LOG(LogTemp, Warning, TEXT("StartLogic 완료"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StateTreeComp가 nullptr! StateTree가 실행되지 않습니다!"));
	}
}

void ACrowdAiController::BeginPlay()
{
	Super::BeginPlay();
	TargetPointManager = Cast<ATargetPointManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ATargetPointManager::StaticClass())
	);
}

void ACrowdAiController::WalkToPoint(AActor* TargetActor)
{
	MoveToActor(TargetActor);
}

void ACrowdAiController::WalkToLocation(FVector targetLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("=== WalkToLocation 호출됨 ==="));
	UE_LOG(LogTemp, Warning, TEXT("목표 위치: %s"), *targetLocation.ToString());

	MoveToLocation(targetLocation);
}
void ACrowdAiController::NotifyArrived()
{
	
}

void ACrowdAiController::Patrol()
{
	UE_LOG(LogTemp, Warning, TEXT("===== Patrol 호출됨 ====="));

	if (!OwnerCrowd)
	{
		UE_LOG(LogTemp, Error, TEXT("Patrol: OwnerCrowd is null!"));
		return;
	}

	if (APatrolArmy* army = Cast<APatrolArmy>(OwnerCrowd))
	{
		UE_LOG(LogTemp, Warning, TEXT("Patrol: PatrolArmy 캐스팅 성공 - %s"), *army->GetName());

		FPatrolPoint* nextPoint = army->GetcurrentPatrolPoint();
		if (nextPoint)
		{
			UE_LOG(LogTemp, Warning, TEXT("Patrol: 다음 목적지로 이동 명령 - %s"), *nextPoint->PatrolLocation.ToString());
			WalkToLocation(nextPoint->PatrolLocation);
		}
		
		return;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Patrol: OwnerCrowd는 PatrolArmy가 아님!"));
	}
}


void ACrowdAiController::Talk(ACrowd* crowd)
{
	//crowd->Talk()
}


void ACrowdAiController::GoMarket()
{
}

void ACrowdAiController::GoHome()
{
	
}

void ACrowdAiController::Greeting()
{
	UE_LOG(LogDialogue, Display, TEXT("[Greeting] 인사 애니메이션 재생"));

	if (!OwnerCrowd)
	{
		UE_LOG(LogDialogue, Error, TEXT("[오류] OwnerCrowd가 nullptr!"));
		return;
	}

	// 인사 애니메이션만 재생 (대화는 모두 도착 후 자동 시작)
	OwnerCrowd->PlayGreeting();
}

void ACrowdAiController::Waiting()
{
	
}

void ACrowdAiController::Ouch()
{
	OwnerCrowd->PlayOuch();
}

FVector ACrowdAiController::FindRandomDestination()
{
	
	// nullptr 체크!
	if (!TargetPointManager)
	{
		UE_LOG(LogTemp, Error, TEXT("====TargetPointManager가 nullptr! 다시 찾기 시도"));

		TargetPointManager = Cast<ATargetPointManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ATargetPointManager::StaticClass())
		);

		if (!TargetPointManager)
		{
			UE_LOG(LogTemp, Error, TEXT("====TargetPointManager를 찾을 수 없음! 맵에 배치되어 있나요?"));
			return FVector::ZeroVector;
		}
	}

	FVector RandomLocation = TargetPointManager->GetRandomTargetLocation();
	UE_LOG(LogTemp, Error, TEXT("====랜덤 위치: %s"), *RandomLocation.ToString());
	return RandomLocation;

	
}

