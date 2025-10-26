// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Jegi.generated.h"

class AMainPlayer;
class UInteractableComponent;
class USphereComponent;

UENUM()
enum class EKickTiming : uint8
{
	Default = 0 UMETA(Hidden),
	Missed = 5,
	Good = 10,
	Great = 15,
	Perfect = 20,
	End = 25
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
	UFUNCTION()
	void TempInteract(APawn* player);

	UFUNCTION()
	void OnStart();
	UFUNCTION()
	void OnEnd(APawn* player);
	
	UFUNCTION(BlueprintCallable)
	void OnKick(EKickTiming inTiming);
	void TempKick();

	UFUNCTION(BlueprintCallable)
	void OnReachToGround(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnKicked(EKickTiming inTiming);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnReachToGround();

	static float FindCylinderMeshRadius(const FVector& meshBoxExtent);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> sphereCollider = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> meshComponent = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> movementComponent = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> tempCameraActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* originalCameraActor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float groundHeight = 0.f;
};
