// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GraphicSettingWidget.generated.h"

class UTextBlock;
class USlider;
class UComboBoxKey;
class UCheckBox;

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
	virtual void NativeConstruct() override;

private:
	UFUNCTION()
	void OnBrightnessValueChanged(float inChangeValue);
	UFUNCTION()
	void OnToggleShowFPSCheckBox(bool bIsChecked);
	UFUNCTION()
	void OnAntiAliasingSelectionChanged(FName selectedItem, ESelectInfo::Type selectionType);

protected:
	UPROPERTY()
	USettingWidget* settingWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bEditable = false;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USlider> gammaSlider;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> brightnessValue;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UComboBoxKey> antiAliasingDropdown;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCheckBox> fpsShowingCheckBox;
};
