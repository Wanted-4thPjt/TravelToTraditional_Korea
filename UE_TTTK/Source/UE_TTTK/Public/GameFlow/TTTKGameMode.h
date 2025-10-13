// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TTTKGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UE_TTTK_API ATTTKGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ATTTKGameMode();

protected:
	virtual void BeginPlay() override;
};
