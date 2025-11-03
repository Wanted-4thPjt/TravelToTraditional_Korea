// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Settings/NetworkSettingWidget.h"

#include "UI/Settings/SettingWidget.h"

void UNetworkSettingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	settingWidget = Cast<USettingWidget>(GetOuter());
}
