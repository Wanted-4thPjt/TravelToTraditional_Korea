// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TargetPointManager.generated.h"

UCLASS()
class UE_TTTK_API ATargetPointManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATargetPointManager();
	TArray<class ACrowdTargetPoint*> HomeTargetPoints;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CollectHomeTargetPoints();
	void ScatterTargetPoints();
	FVector GetRandomTargetLocation();
};
