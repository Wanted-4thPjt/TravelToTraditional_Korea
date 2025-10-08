// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarriageVehicle.generated.h"

UCLASS()
class UE_TTTK_API ACarriageVehicle : public AActor
{
	GENERATED_BODY()
	

public:
	ACarriageVehicle();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class UStaticMeshComponent* CarriageMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class UCarriageMovementComponent* MovementComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class UBoxComponent* BoardArea;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Carriage")
	class ACarriagePathActor* PathActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carriage")
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carriage")
	bool bLooping;
	
	

protected:
	virtual void BeginPlay() override;
public:
	UFUNCTION(BlueprintCallable, Category = "Carriage")
	void StartMovement();
	UFUNCTION(BlueprintCallable, Category = "Carriage")
	void StopMovement();
	UFUNCTION()
	ACarriagePathActor* GetPathActor(){return PathActor;};
};


