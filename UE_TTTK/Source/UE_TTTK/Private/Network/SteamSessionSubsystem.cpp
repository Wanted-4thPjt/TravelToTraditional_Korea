// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/SteamSessionSubsystem.h"

#include "Network/SteamSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemSteam.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"
#include "UI/Network/LoadingWidget.h"
#include "Data/MapInfo.h"
#include "Data/PDA_MapList.h"
#include "Data/TTTKUserSettings.h"
#include "Engine/AssetManager.h"
#include "Online/OnlineSessionNames.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StreamableManager.h"
#include "Utility/Base64Converter.h"

USteamSessionSubsystem::USteamSessionSubsystem()
{
	ConstructorHelpers::FClassFinder<ULoadingWidget> loadingWidgetFinder(TEXT("/Game/UI/MainMenu/WBP_Loading.WBP_Loading_C"));
	if (loadingWidgetFinder.Succeeded())
	{
		loadingWidgetFactory = loadingWidgetFinder.Class;
	}
}

void USteamSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	steamMapSettings = USteamSessionSettings::Get();
	IOnlineSubsystem* subsys = Online::GetSubsystem(GetWorld());
	if (!subsys) {return;}

	sessionInterface = subsys->GetSessionInterface();
	sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
		this, &USteamSessionSubsystem::OnCompleteCreateSession);
	sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
		this, &USteamSessionSubsystem::OnCompleteFindSession);
	sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(
		this, &USteamSessionSubsystem::OnCompleteJoinSession);
	sessionInterface->OnDestroySessionCompleteDelegates.AddUObject(
		this, &USteamSessionSubsystem::OnCompleteDestroySession);
	sessionInterface->OnUpdateSessionCompleteDelegates.AddUObject(
		this, &USteamSessionSubsystem::OnCompleteUpdateSession);

	hostNamePair.Value = GetSteamNickName();

	
	//FStreamableManager& LoadingManager = UAssetManager::Get().GetStreamableManager();

	BindCloudDelegates(subsys);
	loadingWidget = CreateWidget<ULoadingWidget>(GetOuterUGameInstance(), loadingWidgetFactory);
}

