// Fill out your copyright notice in the Description page of Project Settings.


#include "Carriage/CarriagePathActor.h"

#include "VectorTypes.h"
#include "Carriage/CarriageStopPoint.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ACarriagePathActor::ACarriagePathActor()
{
	PrimaryActorTick.bCanEverTick = false;

	// Spline Component 생성
	PathSpline = CreateDefaultSubobject<USplineComponent>(TEXT("PathSpline"));
	RootComponent = PathSpline;

	// 기본값
	PathNme = TEXT("마차 경로");
	StopPointSearchRadius = 500.0f; // 5m
}

// Called when the game starts or when spawned
void ACarriagePathActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACarriagePathActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	CollectStopPoint();
}

void ACarriagePathActor::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.Property!= nullptr)
	{
		FName ProperyName = PropertyChangedEvent.Property->GetFName();
		if (ProperyName == GET_MEMBER_NAME_CHECKED(ACarriagePathActor,StopPointSearchRadius))
		{
			CollectStopPoint();
		}
	}
}

// Called every frame
void ACarriagePathActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

float ACarriagePathActor::GetPathLength() const
{
	return PathSpline ? PathSpline->GetSplineLength() : 0.0f;
}



FVector ACarriagePathActor::GetLocationAtDistance(float Distance) const
{
	if (!PathSpline) return FVector::ZeroVector;
	return PathSpline->GetLocationAtDistanceAlongSpline(Distance,ESplineCoordinateSpace::World);
}

FRotator ACarriagePathActor::GetRotationAtDistance(float Distance) const
{
	if (!PathSpline) return FRotator::ZeroRotator;
	return PathSpline->GetRotationAtDistanceAlongSpline(Distance,ESplineCoordinateSpace::World);
}

void ACarriagePathActor::CollectStopPoint()
{
	StopPoints.Empty();
	if (!PathSpline || !GetWorld())return;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ACarriageStopPoint::StaticClass(),FoundActors);
	if (FoundActors.Num() > 0)
	{
		for (AActor* FoundActor : FoundActors)
		{
			ACarriageStopPoint* StopPoint = Cast<ACarriageStopPoint>(FoundActor);
			if (!StopPoint) continue;

			FVector StopLocation = StopPoint->GetActorLocation();
			float InputKey = PathSpline->FindInputKeyClosestToWorldLocation(StopLocation);
			FVector closestLocation = PathSpline->GetLocationAtSplineInputKey(InputKey,ESplineCoordinateSpace::World);
			float distance = FVector::Dist(closestLocation, StopLocation);
			if (distance<=StopPointSearchRadius)
			{
				StopPoints.Add(StopPoint);
			}
		}
		StopPoints.Sort([](const ACarriageStopPoint& A,const ACarriageStopPoint& B )
		{
			return A.StopIndex < B.StopIndex;
		});
		UE_LOG(LogTemp,Warning,TEXT("[%s] 정류장 %d 개 수집 완료!!"),*PathNme,StopPoints.Num());
		
	}
}

ACarriageStopPoint* ACarriagePathActor::FindNearestStopPoint(float Distance, float Tolerance)
{
	if (!PathSpline) return nullptr;

	for (ACarriageStopPoint* StopPoint : StopPoints)
	{
		if (!StopPoint || StopPoint->bVisited) continue;

		// 정류장의 Spline 상 거리 계산 (InputKey가 아닌 실제 Distance)
		FVector StopLocation = StopPoint->GetActorLocation();
		float InputKey = PathSpline->FindInputKeyClosestToWorldLocation(StopLocation);
		float StopDistance = PathSpline->GetDistanceAlongSplineAtSplineInputKey(InputKey);

		if (FMath::Abs(Distance - StopDistance) <= Tolerance)
		{
			return StopPoint;
		}
	}
	return nullptr;
}

