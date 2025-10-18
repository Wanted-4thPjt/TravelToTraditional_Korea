// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrowdData.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Character.h"
#include "Crowd.generated.h"

UCLASS()
class UE_TTTK_API ACrowd : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACrowd();
	UPROPERTY(EditDefaultsOnly, Category = "Crowd")
	class UCrowdData* CrowdData;
	UPROPERTY()
	TMap<FString,class ACrowdTargetPoint*> TargetPoints;
	UPROPERTY()
	FVector realTarget;
	UPROPERTY(EditDefaultsOnly, Category = "Ani")
	class UAnimMontage* GreetingMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Ani")
	class UAnimMontage* OuchAnimMontage;
private:
	bool bIsMoving;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	class ACrowdTargetPoint* GetTargetPoint(FString Destination);
	class ACrowdTargetPoint* GetTargetByEnum();
	bool GetIsMoving(){return bIsMoving;}
	void SetIsMoving(bool MovingState){bIsMoving = MovingState;}
	void CollectingTargetPoints();
	void PlayGreeting();

};
