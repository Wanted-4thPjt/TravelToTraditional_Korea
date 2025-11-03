// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuController.generated.h"

class UMainMenuSteam;
/**
 * 
 */
UCLASS()
class UE_TTTK_API AMainMenuController : public APlayerController
{
	GENERATED_BODY()

public:
	AMainMenuController();
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TSubclassOf<UMainMenuSteam> steamWidgetFactory;
};
