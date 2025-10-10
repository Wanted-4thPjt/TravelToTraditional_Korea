// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerSubSystem.h"
#include "UE_TTTKCharacter.h"
#include "MainPlayer.generated.h"

enum class EMappingMode : uint8;

/**
 * 
 */
UCLASS()
class UE_TTTK_API AMainPlayer : public AUE_TTTKCharacter
{
	GENERATED_BODY()
protected:
	class UInputComponent* myInputComponent;

public:
	// Boarding State
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Boarding")
	bool bIsRidingCarriage = false;

	UPROPERTY(BlueprintReadOnly, Category = "Boarding")
	class ACarriageVehicle* CurrentCarriage = nullptr;

protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void RequestChangeInputMapping(EMappingMode mode);
	EMappingMode currentMdode = EMappingMode::Content1; // 현재 모드

	// Boarding System
	void HandleFKeyPress();
	ACarriageVehicle* FindNearbyCarriage();

public:
	EMappingMode GetCurrentMode(){return currentMdode;}
	void ReturnToDefaultMode();

	// Boarding Functions
	UFUNCTION(BlueprintCallable, Category = "Boarding")
	void RequestBoardCarriage(ACarriageVehicle* Carriage);

	UFUNCTION(Server, Reliable, Category = "Boarding")
	void Server_RequestBoardCarriage(ACarriageVehicle* Carriage);

	UFUNCTION(BlueprintCallable, Category = "Boarding")
	void RequestExitCarriage();

	UFUNCTION(Server, Reliable, Category = "Boarding")
	void Server_RequestExitCarriage(ACarriageVehicle* Carriage);

	UFUNCTION(BlueprintCallable, Category = "Boarding")
	bool IsRidingCarriage() const { return bIsRidingCarriage; }

	// Camera Functions (called from CarriageVehicle Multicast)
	void SwitchToFirstPersonCamera();
	void SwitchToThirdPersonCamera();
};
