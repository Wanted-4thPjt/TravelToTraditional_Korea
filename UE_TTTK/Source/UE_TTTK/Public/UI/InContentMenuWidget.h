// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InContentMenuWidget.generated.h"

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

public:
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* settingsWidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	UButton* continueButton;
	UPROPERTY(meta=(BindWidget))
	UButton* exitButton;
};
