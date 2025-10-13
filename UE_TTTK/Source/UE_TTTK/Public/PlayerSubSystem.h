// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "PlayerSubSystem.generated.h"

/**
 * 
 */
class AMainPlayer;
UENUM(BlueprintType)
enum class EMappingMode : uint8
{
	Content1,
	Content2,
	Content3
};
UCLASS()
class UE_TTTK_API UPlayerSubSystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	//사용자가 지닐 수 있는 맵핑 컨텍스트
	UPROPERTY(EditDefaultsOnly)
	TMap<EMappingMode,UInputMappingContext*> InputMappings;
	


	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	bool ChangeInputMapping(AMainPlayer* requester,EMappingMode mode);
	void SetUpMappingContext(AMainPlayer* requester,EMappingMode mode);
	UInputMappingContext* GetInputMappingContext(EMappingMode mode);
	
	
	
};
