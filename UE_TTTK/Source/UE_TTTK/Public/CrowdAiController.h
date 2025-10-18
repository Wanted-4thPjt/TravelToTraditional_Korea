// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CrowdAiController.generated.h"


DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnCrowdArrived,class ACrowd*, ArrivedCrowd,FVector,Destination);
UCLASS()
class UE_TTTK_API ACrowdAiController : public AAIController
{
	GENERATED_BODY()
	ACrowdAiController();
public:
	FOnCrowdArrived OnCrowdArrived;
	UPROPERTY(EditDefaultsOnly)
	class UStateTreeAIComponent* StateTreeComp;
	UPROPERTY()
	class ACrowd* OwnerCrowd;
public:
	virtual void OnPossess(APawn* InPawn) override;
	void WalkToPoint(AActor* TargetActor);
	void WalkToLocation(FVector targetLocation);
	void NotifyArrived();
	void Patrol();
	void Talk(ACrowd* crowd);
	void RandomRun();
	void GoMarket();
	void GoHome();
	void Greeting();
	
};


