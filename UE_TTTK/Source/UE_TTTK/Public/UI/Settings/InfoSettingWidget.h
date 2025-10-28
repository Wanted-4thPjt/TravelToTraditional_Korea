// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InfoSettingWidget.generated.h"

class UTextBlock;

USTRUCT()
struct UE_TTTK_API FInfoSettingProperties
{
	GENERATED_BODY()
	
};

/**
 * 
 */
UCLASS()
class UE_TTTK_API UInfoSettingWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class USettingWidget;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bEditable = false;
};
