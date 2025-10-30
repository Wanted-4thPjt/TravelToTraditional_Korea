// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Settings/InfoSettingWidget.h"

#include "UI/Settings/SettingWidget.h"

void UInfoSettingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	settingWidget = Cast<USettingWidget>(GetOuter());
}
