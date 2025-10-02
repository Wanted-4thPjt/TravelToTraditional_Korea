// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuSteam.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"

void UMainMenuSteam::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainMenuSteam::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMainMenuSteam::CreateHost()
{
	if (IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get())
	{
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
		if (sessions.IsValid())
		{
			sessions->OnCreateSessionCompleteDelegates.AddUObject(
				this, &UMainMenuSteam::OnCompleteCreateSession);

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

			sessions->CreateSession(0, FName("TTTK_Session"), sessionSettings);
		}
	}
}

void UMainMenuSteam::OnCompleteCreateSession(FName inSessionName, bool bWasSuccess)
{
	if (bWasSuccess)
	{
		GetWorld()->ServerTravel(mapPath + "?listen");
	}
}

void UMainMenuSteam::ClickFindButton()
{
	if (IOnlineSubsystem* onlineSub = IOnlineSubsystem::Get())
	{
		IOnlineSessionPtr sessions = onlineSub->GetSessionInterface();
		if (sessions.IsValid())
		{
			sessions->OnFindSessionsCompleteDelegates.AddUObject(
				this, &UMainMenuSteam::OnFindSessionComplete
			);
			
			sessionSearch = MakeShareable(new FOnlineSessionSearch());
			sessionSearch->MaxSearchResults = 10;
			sessionSearch->bIsLanQuery = false;
			sessionSearch->QuerySettings.Set(
				FName(TEXT("PRESENCE")), true,
				EOnlineComparisonOp::Equals
			);
			sessions->FindSessions(0, sessionSearch.ToSharedRef());
		}
	}
}

void UMainMenuSteam::OnFindSessionComplete(bool bWasSuccess)
{
	if (!bWasSuccess) {return;}
	//const TArray<FOnlineSession>& results = ser
}

void UMainMenuSteam::ClickJoinButton()
{
}

void UMainMenuSteam::InputUrl(const FText& changedText)
{
}

void UMainMenuSteam::JoinToUrl(const FText& inText, ETextCommit::Type inCommitMethod)
{
}