// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuSteam.h"

#include "OnlineSessionSettings.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "UI/CreatingSession.h"
#include "UI/SessionsEntry.h"

void UMainMenuSteam::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UMainMenuSteam::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	UE_LOG(LogTemp, Warning, TEXT("MainMenuSteam::NativeOnInitialized 호출됨"));

	
	if (IsValid(goLobbyButton))
	{
		goLobbyButton->OnClicked.AddDynamic(this, &UMainMenuSteam::ClickLobbyButton);
		goLobbyButton->SetVisibility(ESlateVisibility::Hidden);
		goLobbyButton->SetIsEnabled(false);
	}
	if (IsValid(goHostButton))
	{
		goHostButton->OnClicked.AddDynamic(this, &UMainMenuSteam::ClickHostButton);
	}
	if (IsValid(goFindButton))
	{
		goFindButton->OnClicked.AddDynamic(this, &UMainMenuSteam::ClickFindButton);
	}
	if (IsValid(exitButton))
	{
		exitButton->OnClicked.AddDynamic(this, &UMainMenuSteam::ClickExit);
	}


}

void UMainMenuSteam::ClickLobbyButton()
{
	lobbySwitcher->SetActiveWidgetIndex(0);
	goLobbyButton->SetVisibility(ESlateVisibility::Hidden);
	goLobbyButton->SetIsEnabled(false);
}

void UMainMenuSteam::ClickHostButton()
{
	lobbySwitcher->SetActiveWidgetIndex(1);
	goLobbyButton->SetVisibility(ESlateVisibility::Visible);
	goLobbyButton->SetIsEnabled(true);
}

void UMainMenuSteam::ClickFindButton()
{
	lobbySwitcher->SetActiveWidgetIndex(2);
	goLobbyButton->SetVisibility(ESlateVisibility::Visible);
	goLobbyButton->SetIsEnabled(true);
}


void UMainMenuSteam::ClickExit()
{
	UE_LOG(LogTemp, Warning, TEXT("ClickExit 호출됨!"));
	GetGameInstance()->Shutdown();

	GetWorld()->EndPlay(EEndPlayReason::Type::Quit);
}