void USteamSessionSubsystem::Deinitialize()
{
	IOnlineSubsystem* onlineSub = Online::GetSubsystem(GetWorld());
	if (onlineSub)
	{
		IOnlineUserCloudPtr userCloud = onlineSub->GetUserCloudInterface();
		if (userCloud.IsValid())
		{
			if (userCloud->OnReadUserFileCompleteDelegates.IsBoundToObject(this))
			{
				userCloud->OnReadUserFileCompleteDelegates.Clear();
			}
			if (userCloud->OnWriteUserFileCompleteDelegates.IsBoundToObject(this))
			{
				userCloud->OnWriteUserFileCompleteDelegates.Clear();
			}
		}
	}
	
	UGameUserSettings* settings = UGameUserSettings::GetGameUserSettings();
	if (settings)
	{
		settings->OnUpdateCloudDataFromGameUserSettings.Unbind();
		settings->OnUpdateGameUserSettingsFileFromCloud.Unbind();
	}
	
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
			if (loadingWidget)
			{
				loadingWidget->AddToViewport();
				GetOuterUGameInstance()->GetTimerManager().SetTimer(loadingWidgetTimer,
					[&]()->void
					{
						loadingWidget->RemoveFromParent();
					}, mapInfo.desiredLoadingTime, false);
			}
		}
		
		sessionSettings.bIsLANMatch = subsysName.IsEqual(FName(TEXT("NULL")));
		sessionSettings.NumPublicConnections = maxPlayerCount;
		sessionSettings.bShouldAdvertise = true;  // open in Steam friend list
		sessionSettings.bAllowJoinInProgress = true;  // allow participate in progress server
		sessionSettings.bUsesPresence = true;  // for finding friends
		sessionSettings.bAllowJoinViaPresence = true;  // allow friends able to  participate directly
		sessionSettings.bUseLobbiesIfAvailable = true;
		sessionSettings.Set<FString>(hostNamePair.Key, UBase64Converter::StringConvertToBase64(hostNamePair.Value), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		sessionSettings.Set<FString>(displayNamePair.Key, UBase64Converter::StringConvertToBase64(displayNamePair.Value), EOnlineDataAdvertisementType::ViaOnlineService);
		sessionSettings.Set<FString>(mapNamePair.Key, UBase64Converter::StringConvertToBase64(mapNamePair.Value), EOnlineDataAdvertisementType::ViaOnlineService);
		sessionSettings.Set<int32>(participantsCountPair.Key, participantsCountPair.Value, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

		FUniqueNetIdPtr netId = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
		sessionInterface->CreateSession(*netId, FName(displayNamePair.Value), sessionSettings);
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
	results[sessionIndex].Session.SessionSettings.Get(displayNamePair.Key, displayName);
	results[sessionIndex].Session.SessionSettings.Get(mapNamePair.Key, mapNamePair.Value);
	if (loadingWidget)
	{
		loadingWidget->AddToViewport();
		
		if (steamMapSettings->mapListAsset)
		{
			FMapInfo mapInfo;
			steamMapSettings->mapListAsset->GetMapInfoByName(mapNamePair.Value, mapInfo);
			
			GetOuterUGameInstance()->GetTimerManager().SetTimer(loadingWidgetTimer,
				[&]()->void
				{
					loadingWidget->RemoveFromParent();
				}, mapInfo.desiredLoadingTime, false
			);
		}
	}
	sessionInterface->JoinSession(0, FName(displayName), results[sessionIndex]);
}

void USteamSessionSubsystem::OpenSessionEntry()
{
	if (!sessionInterface.IsValid()) {return;}
	if (displayNamePair.Value.Len() == 0) {return;}
	
	sessionSettings.bShouldAdvertise = true;
	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.bUsesPresence = true;
	
	sessionInterface->UpdateSession(FName(displayNamePair.Value), sessionSettings);
}

void USteamSessionSubsystem::CloseSessionEntry()
{
	if (!sessionInterface.IsValid()) {return;}
	if (displayNamePair.Value.Len() == 0) {return;}
	
	sessionSettings.bShouldAdvertise = false;
	sessionSettings.bAllowJoinInProgress = false;
	sessionSettings.bUsesPresence = false;
	
	sessionInterface->UpdateSession(FName(displayNamePair.Value), sessionSettings);
}

void USteamSessionSubsystem::RefreshSessionInfo(const int32& changedParticipantsCount)
{
	if (!sessionInterface.IsValid()) {return;}
	if (displayNamePair.Value.Len() == 0) {return;}
	
	sessionSettings.Get(participantsCountPair.Key, participantsCountPair.Value);
	sessionSettings.Set(participantsCountPair.Key, participantsCountPair.Value + changedParticipantsCount);
	sessionInterface->UpdateSession(FName(displayNamePair.Value), sessionSettings);
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
	if (!OnlineSub->GetSubsystemName().IsEqual("STEAM"))
	{
		Nickname = TEXT("Local_") + Nickname.Left(5);
	}
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
		if (loadingWidget) {loadingWidget->RemoveFromParent();}
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
		/*for (int32 i = 0; i < results.Num(); i++)
		{
			// 방 제목 이름 담을 변수
			FString displayName;
			results[i].Session.SessionSettings.Get(FName(TEXT("DP_NAME")), displayName);
			UE_LOG(LogTemp, Warning, TEXT("세션 - %i, 이름 : %s"), i, *displayName);
			
			/*displayName.RemoveFromStart(displayNamePrefix);
			UE_LOG(LogTemp, Warning, TEXT("DisplayName만 남긴 이름 : %s"), *displayName);#1#
		}*/
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
		return;
	}

	if (loadingWidget) {loadingWidget->RemoveFromParent();}
	mapNamePair.Value.Empty();
}

