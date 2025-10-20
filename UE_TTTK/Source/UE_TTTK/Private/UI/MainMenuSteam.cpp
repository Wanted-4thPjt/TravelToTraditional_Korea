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
	
	UE_LOG(LogTemp, Warning, TEXT("MainMenuSteam::NativeOnInitialized 호출됨"));
	
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
		creatingSessionOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
	if (sessionsOverlay)
	{
		sessionsOverlay->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainMenuSteam::CreateHost()
{
	UE_LOG(LogTemp, Warning, TEXT("CreateHost 호출됨!"));

	if (!creatingSessionOverlay)
	{
		UE_LOG(LogTemp, Error, TEXT("creatingSessionOverlay가 nullptr입니다!"));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("Overlay 표시 중..."));

	if (creatingSessionOverlay)
	{
		creatingSessionOverlay->SetVisibility(ESlateVisibility::Visible);
		creatingSession->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainMenuSteam::OnCompleteCreateSession(FName inSessionName, bool bWasSuccess)
{
	if (!bWasSuccess) {return;}
	GetWorld()->ServerTravel(inSessionName.ToString() + "?listen");
}

void UMainMenuSteam::ClickFindButton()
{
	UE_LOG(LogTemp, Warning, TEXT("ClickFindButton 호출됨!"));

	if (USteamSessionSubsystem* sessionSubsystem = GetGameInstance()->GetSubsystem<USteamSessionSubsystem>())
	{
		UE_LOG(LogTemp, Display, TEXT("Subsystem 획득 성공, FindSession 호출"));
		sessionSubsystem->FindSession(10);

		if (sessionsOverlay)
		{
			UE_LOG(LogTemp, Display, TEXT("sessionsOverlay 표시"));
			sessionsOverlay->SetVisibility(ESlateVisibility::Visible);
			sessionsEntry->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("sessionsOverlay가 nullptr입니다!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SteamSessionSubsystem을 가져올 수 없습니다!"));
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
	UE_LOG(LogTemp, Warning, TEXT("ClickExit 호출됨!"));

	GetWorld()->EndPlay(EEndPlayReason::Type::Quit);
}
