// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/SettingsData.h"
#include "NetworkSettingWidget.generated.h"

class UTextBlock;
class UEditableTextBox;




/**
 * 
 */
UCLASS()
class UE_TTTK_API UNetworkSettingWidget : public UUserWidget
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
