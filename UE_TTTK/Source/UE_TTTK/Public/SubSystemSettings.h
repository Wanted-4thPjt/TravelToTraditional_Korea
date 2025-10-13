// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Engine/DeveloperSettings.h"
#include "SubSystemSettings.generated.h"

/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "Player Subsystem Settings"))
class UE_TTTK_API USubSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	//사용자가 지닐 수 있는 맵핑 컨텍스트
public:
	
	USubSystemSettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("My Input Settings");
	}
	
	UPROPERTY(Config, EditAnywhere, Category = "Input Mappings")
	TSoftObjectPtr<UInputMappingContext> Content1Context;

	UPROPERTY(Config, EditAnywhere, Category = "Input Mappings")
	TSoftObjectPtr<UInputMappingContext> Content2Context;

	UPROPERTY(Config, EditAnywhere, Category = "Input Mappings")
	TSoftObjectPtr<UInputMappingContext> Content3Context;

	// 싱글톤 접근
	static const USubSystemSettings* Get()
	{
		return GetDefault<USubSystemSettings>();
	}
};
