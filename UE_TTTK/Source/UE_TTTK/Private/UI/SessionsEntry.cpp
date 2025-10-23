// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SessionsEntry.h"

#include "Components/ListView.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/SessionNodeData.h"
#include "Network/SteamSessionSubsystem.h"
#include "UI/MainMenuSteam.h"

void USessionsEntry::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (IsValid(joinButton))
	{
		joinButton->OnClicked.AddDynamic(this, &USessionsEntry::OnJoinButtonClicked);
	}
	if (IsValid(searchButton))
	{
		searchButton->OnClicked.AddDynamic(this, &USessionsEntry::OnSearchSessionsClicked);
	}
	if (USteamSessionSubsystem* sessionSubsystem = GetGameInstance()->GetSubsystem<USteamSessionSubsystem>())
	{
		sessionSubsystem->OnFindSessions.BindUObject(this, &USessionsEntry::OnSearchSessionsCompleted);
	}
	sessionFindText->SetText(FText::FromString("Search"));
	/*if (IsValid(sessionsListContainer))
	{
		sessionsListContainer->OnItemClicked().AddUObject(this, &USessionsEntry::OnSessionNodeClicked);
	}*/
}

void USessionsEntry::PopulateSessionsList(const TArray<FOnlineSessionSearchResult>& SearchResults)
{
	if (!IsValid(sessionsListContainer))
	{
		return;
	}

	sessionsListContainer->ClearListItems();
	USteamSessionSubsystem* sss = GetGameInstance()->GetSubsystem<USteamSessionSubsystem>();
	for (int32 i = 0; i < SearchResults.Num(); ++i)
	{
		const FOnlineSessionSearchResult& Result = SearchResults[i];

		USessionNodeData* NodeData = NewObject<USessionNodeData>();
		
		NodeData->searchResultIndex = i;
		NodeData->currentPlayerCount = Result.Session.SessionSettings.NumPublicConnections - Result.Session.NumOpenPublicConnections;
		NodeData->maxPlayerCount = Result.Session.SessionSettings.NumPublicConnections;
		NodeData->ping = Result.PingInMs;
		
		FString tempValue;
		Result.Session.SessionSettings.Get(sss->GetHostNameKey(), tempValue);
		NodeData->hostName = tempValue;
		Result.Session.SessionSettings.Get(sss->GetDisplayNameKey(), tempValue);
		NodeData->sessionName = tempValue;
		Result.Session.SessionSettings.Get(sss->GetMapNameKey(), tempValue);
		NodeData->mapName = tempValue;
		sessionsListContainer->AddItem(NodeData);
	}
}

void USessionsEntry::OnJoinButtonClicked()
{
	if (!IsValid(sessionsListContainer))
	{
		return;
	}

	UObject* SelectedItem = sessionsListContainer->GetSelectedItem();

	if (USessionNodeData* SelectedData = Cast<USessionNodeData>(SelectedItem))
	{
		if (USteamSessionSubsystem* sessionSubsystem = GetGameInstance()->GetSubsystem<USteamSessionSubsystem>())
		{
			sessionSubsystem->JoinSession(SelectedData->searchResultIndex);
		}
	}
}


void USessionsEntry::OnSearchSessionsClicked()
{
	if (USteamSessionSubsystem* sessionSubsystem = GetGameInstance()->GetSubsystem<USteamSessionSubsystem>())
	{
		UE_LOG(LogTemp, Display, TEXT("Subsystem 획득 성공, FindSession 호출"));
		sessionsListContainer->ClearListItems();
		sessionSubsystem->FindSession();
		sessionFindText->SetText(FText::FromString("Searching Sessions ..."));
		searchButton->SetIsEnabled(false);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SteamSessionSubsystem을 가져올 수 없습니다!"));
	}
}

void USessionsEntry::OnSearchSessionsCompleted(const TArray<FOnlineSessionSearchResult>& results)
{
	PopulateSessionsList(results);
	sessionFindText->SetText(FText::FromString("Search"));
	searchButton->SetIsEnabled(true);
}

/*
void USessionsEntry::OnSessionNodeClicked(UObject* Item)
{
	// 클릭 시 선택 표시 (자동으로 처리됨)
}
*/
