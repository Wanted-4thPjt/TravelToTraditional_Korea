// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrolPath.h"

#include "Components/SplineComponent.h"

// Sets default values
APatrolPath::APatrolPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	splineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	
}

// Called when the game starts or when spawned
void APatrolPath::BeginPlay()
{
	Super::BeginPlay();
	InitializePatrolPoints();
	
}

// Called every frame
void APatrolPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FPatrolPoint* APatrolPath::GetPatrolPoint(int32 idx)
{
	if (IsValidIndex(idx))
	{
		return patrolPoints[idx];
	}
	return nullptr;
}

int32 APatrolPath::GetNumPatrolPoints()
{
	return patrolPoints.Num();
}

FPatrolPoint* APatrolPath::FindNextPoint(int32 idx)
{
	
	if (idx+1 >= patrolPoints.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("FindNextPoint: Invalid index %d"), idx);
		return nullptr;
	}

	int32 nextIdx = idx + 1;
	UE_LOG(LogTemp, Warning, TEXT("FindNextPoint: 현재[%d] -> 다음[%d]"), idx, nextIdx);

	return patrolPoints[nextIdx];
}

FPatrolPoint* APatrolPath::FindPervPoint(int32 currentIdx)
{
	if (currentIdx-1>=0 && currentIdx-1<patrolPoints.Num()-1)
	{
		return patrolPoints[currentIdx-1];
	}
	return nullptr;
}

int32 APatrolPath::GetFinalPatrolPointIdx()
{
	return patrolPoints.Num()-1;
}

bool APatrolPath::isFinalPoint(int32 idx)
{
	if (patrolPoints.Num()-1 >= idx || idx ==0)
	{
		return true;
	}
	return false;
}

void APatrolPath::ClearPatrolPoints()
{
	patrolPoints.Empty();
	
}

bool APatrolPath::IsValidIndex(int32 idx)
{
	if (patrolPoints.Num()<=idx || idx <0)
	{
		return false;
	}
	return true;
}



void APatrolPath::InitializePatrolPoints()
{
	UE_LOG(LogTemp, Warning, TEXT("===== PatrolPath::InitializePatrolPoints 시작 ====="));

	if (!splineComp)
	{
		UE_LOG(LogTemp, Error, TEXT("splineComp is NULL! PatrolPath 초기화 실패!"));
		return;
	}

	int splinePointNums = splineComp->GetNumberOfSplinePoints();
	UE_LOG(LogTemp, Warning, TEXT("Spline Point 개수: %d"), splinePointNums);

	for (int i=0;i<splinePointNums;i++)
	{
		FVector pointLocation = splineComp->GetLocationAtSplinePoint(i,ESplineCoordinateSpace::World);
		patrolPoints.Add(new FPatrolPoint(pointLocation,i,false));
		UE_LOG(LogTemp, Warning, TEXT("[%d] 패트롤 경로 점 추가: %s"), i, *pointLocation.ToString());
	}

	UE_LOG(LogTemp, Warning, TEXT("===== PatrolPath 초기화 완료! 총 %d개 포인트 ====="), patrolPoints.Num());
}

