// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/MainMenuGameMode.h"

#include "MainMenu/MainMenuController.h"
#include "MainMenu/MainMenuPawn.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	PlayerControllerClass = AMainMenuController::StaticClass();
	DefaultPawnClass = AMainMenuPawn::StaticClass();
	DefaultPlayerName = FText::FromString("관광객");
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();
}
