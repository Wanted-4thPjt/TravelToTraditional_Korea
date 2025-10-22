// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "SteamSessionSettings.generated.h"

class UPDA_MapList;
/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Steam Session"))
class UE_TTTK_API USteamSessionSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category = "Maps", meta = (AllowedClasses = "/Script/UE_TTTK.PDA_MapList"))
	TSoftObjectPtr<UPDA_MapList> mapListAsset;

	static const USteamSessionSettings* Get()
	{
		return GetDefault<USteamSessionSettings>();
	}
};
