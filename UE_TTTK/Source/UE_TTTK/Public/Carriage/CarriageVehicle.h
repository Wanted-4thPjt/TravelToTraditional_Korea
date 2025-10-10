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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Component")
	class UStaticMeshComponent* WheelMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class UCarriageMovementComponent* MovementComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class UBoxComponent* BoardArea;

	// Ground Trace Points
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class USceneComponent* FrontTracePoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class USceneComponent* RearTracePoint;

	// Seat System
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class USceneComponent* Seat1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	class USceneComponent* Seat2;
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Carriage")
	class ACarriagePathActor* PathActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carriage")
	float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carriage")
	bool bLooping;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carriage")
	float WheelSpeed = 25.f;
	
	

	// Passenger Management (Replicated)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Boarding")
	TArray<class AMainPlayer*> PassengerList;

	// Players in BoardArea range
	UPROPERTY()
	TArray<class AMainPlayer*> PlayersInRange;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// BoardArea Overlap Events
	UFUNCTION()
	void OnBoardAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoardAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	UFUNCTION(BlueprintCallable, Category = "Carriage")
	void StartMovement();
	UFUNCTION(BlueprintCallable, Category = "Carriage")
	void StopMovement();
	UFUNCTION()
	ACarriagePathActor* GetPathActor(){return PathActor;};
	UFUNCTION(BlueprintCallable, Category = "Carriage")
	void RotateWheel();

	// Boarding System
	UFUNCTION(Server, Reliable, Category = "Boarding")
	void Server_RequestBoard(AMainPlayer* Player);

	UFUNCTION(NetMulticast, Reliable, Category = "Boarding")
	void Multicast_OnPlayerBoarded(AMainPlayer* Player, int32 SeatIndex);

	UFUNCTION(Server, Reliable, Category = "Boarding")
	void Server_RequestExit(AMainPlayer* Player);

	UFUNCTION(NetMulticast, Reliable, Category = "Boarding")
	void Multicast_OnPlayerExited(AMainPlayer* Player);

	// Helper Functions
	UFUNCTION(BlueprintCallable, Category = "Boarding")
	bool CanBoardCarriage() const;

	UFUNCTION(BlueprintCallable, Category = "Boarding")
	int32 GetAvailableSeatIndex() const;

	UFUNCTION(BlueprintCallable, Category = "Boarding")
	USceneComponent* GetSeatComponent(int32 SeatIndex) const;
};


