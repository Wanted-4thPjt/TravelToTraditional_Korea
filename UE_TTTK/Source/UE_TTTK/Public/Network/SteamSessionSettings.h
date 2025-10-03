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

	UPROPERTY(Config, EditAnywhere, Category = "Session", meta=(ClampMin=1, ClampMax=10))
	int32 maxPlayers = 3;

	static const USteamSessionSettings* Get()
	{
		return GetDefault<USteamSessionSettings>();
	}
};
