// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SessionsEntry.h"

#include "Components/ListView.h"
#include "Components/Button.h"
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
	if (IsValid(cancelButton))
	{
		cancelButton->OnClicked.AddDynamic(this, &USessionsEntry::OnCancelButtonClicked);
	}
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
	USessionNodeData* SelectedData = Cast<USessionNodeData>(SelectedItem);

	if (SelectedData)
	{
		if (USteamSessionSubsystem* sessionSubsystem = GetGameInstance()->GetSubsystem<USteamSessionSubsystem>())
		{
			sessionSubsystem->JoinSession(SelectedData->searchResultIndex);
		}
	}
}

void USessionsEntry::OnCancelButtonClicked()
{
	GetParent()->SetVisibility(ESlateVisibility::Hidden);
	SetVisibility(ESlateVisibility::Hidden);
}

/*
void USessionsEntry::OnSessionNodeClicked(UObject* Item)
{
	// 클릭 시 선택 표시 (자동으로 처리됨)
}
*/
