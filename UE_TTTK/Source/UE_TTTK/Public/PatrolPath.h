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
	int idx;
	bool visited = false;
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
	FPatrolPoint* FindNextPoint(int32 idx);
	FPatrolPoint* FindPervPoint(int32 currentIdx);
	int32 GetFinalPatrolPointIdx();
	bool isFinalPoint(int32 idx);
	void ClearPatrolPoints();
	


	
	UFUNCTION()
	void InitializePatrolPoints();
	
	

};
