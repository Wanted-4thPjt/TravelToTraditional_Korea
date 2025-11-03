// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFlow/TTTKGameMode.h"
#include "OnlineSubsystem.h"
#include "Online.h"
#include "MainPlayer.h"
#include "Network/SteamSessionSubsystem.h"

ATTTKGameMode::ATTTKGameMode()
{
	static ConstructorHelpers::FClassFinder<AMainPlayer> playerClass(TEXT("/Game/YKD/BluePrint/BP_MainCharacter.BP_MainCharacter_C"));
	if (playerClass.Succeeded())
	{
		DefaultPawnClass = playerClass.Class;
	}
	
}

void ATTTKGameMode::OnPostLogin(AController* inPlayer)
{
	Super::OnPostLogin(inPlayer);
	
	if (USteamSessionSubsystem* onlineSubsys = GetGameInstance()->GetSubsystem<USteamSessionSubsystem>())
	{
		onlineSubsys->RefreshSessionInfo(+1);
	}
}

void ATTTKGameMode::Logout(AController* outPlayer)
{
	Super::Logout(outPlayer);

	if (USteamSessionSubsystem* onlineSubsys = GetGameInstance()->GetSubsystem<USteamSessionSubsystem>())
	{
		onlineSubsys->RefreshSessionInfo(-1);
	}
}

void ATTTKGameMode::BeginPlay()
{
	Super::BeginPlay();
	

}
