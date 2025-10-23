// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrolPath.generated.h"

USTRUCT()
struct FPatrolPoint
{
	GENERATED_BODY()

	FVector PatrolLocation;
	int32 idx;
	bool visited = false;

	// 기본 생성자
	FPatrolPoint()
		: PatrolLocation(FVector::ZeroVector), idx(0), visited(false)
	{
	}

	// 매개변수 생성자
	FPatrolPoint(FVector InLocation, int32 InIdx, bool InVisited)
		: PatrolLocation(InLocation), idx(InIdx), visited(InVisited)
	{
	}
};
UCLASS()
class UE_TTTK_API APatrolPath : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APatrolPath();
	class USplineComponent* splineComp;
	TArray<FPatrolPoint*> patrolPoints;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FPatrolPoint* GetPatrolPoint(int32 idx);
	int32 GetNumPatrolPoints();
	FPatrolPoint* FindNextPoint(int32 idx);
	FPatrolPoint* FindPervPoint(int32 currentIdx);
	int32 GetFinalPatrolPointIdx();
	bool isFinalPoint(int32 idx);
	void ClearPatrolPoints();
	bool IsValidIndex(int32 idx);
	


	
	UFUNCTION()
	void InitializePatrolPoints();
	
	

};
