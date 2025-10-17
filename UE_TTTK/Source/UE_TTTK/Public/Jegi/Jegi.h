// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Jegi.generated.h"

UENUM()
enum class EKickTiming : uint8
{
	Default = 0 UMETA(Hidden),
	Missed,
	Good,
	Great,
	Perfect
};

class UProjectileMovementComponent;

UCLASS()
class UE_TTTK_API AJegi : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AJegi();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void OnKick(EKickTiming inTiming);

private:
	UFUNCTION(Server, Reliable)
	void Server_OnKicked(EKickTiming inTiming);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = ProjectileMovement, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* meshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* movementComponent;
};
