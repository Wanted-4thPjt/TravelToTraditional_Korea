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

	// AI Controller Tick 비활성화 (성능 최적화)
	PrimaryActorTick.bCanEverTick = false;
}

void ACrowdAiController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwnerCrowd = Cast<ACrowd>(InPawn);

	if (StateTreeComp)
	{
		StateTreeComp->StartLogic();
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

	MoveToLocation(targetLocation);
}
void ACrowdAiController::NotifyArrived()
{
	
}

void ACrowdAiController::Patrol()
{

	if (!OwnerCrowd)
	{
		return;
	}

	if (APatrolArmy* army = Cast<APatrolArmy>(OwnerCrowd))
	{

		FPatrolPoint* nextPoint = army->GetcurrentPatrolPoint();
		if (nextPoint)
		{
			WalkToLocation(nextPoint->PatrolLocation);
		}
		
		return;
	}
	else
	{
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

	if (!OwnerCrowd)
	{
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

		TargetPointManager = Cast<ATargetPointManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ATargetPointManager::StaticClass())
		);

		if (!TargetPointManager)
		{
			return FVector::ZeroVector;
		}
	}

	FVector RandomLocation = TargetPointManager->GetRandomTargetLocation();
	return RandomLocation;

	
}