void USteamSessionSubsystem::OnCompleteUpdateSession(FName SessionName, bool bWasSuccess)
{
	if (!bWasSuccess) {return;}
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

void USteamSessionSubsystem::BindCloudDelegates(IOnlineSubsystem* onlineSubsys)
{
	IOnlineUserCloudPtr userCloud = onlineSubsys->GetUserCloudInterface();
	if (!userCloud.IsValid()) {return;}

	UTTTKUserSettings* settings = UTTTKUserSettings::Get();
	if (!IsValid(settings)) {return;}

	settings->OnUpdateGameUserSettingsFileFromCloud.BindUObject(
		this, &USteamSessionSubsystem::LoadUserSettingsFromCloud);
	settings->OnUpdateCloudDataFromGameUserSettings.BindUObject(
		this, &USteamSessionSubsystem::SaveUserSettingsToCloud);
	userCloud->OnReadUserFileCompleteDelegates.AddUObject(this, &USteamSessionSubsystem::OnCompleteReadUserCloudData);
}

void USteamSessionSubsystem::OnCompleteReadUserCloudData(bool bWasSuccess, const FUniqueNetId& netId,
                                                         const FString& FileName)
{
	UGameUserSettings* settings = UGameUserSettings::GetGameUserSettings();
	if (!IsValid(settings)) {return;}

	if (bWasSuccess)
	{
		IOnlineSubsystem* subsys = Online::GetSubsystem(GetWorld());
		if (!subsys) return;
		IOnlineUserCloudPtr userCloud = subsys->GetUserCloudInterface();
		if (!userCloud.IsValid()) {return;}
		TArray<uint8> fileData;
		if (userCloud->GetFileContents(netId, FileName, fileData))
		{
			FString filePath = FPaths::GeneratedConfigDir() + UGameplayStatics::GetPlatformName() + "/" +  GGameUserSettingsIni + ".ini";
			if (!FPaths::DirectoryExists(FPaths::GetPath(filePath)))
			{
				IFileManager::Get().MakeDirectory(*FPaths::GetPath(filePath), true);
			}
			if (!FFileHelper::SaveArrayToFile(fileData, *filePath))
			{
				settings->LoadSettings(false);
				settings->ApplySettings(false);
				return;
			}
		}
	}
	
	settings->LoadSettings(bWasSuccess);
	settings->ApplySettings(bWasSuccess);
	
	
}

bool USteamSessionSubsystem::LoadUserSettingsFromCloud(const FString& FilePath)
{
	IOnlineSubsystem* subsys = Online::GetSubsystem(GetWorld());
	if (!subsys || subsys->GetSubsystemName() != FName(TEXT("STEAM"))) {return false;}

	IOnlineUserCloudPtr userCloud = subsys->GetUserCloudInterface();
	if (!userCloud.IsValid()) {return false;}

	IOnlineIdentityPtr identity =  subsys->GetIdentityInterface();
	if (!identity.IsValid()) {return false;}

	FUniqueNetIdPtr netId = identity->GetUniquePlayerId(0);
	if (!netId.IsValid()) {return false;}

	if (!userCloud->ReadUserFile(*netId.Get(), *GGameUserSettingsIni)) {return false;}

	return true;
}

bool USteamSessionSubsystem::SaveUserSettingsToCloud(const FString& FilePath)
{	
	IOnlineSubsystem* subsys = Online::GetSubsystem(GetWorld());
	if (!subsys) {return false;}

	IOnlineUserCloudPtr userCloud = subsys->GetUserCloudInterface();
	if (!userCloud.IsValid()) {return false;}

	IOnlineIdentityPtr identity =  subsys->GetIdentityInterface();
	if (!identity.IsValid()) {return false;}

	FUniqueNetIdPtr netId = identity->GetUniquePlayerId(0);
	if (!netId.IsValid()) {return false;}
	
	TArray<uint8> fileData;
	if (!FFileHelper::LoadFileToArray(fileData, *FilePath)) {return false;}

	return userCloud->WriteUserFile(*netId.Get(), *GGameUserSettingsIni, fileData);
}
