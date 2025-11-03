// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/MainMenuController.h"
#include "UI/Network/MainMenuSteam.h"

AMainMenuController::AMainMenuController()
{
	ConstructorHelpers::FClassFinder<UMainMenuSteam> finderWidget(TEXT("/Game/UI/MainMenu/WBP_MainMenuSteam.WBP_MainMenuSteam_C"));
	if (finderWidget.Succeeded())
	{
		steamWidgetFactory = finderWidget.Class;
	}
}

void AMainMenuController::BeginPlay()
{
	Super::BeginPlay();

	UMainMenuSteam* widget = CreateWidget<UMainMenuSteam>(this, steamWidgetFactory);
	widget->AddToViewport(3);
	
	FInputModeUIOnly newInputMode;
	newInputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
	SetInputMode(newInputMode);
	SetShowMouseCursor(true);

	
}
