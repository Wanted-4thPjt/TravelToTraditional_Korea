// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Settings/GraphicSettingWidget.h"

#include "Components/CheckBox.h"
#include "Components/ComboBoxKey.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Data/TTTKUserSettings.h"
#include "UI/Settings/SettingWidget.h"

void UGraphicSettingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	settingWidget = Cast<USettingWidget>(GetOuter());

	gammaSlider->OnValueChanged.AddDynamic(this, &UGraphicSettingWidget::OnBrightnessValueChanged);

	FCheckBoxStyle checkBoxWidgetStyle = fpsShowingCheckBox->GetWidgetStyle();
	checkBoxWidgetStyle.CheckedHoveredImage.ImageSize = FVector2f(40.f, 40.f);
	checkBoxWidgetStyle.CheckedImage.ImageSize = FVector2f(40.f, 40.f);
	checkBoxWidgetStyle.CheckedHoveredImage.ImageSize = FVector2f(40.f, 40.f);
	checkBoxWidgetStyle.CheckedPressedImage.ImageSize = FVector2f(40.f, 40.f);
	checkBoxWidgetStyle.UncheckedImage.ImageSize = FVector2f(40.f, 40.f);
	checkBoxWidgetStyle.UncheckedHoveredImage.ImageSize = FVector2f(40.f, 40.f);
	checkBoxWidgetStyle.UncheckedPressedImage.ImageSize = FVector2f(40.f, 40.f);
	fpsShowingCheckBox->SetWidgetStyle(checkBoxWidgetStyle);
	fpsShowingCheckBox->OnCheckStateChanged.AddDynamic(this, &UGraphicSettingWidget::OnToggleShowFPSCheckBox);

	antiAliasingDropdown->AddOption(FName("None"));
	antiAliasingDropdown->AddOption(FName("FXAA"));
	antiAliasingDropdown->AddOption(FName("TAA"));
	antiAliasingDropdown->AddOption(FName("TSR"));
	antiAliasingDropdown->OnSelectionChanged.AddDynamic(this, &UGraphicSettingWidget::OnAntiAliasingSelectionChanged);
}

void UGraphicSettingWidget::OnBrightnessValueChanged(float inChangeValue)
{
	brightnessValue->SetText(FText::AsNumber(inChangeValue));
	settingWidget->ExecuteCommand(FString::Printf(TEXT("r.gamma %f"), inChangeValue));
}

void UGraphicSettingWidget::OnToggleShowFPSCheckBox(bool bIsChecked)
{
	if (!settingWidget) {return;}
	if (bIsChecked)
	{
		
	}
}

void UGraphicSettingWidget::OnAntiAliasingSelectionChanged(FName selectedItem, ESelectInfo::Type selectionType)
{
	if (!settingWidget) {return;}
	if (selectionType != ESelectInfo::OnMouseClick) {return;}
	
	FString command = "r.AntiAliasingMethod ";
	
	
	if (selectedItem == FName("None"))
	{
		command += FString::FromInt(EAntiAliasingMethod::AAM_None);
	}
	else if (selectedItem == FName("FXAA"))
	{
		command += FString::FromInt(EAntiAliasingMethod::AAM_FXAA);
	}
	if (selectedItem == FName("TAA"))
	{
		command += '2';
	}
	else if (selectedItem == FName("TSR"))
	{
		command += '4';
	}

	settingWidget->ExecuteCommand(command);
}
