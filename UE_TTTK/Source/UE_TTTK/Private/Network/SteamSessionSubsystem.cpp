// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/SteamSessionSubsystem.h"

#include "Network/SteamSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineSessionInterface.h"

void USteamSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	const USteamSessionSettings* settings = GetDefault<USteamSessionSettings>();
	
}

void USteamSessionSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void USteamSessionSubsystem::CreateSession(FName mapName, int32 maxPlayerCount)
{
	if (IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get())
	{
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
		if (sessions.IsValid())
		{
			sessions->OnCreateSessionCompleteDelegates.AddUObject(
				this, &USteamSessionSubsystem::OnCompleteCreateSession);

			FOnlineSessionSettings sessionSettings;
			sessionSettings.NumPublicConnections = maxPlayerCount;
			sessionSettings.bShouldAdvertise = true;  // open in Steam friend list
			sessionSettings.bAllowJoinInProgress = true;  // allow participate in progress server
			sessionSettings.bIsLANMatch = false;  // must be FALSE
			sessionSettings.bUsesPresence = true;  // for finding friends
			sessionSettings.bAllowJoinViaPresence = true;  // allow friends able to  participate directly
			/* not important. Steam Lobby Type
			sessionSettings.Set(
				FName("setting map"),
				FString("My Map Name"),
				EOnlineDataAdvertisementType::ViaOnlineService
			);*/

			sessions->CreateSession(0, mapName, sessionSettings);
		}
	}
}

void USteamSessionSubsystem::FindSession(int32 maxSearchResult)
{
}

void USteamSessionSubsystem::JoinSession()
{
}

void USteamSessionSubsystem::OnCompleteCreateSession(FName inSessionName, bool bWasSuccess)
{
	if (bWasSuccess)
	{
		//GetWorld()->ServerTravel(mapPath + "?listen");
	}
}

void USteamSessionSubsystem::OnCompleteFindSession(bool bWasSuccess)
{
}
