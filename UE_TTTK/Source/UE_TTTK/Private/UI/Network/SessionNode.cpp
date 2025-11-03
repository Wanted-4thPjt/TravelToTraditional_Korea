// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Network/SessionNode.h"

#include "OnlineSubsystem.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Data/MapInfo.h"
#include "Data/PDA_MapList.h"
#include "Network/SteamSessionSettings.h"
#include "UI/Network/SessionNodeData.h"


void USessionNode::RefreshSessionParticipantsCount(const int32 newCount)
{
	currentPlayerCounter->SetText(FText::AsNumber(newCount));
}

void USessionNode::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	InitializeItem(ListItemObject);
}

void USessionNode::InitializeItem(UObject* ListItemObject) const
{
	USessionNodeData* data = Cast<USessionNodeData>(ListItemObject);
	if (data == nullptr)
	{
		UE_LOG(LogOnline, Error, TEXT("Can not Cast to Node Data Obj"));
		return;
	}

	FMapInfo mapInfo;
	const USteamSessionSettings* mapSettings = USteamSessionSettings::Get();
	if (mapSettings)
	{
		mapSettings->mapListAsset->GetMapInfoByName(data->mapName, mapInfo);
	}
	
	sessionName->SetText(FText::FromString(data->sessionName));
	mapName->SetText(mapInfo.displayName);
	hostName->SetText(FText::FromString(data->hostName));
	mapIcon->SetBrushFromTexture(mapInfo.mapIcon.Get());
	FString counterText = FString::Printf(
			TEXT(" / %d "),
			data->maxPlayerCount
		);
	maxPlayerCounter->SetText(FText::FromString(counterText));
	currentPlayerCounter->SetText(FText::AsNumber(data->currentPlayerCount));
	hostPing->SetText(FText::AsNumber(data->ping));
	if (data->ping > 100) {hostPing->SetColorAndOpacity(FColor::Red);}
	else if (data->ping > 60) {hostPing->SetColorAndOpacity(FColor::Orange);}
	else if (data->ping > 30) {hostPing->SetColorAndOpacity(FColor::White);}
	else {hostPing->SetColorAndOpacity(FColor::Green);}
	

	UE_LOG(LogOnline, Warning, TEXT("\n"
				"=========== Item ===========\n"
				"   Host : %s / Ping : %i\n   Session : %s\n   Map : %s\n"
				"============================"
			),
			*data->hostName, data->ping, *data->sessionName, *mapInfo.displayName.ToString()
		)
}
