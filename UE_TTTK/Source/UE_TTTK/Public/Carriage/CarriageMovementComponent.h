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

	UPROPERTY(BlueprintReadOnly, Replicated, Category=Movement)
	float CurrentDistance;
	UPROPERTY(BlueprintReadOnly, Replicated, Category=Movement)
	bool bisMoving;
	UPROPERTY(BlueprintReadOnly, Replicated, Category=Movement)
	bool bisStoped;
	UPROPERTY(BlueprintReadOnly,Category=Movement)
	float stopTimer;
	UPROPERTY(BlueprintReadOnly,Category=Movement)
	class ACarriageStopPoint* CurrentStopPoint;
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

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

	// Ground Tracking
	UFUNCTION()
	void TraceGround(const FVector& BaseLocation, const FRotator& BaseRotation, FVector& OutAdjustedLocation, FRotator& OutAdjustedRotation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ground Tracking")
	float TraceDistance = 1000.0f;  // Trace 길이

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ground Tracking")
	float TraceStartOffset = 500.0f;  // 위쪽 시작 오프셋

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ground Tracking")
	float GroundOffset = 50.0f;  // 지면으로부터의 높이 오프셋

};
