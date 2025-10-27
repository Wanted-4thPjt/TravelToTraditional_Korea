// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerSubSystem.h"
#include "UE_TTTKCharacter.h"
#include "MainPlayer.generated.h"

enum class EMappingMode : uint8;

class UViewComponent;
class UInteractionComponent;
class UBaseContentManager;
class UBaseContentComponent;
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

public:
	AMainPlayer();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void RequestChangeInputMapping(EMappingMode mode);
	EMappingMode currentMode = EMappingMode::Content1; // 현재 모드

	// Boarding System
	void HandleFKeyPress();
	ACarriageVehicle* FindNearbyCarriage();

public:
	EMappingMode GetCurrentMode() {return currentMode;}
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

	// ========== Content Input 관리 ==========

	/** Content가 활성화될 때 호출 (Lazy Initialization) */
	UFUNCTION(BlueprintCallable, Category="ContentInput")
	void ActivateContentInput(UBaseContentManager* manager);

	/** Content가 비활성화될 때 호출 */
	UFUNCTION(BlueprintCallable, Category="ContentInput")
	void DeactivateContentInput();

	/** 현재 활성화된 Content Input Component */
	UFUNCTION(BlueprintPure, Category="ContentInput")
	UBaseContentComponent* GetActiveContentInput() const { return activeContentInput; }

protected:
	/** 현재 활성화된 Content Input Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="ContentInput")
	UBaseContentComponent* activeContentInput = nullptr;

	/** Cached Content Input Components (재사용) */
	UPROPERTY()
	TMap<TSubclassOf<UBaseContentComponent>, UBaseContentComponent*> cachedContentInputs;

private:
	/** Content Input Component Lazy Initialization */
	UBaseContentComponent* GetOrCreateContentInput(TSubclassOf<UBaseContentComponent> componentClass);

#pragma region VaVamVa
public:
	UFUNCTION(BlueprintCallable)
	AActor* GetFocusedActor() const;
	
protected:
	UFUNCTION()
	void OnViewInteractableActor(const FHitResult& hitResult);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TObjectPtr<UViewComponent> viewComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TObjectPtr<UInteractionComponent> interactionComponent;
#pragma endregion VaVamVa
};
