// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetPointManager.h"

#include "Crowd.h"
#include "CrowdTargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ATargetPointManager::ATargetPointManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATargetPointManager::BeginPlay()
{
	Super::BeginPlay();
	CollectHomeTargetPoints();
	ScatterTargetPoints();
	
}

// Called every frame
void ATargetPointManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATargetPointManager::CollectHomeTargetPoints()
{
	UE_LOG(LogTemp,Warning,TEXT("집포인트 모으기 함수 "));
	TArray<AActor*> TargetPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ACrowdTargetPoint::StaticClass(),TargetPoints);
	for (auto TargetPoint : TargetPoints)
	{
		if (ACrowdTargetPoint* targetHomePoint = Cast<ACrowdTargetPoint>(TargetPoint))
		{
			if (targetHomePoint ->IsHome())
			{
				UE_LOG(LogTemp,Warning,TEXT("집포인트 추가됩니다. 모으는중 ````"));
				HomeTargetPoints.Add(targetHomePoint);
			}
		}
	}
}

void ATargetPointManager::ScatterTargetPoints()
{
	UE_LOG(LogTemp,Warning,TEXT("집포인트 뿌려주기 "));
	TArray<AActor*> Crowds;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ACrowd::StaticClass(),Crowds);
	if (HomeTargetPoints.Num() == 0)
	{
		UE_LOG(LogTemp,Warning,TEXT("crowd 가 없대 "));
		return;
	}
	for (int i =0 ; i < Crowds.Num(); i++)
	{
		ACrowd* crowd = Cast<ACrowd>(Crowds[i]);
		if (crowd && crowd->GetGoHomeTargetPoint() == nullptr)
		{
			int32 idx = i % HomeTargetPoints.Num();

			if (HomeTargetPoints.IsValidIndex(idx) && HomeTargetPoints[idx]->IsHome())
			{
				crowd->SetGoHomeTargetPoint(HomeTargetPoints[idx]);
				UE_LOG(LogTemp, Warning, TEXT("Crowd[%d]에 HomeTargetPoint[%d] 할당"), i, idx);
			}

			
		}
	}
}

FVector ATargetPointManager::GetRandomTargetLocation()
{
	
	if (HomeTargetPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("====HomeTargetPoints가 비어있음! 포인트를 배치하세요!"));
		return FVector::ZeroVector;
	}

	int32 randomIndex = FMath::RandRange(0, HomeTargetPoints.Num() - 1); // rand() 대신 FMath 사용
	FVector randomLoc = HomeTargetPoints[randomIndex]->GetActorLocation();

	UE_LOG(LogTemp, Warning, TEXT("====랜덤 타겟 선택: Index %d, 위치: %s"), randomIndex, *randomLoc.ToString());

	return randomLoc;

}

