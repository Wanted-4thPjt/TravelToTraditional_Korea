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
		sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(
			this, &USteamSessionSubsystem::OnCompleteDestroySession);
	}

	hostNamePair.Value = GetSteamNickName();
}

void USteamSessionSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void USteamSessionSubsystem::CreateSession(const FString& mapName, const FString& displayName, int32 maxPlayerCount)
{
	FName subsysName = Online::GetSubsystem(GetWorld())->GetSubsystemName();

	displayNamePair.Value = displayName;
	
	if (sessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("최대 플레이어 수 : %i"), maxPlayerCount);
		
		if (steamMapSettings->mapListAsset)
		{
			FMapInfo mapInfo;
			steamMapSettings->mapListAsset->GetMapInfoByName(mapName, mapInfo);
			mapNamePair.Value = mapInfo.mapAsset.GetAssetName();
			if (maxPlayerCount <= 0)
			{
				if (!mapInfo.mapName.IsEmpty())
				{
					maxPlayerCount = mapInfo.maxPlayers;
				}
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
		sessionSettings.Set<FString>(hostNamePair.Key, hostNamePair.Value, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		sessionSettings.Set<FString>(displayNamePair.Key, displayNamePair.Value, EOnlineDataAdvertisementType::ViaOnlineService);
		sessionSettings.Set<FString>(mapNamePair.Key, mapNamePair.Value, EOnlineDataAdvertisementType::ViaOnlineService);

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

bool USteamSessionSubsystem::DestroySession()
{
	if (!sessionInterface.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("SessionInterface가 유효하지 않음"));
		return false;
	}
	
	if (FNamedOnlineSession* currentSession = sessionInterface->GetNamedSession(NAME_GameSession))
	{
		UE_LOG(LogTemp, Warning, TEXT("세션 파괴 시작: %s"), *currentSession->SessionName.ToString());
		sessionInterface->DestroySession(currentSession->SessionName);
		return true;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("파괴할 세션이 없음"));
	return false;
	
}

FString USteamSessionSubsystem::GetSteamNickName() const
{
	IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld());
	if (!OnlineSub) return TEXT("Unknown");

	IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
	if (!Identity.IsValid()) return TEXT("Unknown");

	FString Nickname = Identity->GetPlayerNickname(0);
	return Nickname.IsEmpty() ? TEXT("Unknown") : Nickname;
}

void USteamSessionSubsystem::OnCompleteCreateSession(FName inSessionName, bool bWasSuccess)
{
	if (bWasSuccess)
	{
		if (mapNamePair.Value.IsEmpty()) {return;}
		FString url = mapNamePair.Value + FString("?listen");
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

void USteamSessionSubsystem::OnCompleteDestroySession(FName SessionName, bool bWasSuccess)
{
	if (bWasSuccess)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 파괴 성공"), *SessionName.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 파괴 실패"), *SessionName.ToString());
	}

	if (GetWorld()->GetNetDriver())
	{
		GetWorld()->GetNetDriver()->Shutdown();
	}
	// TODO: MainMenu로 나가기 || 프로그램 종료로 나누기
	FGenericPlatformMisc::RequestExit(false);
	
}
