// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/SteamSessionSubsystem.h"

#include "Network/SteamSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemSteam.h"
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
	//steam.GetSessionInterface();
	if (IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get("Steam"))
	{
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
		
		if (sessions.IsValid())
		{
			sessions->OnCreateSessionCompleteDelegates.Clear();
			sessions->OnCreateSessionCompleteDelegates.AddUObject(
				this, &USteamSessionSubsystem::OnCompleteCreateSession);
			
			FOnlineSessionSettings sessionSettings;
			sessionSettings.NumPublicConnections = maxPlayerCount;
			sessionSettings.bShouldAdvertise = true;  // open in Steam friend list
			sessionSettings.bAllowJoinInProgress = true;  // allow participate in progress server
			sessionSettings.bIsLANMatch = false;  // must be FALSE
			sessionSettings.bUsesPresence = true;  // for finding friends
			sessionSettings.bAllowJoinViaPresence = true;  // allow friends able to  participate directly
			sessionSettings.bUseLobbiesIfAvailable = true;
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
	if (IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get("Steam"))
	{
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
		if (sessions.IsValid())
		{
			// 델리게이트 중복 방지
			sessions->OnFindSessionsCompleteDelegates.Clear();
			sessions->OnFindSessionsCompleteDelegates.AddUObject(
				this, &USteamSessionSubsystem::OnCompleteFindSession
			);

			sessionSearch = MakeShareable(new FOnlineSessionSearch());
			sessionSearch->MaxSearchResults = maxSearchResult;
			sessionSearch->bIsLanQuery = false;
			/*sessionSearch->QuerySettings.Set(
				SEARCH_PRESENCE, true,
				EOnlineComparisonOp::Equals
			);*/

			sessions->FindSessions(0, sessionSearch.ToSharedRef());
		}
	}
}

void USteamSessionSubsystem::JoinSession(int32 searchResultIndex)
{
	if (!sessionSearch.IsValid() || !sessionSearch->SearchResults.IsValidIndex(searchResultIndex))
	{
		return;
	}

	if (IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get("Steam"))
	{
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
		if (sessions.IsValid())
		{
			sessions->OnJoinSessionCompleteDelegates.Clear();
			sessions->OnJoinSessionCompleteDelegates.AddUObject(
				this, &USteamSessionSubsystem::OnCompleteJoinSession
			);

			sessions->JoinSession(0, NAME_GameSession, sessionSearch->SearchResults[searchResultIndex]);
		}
	}
}

void USteamSessionSubsystem::OnCompleteCreateSession(FName inSessionName, bool bWasSuccess)
{
	if (bWasSuccess)
	{
		GetWorld()->ServerTravel(inSessionName.ToString() + "?listen");
	}
}

void USteamSessionSubsystem::OnCompleteFindSession(bool bWasSuccess)
{
	if (!bWasSuccess || !sessionSearch.IsValid())
	{
		OnSessionSearchComplete.Broadcast(TArray<FOnlineSessionSearchResult>());
		return;
	}

	OnSessionSearchComplete.Broadcast(sessionSearch->SearchResults);
}

void USteamSessionSubsystem::OnCompleteJoinSession(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		return;
	}

	if (IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get())
	{
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
		if (sessions.IsValid())
		{
			FString connectInfo;
			if (sessions->GetResolvedConnectString(NAME_GameSession, connectInfo))
			{
				APlayerController* PC = GetWorld()->GetFirstPlayerController();
				if (PC)
				{
					PC->ClientTravel(connectInfo, ETravelType::TRAVEL_Absolute);
				}
			}
		}
	}
	
}
