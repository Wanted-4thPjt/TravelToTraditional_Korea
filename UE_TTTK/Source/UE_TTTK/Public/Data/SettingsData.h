#pragma once

#include "SettingsData.generated.h"

USTRUCT(BlueprintType)
struct UE_TTTK_API FGraphicSettingProperties
{
	GENERATED_BODY()

	// GraphicSettingWidget.h에 구현된 설정들
	float Brightness = 2.2f;
	EAntiAliasingMethod antiAliasingMethod = EAntiAliasingMethod::AAM_None;
	bool bShowFPS = false;

	// GameUserSettings에 구현된 설정들
	float FrameRateLimit = 60.f;
	EWindowMode::Type FullscreenMode = EWindowMode::Type::Windowed;
	int32 ResolutionSizeX = 1280;
	int32 ResolutionSizeY = 960;
	int32 scalabilityQuality = Scalability::DefaultQualityLevel;
	bool bUseVSync = false;

	bool operator==(const FGraphicSettingProperties& comparedSetting) const
	{
		return
			Brightness != comparedSetting.Brightness &&
			antiAliasingMethod != comparedSetting.antiAliasingMethod &&
			bShowFPS != comparedSetting.bShowFPS &&
			FrameRateLimit != comparedSetting.FrameRateLimit &&
			FullscreenMode != comparedSetting.FullscreenMode &&
			ResolutionSizeX != comparedSetting.ResolutionSizeX &&
			ResolutionSizeY != comparedSetting.ResolutionSizeY &&
			scalabilityQuality != comparedSetting.scalabilityQuality &&
			bUseVSync != comparedSetting.bUseVSync
		;
	}
};

USTRUCT(BlueprintType)
struct UE_TTTK_API FControllerSettingProperties
{
	GENERATED_BODY()

	// ControllerSettingWidget.h에 구현된 설정들
	// TODO: 마우스 감도, 카메라 FOV 등 추가 예정

	// GameUserSettings에 구현된 설정들

	bool operator==(const FControllerSettingProperties& comparedSetting) const
	{
		return
			false
		;
	}
};

USTRUCT(BlueprintType)
struct UE_TTTK_API FNetworkSettingProperties
{
	GENERATED_BODY()

	// NetworkSettingWidget.h에 구현된 설정
	// TODO: 네트워크 관련 설정 추가 예정 (예: 서버 지역, Ping 표시 등)

	// GameUserSettings에 구현된 설정들

	bool operator==(const FNetworkSettingProperties& comparedSetting) const
	{
		return
			false
		;
	}
};

USTRUCT(BlueprintType)
struct UE_TTTK_API FInfoSettingProperties
{
	GENERATED_BODY()

	// InfoSettingWidget.h에 구현된 설정
	FString NickName;

	// GameUserSettings에 구현된 설정들
	uint32 Version;

	bool operator==(const FInfoSettingProperties& comparedSetting) const
	{
		return
			NickName == comparedSetting.NickName &&
			Version == comparedSetting.Version
		;
		
	}
};