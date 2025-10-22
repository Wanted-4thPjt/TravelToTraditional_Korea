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

	for (int32 i = 0; i < SearchResults.Num(); ++i)
	{
		const FOnlineSessionSearchResult& Result = SearchResults[i];

		USessionNodeData* NodeData = NewObject<USessionNodeData>();
		NodeData->searchResultIndex = i;
		NodeData->currentPlayerCount = Result.Session.SessionSettings.NumPublicConnections - Result.Session.NumOpenPublicConnections;
		NodeData->maxPlayerCount = Result.Session.SessionSettings.NumPublicConnections;
		NodeData->ping = Result.PingInMs;

		// 호스트 이름 (Steam 닉네임 또는 SessionId)
		NodeData->hostName = Result.Session.OwningUserName;
		if (NodeData->hostName.IsEmpty())
		{
			NodeData->hostName = Result.GetSessionIdStr();
		}

		// 맵 이름 (설정에서 가져오기 - 나중에 구현)
		NodeData->mapName = TEXT("Unknown Map");

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
		sessionFindText->SetText(FText::FromString("세션 찾는 중 ..."));
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
