// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InContentMenuWidget.generated.h"

class USettingWidget;
class UButton;
class UWidgetSwitcher;
/**
 * 
 */
UCLASS()
class UE_TTTK_API UInContentMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void OnClickedContinue();
	UFUNCTION()
	void OnClickedExit();
	UFUNCTION()
	void OnClickedSetting();

public:
	UPROPERTY(meta=(BindWidget))
	UButton* continueButton;
	UPROPERTY(meta=(BindWidget))
	UButton* settingButton;
	UPROPERTY(meta=(BindWidget))
	UButton* exitButton;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USettingWidget> settingsWidget;
};
