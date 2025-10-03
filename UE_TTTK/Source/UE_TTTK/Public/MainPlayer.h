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
protected:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void RequestChangeInputMapping(EMappingMode mode);
	EMappingMode currentMdode = EMappingMode::Content1; // 현재 모드
public:
	EMappingMode GetCurrentMode(){return currentMdode;}
	void ReturnToDefaultMode();
};
