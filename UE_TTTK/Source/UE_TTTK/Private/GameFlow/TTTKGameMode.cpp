// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFlow/TTTKGameMode.h"
#include "OnlineSubsystem.h"
#include "Online.h"
#include "MainPlayer.h"

ATTTKGameMode::ATTTKGameMode()
{
	static ConstructorHelpers::FClassFinder<AMainPlayer> playerClass(TEXT("/Game/YKD/BluePrint/BP_MainCharacter.BP_MainCharacter_C"));
	if (playerClass.Succeeded())
	{
		DefaultPawnClass = playerClass.Class;
	} 
}

void ATTTKGameMode::BeginPlay()
{
	Super::BeginPlay();
	

}
