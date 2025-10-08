// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarriageMovementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UCarriageMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCarriageMovementComponent();

	UPROPERTY(BlueprintReadOnly, Category=Movement)
	float CurrentDistance;
	UPROPERTY(BlueprintReadOnly, Category=Movement)
	bool bisMoving;
	UPROPERTY(BlueprintReadOnly, Category=Movement)
	bool bisStoped;
	UPROPERTY(BlueprintReadOnly,Category=Movement)
	float stopTimer;
	UPROPERTY(BlueprintReadOnly,Category=Movement)
	class ACarriageStopPoint* CurrentStopPoint;
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	FTimerHandle MovementTimerHande;

	class ACarriageVehicle* OwnerCarriage;
	class ACarriagePathActor* CachedPathActor;

public:
	// Called every frame
	UFUNCTION(BlueprintCallable, Category = "CarriageMovementComponent")
	void StartMovement();
	UFUNCTION(BlueprintCallable, Category = "CarriageMovementComponent")
	void StopMovement();

	UFUNCTION()
	void UpateMovement();
	UFUNCTION()
	void CheckStopPoints();
	UFUNCTION()
	void BeginStop(ACarriageStopPoint* StopPoint);
	UFUNCTION()
	void UpdateStopTimer(float DeltaTime);
	
};
