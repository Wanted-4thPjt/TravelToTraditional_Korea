// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Network/SessionNode.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Data/MapInfo.h"
#include "Data/PDA_MapList.h"
#include "Network/SteamSessionSettings.h"
#include "UI/Network/SessionNodeData.h"


void USessionNode::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	USessionNodeData* data = Cast<USessionNodeData>(ListItemObject);
	if (data == nullptr) {return;}

	FMapInfo mapInfo;
	if (const USteamSessionSettings* mapSettings = USteamSessionSettings::Get())
	{
		mapSettings->mapListAsset->GetMapInfoByName(data->mapName, mapInfo);
	}
	if (!mapInfo.mapAsset.IsValid()) {return;}

	if (IsValid(sessionName))
	{
		sessionName->SetText(FText::FromString(data->sessionName));
	}
	if (IsValid(mapName))
	{
		mapName->SetText(FText::FromString(data->mapName));
	}
	if (IsValid(hostName))
	{
		hostName->SetText(FText::FromString(data->hostName));
	}
	if (IsValid(mapIcon) && mapInfo.mapIcon.IsValid())
	{
		mapIcon->SetBrushFromTexture(mapInfo.mapIcon.Get());
	}
	if (IsValid(playerCounter))
	{
		FString counterText = FString::Printf(
			TEXT(" %d / %d "),
			data->currentPlayerCount, data->maxPlayerCount
		);
		playerCounter->SetText(FText::FromString(counterText));
	}
	if (IsValid(hostPing))
	{
		FString pingText = FString::Printf(TEXT("%d ms"), data->ping);
		hostPing->SetText(FText::FromString(pingText));
	}
}
