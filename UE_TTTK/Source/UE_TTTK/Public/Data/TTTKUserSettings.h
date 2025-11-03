// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "SceneUtils.h"
#include "Data/SettingsData.h"
#include "TTTKUserSettings.generated.h"

#define UE_GAMEUSERSETTINGS_VERSION 1



/**
 * 
 */
UCLASS(Config=UGameUserSettings)
class UE_TTTK_API UTTTKUserSettings : public UGameUserSettings
{
	GENERATED_BODY()	
	
public:
	UFUNCTION(BlueprintCallable)
	static UTTTKUserSettings* Get() {return GetMutableDefault<UTTTKUserSettings>();}
	
	virtual void LoadSettings(bool bForceReload = false) override;
	virtual void ApplySettings(bool bCheckForCommandLineOverrides) override;
	virtual void SaveSettings() override;
	
	void UpdateGraphicSettings(struct FGraphicSettingProperties inSetting);
	void UpdateControllerSetting(struct FControllerSettingProperties inSetting);
	void UpdateNetworkSetting(struct FNetworkSettingProperties inSetting);
	void UpdateInfoSetting(struct FInfoSettingProperties inSetting);
	
private:
	FString GetCurrentUserID();
	
private:
	FGraphicSettingProperties graphicSetProperties;
	FControllerSettingProperties controllerSetProperties;
	FNetworkSettingProperties networkSetProperties;
	FInfoSettingProperties infoSetProperties;
};
