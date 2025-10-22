// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/SteamSessionSubsystem.h"

#include "Network/SteamSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemSteam.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "Data/MapInfo.h"
#include "Data/PDA_MapList.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineSessionInterface.h"

void USteamSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	steamMapSettings = USteamSessionSettings::Get();
	IOnlineSubsystem* subsys = Online::GetSubsystem(GetWorld());
	if (subsys)
	{
		sessionInterface = subsys->GetSessionInterface();
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
			this, &USteamSessionSubsystem::OnCompleteCreateSession);
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
			this, &USteamSessionSubsystem::OnCompleteFindSession);
		sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(
			this, &USteamSessionSubsystem::OnCompleteJoinSession);
	}
}

void USteamSessionSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void USteamSessionSubsystem::CreateSession(const FString& mapName, const FString& displayName, int32 maxPlayerCount)
{
	FName subsysName = Online::GetSubsystem(GetWorld())->GetSubsystemName();
	
	if (sessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("최대 플레이어 수 : %i"), maxPlayerCount);
		FMapInfo mapInfo;
		steamMapSettings->mapListAsset->GetMapInfoByName(mapName, mapInfo);
		if (maxPlayerCount <= 0)
		{
			if (!mapInfo.mapName.IsEmpty())
			{
				maxPlayerCount = mapInfo.maxPlayers;
			}
		}
		
		FOnlineSessionSettings sessionSettings;
		sessionSettings.bIsLANMatch = subsysName.IsEqual(FName(TEXT("NULL")));
		sessionSettings.NumPublicConnections =10;// maxPlayerCount;
		sessionSettings.bShouldAdvertise = true;  // open in Steam friend list
		sessionSettings.bAllowJoinInProgress = true;  // allow participate in progress server
		sessionSettings.bUsesPresence = true;  // for finding friends
		sessionSettings.bAllowJoinViaPresence = true;  // allow friends able to  participate directly
		sessionSettings.bUseLobbiesIfAvailable = true;
		sessionSettings.Set(FName("DP_NAME"), /*mapName*/displayName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		//FName sessionName = FName(mapName + displayNamePrefix + displayName);
		sessionMapAsset = mapName;
		UE_LOG(LogTemp, Warning, TEXT("map path : %s"), *sessionMapAsset);
		FUniqueNetIdPtr netId = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
		sessionInterface->CreateSession(*netId, FName(displayName), sessionSettings);
	}
}

void USteamSessionSubsystem::FindSession()
{
	sessionSearch = MakeShared<FOnlineSessionSearch>();
	if (sessionSearch)
	{
		FName subsysName = Online::GetSubsystem(GetWorld())->GetSubsystemName();
		sessionSearch->bIsLanQuery = subsysName.IsEqual(FName(TEXT("NULL")));
		// 어떤 옵션을 기준으로 검색
		sessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
		//sessionSearch->QuerySettings.Set(FName("DP_NAME"), FName("Wanted"), EOnlineComparisonOp::Equals);
		// 검색 갯수
		sessionSearch->MaxSearchResults = 100;
		// 위 설정을 가지고 세션 검색
		sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
	}
}

void USteamSessionSubsystem::JoinSession(int32 sessionIndex)
{
	TArray<FOnlineSessionSearchResult> results = sessionSearch->SearchResults;
	if (!sessionSearch.IsValid() || !results.IsValidIndex(sessionIndex))
	{
		return;
	}

	results[sessionIndex].Session.SessionSettings.bUseLobbiesIfAvailable = true;
	results[sessionIndex].Session.SessionSettings.bUsesPresence = true;

	FString displayName;
	results[sessionIndex].Session.SessionSettings.Get(FName("DP_NAME"), displayName);

	sessionInterface->JoinSession(0, FName(displayName), results[sessionIndex]);
}

void USteamSessionSubsystem::OnCompleteCreateSession(FName inSessionName, bool bWasSuccess)
{
	if (bWasSuccess)
	{
		if (sessionMapAsset.IsEmpty()) {return;}
		FString url = sessionMapAsset + FString("?listen");
		UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 생성 성공"), *inSessionName.ToString());
		GetWorld()->ServerTravel(url);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 생성 실패"), *inSessionName.ToString());
	}
}

void USteamSessionSubsystem::OnCompleteFindSession(bool bWasSuccess)
{
	UE_LOG(LogTemp, Warning, TEXT("세션 조회 끝"));
	if (bWasSuccess)
	{
		// 검색된 세션 결과들
		auto results = sessionSearch->SearchResults;
		for (int32 i = 0; i < results.Num(); i++)
		{
			// 방 제목 이름 담을 변수
			FString displayName;
			results[i].Session.SessionSettings.Get(FName(TEXT("DP_NAME")), displayName);
			UE_LOG(LogTemp, Warning, TEXT("세션 - %i, 이름 : %s"), i, *displayName);
			
			/*displayName.RemoveFromStart(displayNamePrefix);
			UE_LOG(LogTemp, Warning, TEXT("DisplayName만 남긴 이름 : %s"), *displayName);*/
		}
		OnFindSessions.ExecuteIfBound(results);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("세션 조회 실패"));
	}
}

void USteamSessionSubsystem::OnCompleteJoinSession(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString url;
		sessionInterface->GetResolvedConnectString(SessionName, url);
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *url);
		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		pc->ClientTravel(url, TRAVEL_Absolute);
	}
	
}
