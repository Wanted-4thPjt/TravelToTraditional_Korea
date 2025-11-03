// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Settings/ControllerSettingWidget.h"

#include "UI/Settings/SettingWidget.h"

void UControllerSettingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	settingWidget = Cast<USettingWidget>(GetOuter());
}
