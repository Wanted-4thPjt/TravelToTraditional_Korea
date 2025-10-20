// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MainMenuSteam.h"

#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Online/OnlineSessionNames.h"
#include "UI/CreatingSession.h"
#include "UI/SessionsEntry.h"
#include "Network/SteamSessionSubsystem.h"

void UMainMenuSteam::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UMainMenuSteam::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (IsValid(hostButton))
	{
		hostButton->OnClicked.AddDynamic(this, &UMainMenuSteam::CreateHost);
	}
	if (IsValid(findButton))
	{
		findButton->OnClicked.AddDynamic(this, &UMainMenuSteam::ClickFindButton);
	}
	if (IsValid(exitButton))
	{
		exitButton->OnClicked.AddDynamic(this, &UMainMenuSteam::ClickExit);
	}

	// Subsystem 델리게이트 바인딩
	if (USteamSessionSubsystem* sessionSubsystem = GetGameInstance()->GetSubsystem<USteamSessionSubsystem>())
	{
		sessionSubsystem->OnSessionSearchComplete.Clear();
		sessionSubsystem->OnSessionSearchComplete.AddUObject(this, &UMainMenuSteam::OnSessionsFound);
	}

	// 초기 상태 설정
	if (creatingSessionOverlay)
	{
		creatingSessionOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (sessionsOverlay)
	{
		sessionsOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainMenuSteam::CreateHost()
{
	if (creatingSessionOverlay)
	{
		creatingSessionOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainMenuSteam::OnCompleteCreateSession(FName inSessionName, bool bWasSuccess)
{
	if (!bWasSuccess) {return;}
	GetWorld()->ServerTravel(inSessionName.ToString() + "?listen");
}

void UMainMenuSteam::ClickFindButton()
{
	if (USteamSessionSubsystem* sessionSubsystem = GetGameInstance()->GetSubsystem<USteamSessionSubsystem>())
	{
		sessionSubsystem->FindSession(10);

		// UI 전환
		if (sessionsOverlay)
		{
			sessionsOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UMainMenuSteam::OnSessionsFound(const TArray<FOnlineSessionSearchResult>& SearchResults)
{
	if (IsValid(sessionsEntry))
	{
		sessionsEntry->PopulateSessionsList(SearchResults);
	}
}

void UMainMenuSteam::ClickExit()
{
	GetWorld()->EndPlay(EEndPlayReason::Type::Quit);
}
