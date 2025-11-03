// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/SettingsData.h"
#include "ControllerSettingWidget.generated.h"

class UTextBlock;
class USlider;


/**
 * 
 */
UCLASS()
class UE_TTTK_API UControllerSettingWidget : public UUserWidget
{
	GENERATED_BODY()

	friend class USettingWidget;

protected:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY()
	USettingWidget* settingWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bEditable = false;
};
