// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UE_TTTKCharacter.h"
#include "MainPlayer.generated.h"

class UViewComponent;
/**
 * 
 */
UCLASS()
class UE_TTTK_API AMainPlayer : public AUE_TTTKCharacter
{
	GENERATED_BODY()

public:
	AMainPlayer();
	
protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//void RequestChangeInputMapping(EMappingMode mode);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnViewInteractableActor(const FHitResult& hitResult);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	UViewComponent* viewComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	AActor* focusedActor = nullptr;
};
