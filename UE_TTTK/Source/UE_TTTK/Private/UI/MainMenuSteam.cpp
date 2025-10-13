// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuSteam.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Components/Button.h"
#include "Online/OnlineSessionNames.h"
#include "UI/CreatingSession.h"
#include "UI/SessionsList.h"

void UMainMenuSteam::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(hostButton))
	{
		hostButton->OnClicked.AddDynamic(this, &UMainMenuSteam::CreateHost);
	}
	creatingSession = Cast<UCreatingSession>(CreateWidget(this, UCreatingSession::StaticClass()));
	creatingSession->SetVisibility(ESlateVisibility::Hidden);
	sessionsList = Cast<USessionsList>(CreateWidget(this, USessionsList::StaticClass()));
	sessionsList->SetVisibility(ESlateVisibility::Hidden);
}

void UMainMenuSteam::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UMainMenuSteam::CreateHost()
{
	creatingSession->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenuSteam::OnCompleteCreateSession(FName inSessionName, bool bWasSuccess)
{
	
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
				SEARCH_PRESENCE, true,
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
