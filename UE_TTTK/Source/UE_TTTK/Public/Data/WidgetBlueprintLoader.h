// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "WidgetBlueprintLoader.generated.h"

class UUserWidgetBlueprint;
class UUserWidget;

/**
 * 
 */
UCLASS(Config = Game, meta=(DisplayName="WBP_Loader"))
class UE_TTTK_API UWidgetBlueprintLoader : public UDeveloperSettings
{
	GENERATED_BODY()

protected:
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

public:
	UPROPERTY(Config, EditAnywhere, Category="WBP")
	TMap<TSubclassOf<UUserWidget>, TSubclassOf<UUserWidgetBlueprint>> WBP_ClassContainer;

	static const UWidgetBlueprintLoader* Get()
	{
		return GetDefault<UWidgetBlueprintLoader>();
	}
};
