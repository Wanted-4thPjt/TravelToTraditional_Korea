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
	
}

// Called every frame
void APatrolPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FPatrolPoint* APatrolPath::GetPatrolPoint(int32 idx)
{
	return patrolPoints[idx];
}

FPatrolPoint* APatrolPath::FindNextPoint(int32 idx)
{
	return patrolPoints[idx+1];
}

FPatrolPoint* APatrolPath::FindPervPoint(int32 currentIdx)
{
	return patrolPoints[currentIdx-1];
}

int32 APatrolPath::GetFinalPatrolPointIdx()
{
	return patrolPoints.Num()-1;
}

bool APatrolPath::isFinalPoint(int32 idx)
{
	if (patrolPoints.Num()-1 >= idx)
	{
		return true;
	}
	return false;
}

void APatrolPath::ClearPatrolPoints()
{
	patrolPoints.Empty();
	
}


void APatrolPath::InitializePatrolPoints()
{
	
	int splinePointNums = splineComp->GetNumberOfSplinePoints();
	for (int i=0;i<splinePointNums;i++)
	{
		FVector pointLocation = splineComp->GetLocationAtSplinePoint(i,ESplineCoordinateSpace::World);
		patrolPoints.Add(new FPatrolPoint(pointLocation,i,false));
		
	}
}

