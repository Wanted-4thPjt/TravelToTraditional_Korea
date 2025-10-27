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
	// ========== 땅 충돌 감지 ==========

	/** 땅과 충돌 시 호출 (Component에서 감지) */
	UFUNCTION(BlueprintCallable)
	void OnReachToGround(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	/** 땅 충돌 시 물리 정지 (Multicast) */
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticast_OnReachToGround();

	// ========== 제거됨 - Component로 이동 ==========
	// void TempInteract(APawn* player);
	// void OnStart();
	// void OnEnd(APawn* player);
	// void OnKick(EKickTiming inTiming);
	// void TempKick();
	// void NetMulticast_OnKicked(EKickTiming inTiming);

	static float FindCylinderMeshRadius(const FVector& meshBoxExtent);

protected:
	// ========== Components ==========

	/** 충돌 감지 Collider */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> sphereCollider = nullptr;

	/** 제기 Mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> meshComponent = nullptr;

	/** 물리 Movement Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UProjectileMovementComponent> movementComponent = nullptr;

	// ========== 제거됨 - Content Manager 역할 ==========
	// TArray<AActor*> tempCameraActor;
	// AActor* originalCameraActor;
	// float groundHeight;
};
