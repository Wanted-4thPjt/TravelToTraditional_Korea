// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GraphicSettingWidget.generated.h"

class UTextBlock;
class USlider;
class UComboBoxString;

USTRUCT()
struct UE_TTTK_API FGraphicSettingProperties
{
	GENERATED_BODY()
	
};

/**
 * 
 */
UCLASS()
class UE_TTTK_API UGraphicSettingWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class USettingWidget;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bEditable = false;
};
