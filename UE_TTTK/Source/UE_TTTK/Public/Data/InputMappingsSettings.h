// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "InputMappingsSettings.generated.h"

USTRUCT(BlueprintType)
struct UE_TTTK_API FInputMappingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> inputMappingContext;

	UPROPERTY(EditAnywhere)
	TMap<FName, UInputAction*> inputActions;
};

/**
 * 
 */
UCLASS(Config=Game, meta=(DisplayName="CachedInputContexts"))
class UE_TTTK_API UInputMappingsSettings : public UDeveloperSettings
{
	GENERATED_BODY()

protected:
	virtual void PostInitProperties() override;

public:
	UPROPERTY(Config, EditAnywhere, Category="InputMappingData", meta=(ToolTip="/Script/Data/InputMappingSettings"))
	TMap<FName, FInputMappingData> inputMappings;

	TMap<FName, FInputMappingData> prevMappings;

	

	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;

	static const UInputMappingsSettings* Get()
	{
		return GetDefault<UInputMappingsSettings>();
	}
};
