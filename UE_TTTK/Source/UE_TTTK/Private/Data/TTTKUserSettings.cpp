// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/TTTKUserSettings.h"

#include "Data/SettingsData.h"

void UTTTKUserSettings::LoadSettings(bool bForceReload)
{
	Super::LoadSettings(bForceReload);
}

void UTTTKUserSettings::ApplySettings(bool bCheckForCommandLineOverrides)
{
	Super::ApplySettings(bCheckForCommandLineOverrides);
}

void UTTTKUserSettings::SaveSettings()
{
	Super::SaveSettings();
}

void UTTTKUserSettings::UpdateGraphicSettings(FGraphicSettingProperties inSetting)
{
	if (inSetting == graphicSetProperties) {return;}
}

void UTTTKUserSettings::UpdateControllerSetting(FControllerSettingProperties inSetting)
{
	//if (inSetting == controllerSetProperties) {return;}
}

void UTTTKUserSettings::UpdateNetworkSetting(FNetworkSettingProperties inSetting)
{
	//if (inSetting == networkSetProperties) {return;}
}

void UTTTKUserSettings::UpdateInfoSetting(FInfoSettingProperties inSetting)
{
	if (inSetting == infoSetProperties) {return;}
}

FString UTTTKUserSettings::GetCurrentUserID()
{
	return {};
}
