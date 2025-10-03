// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/SessionNode.h"
#include "Components/TextBlock.h"
#include "Components/GridPanel.h"
#include "Components/Image.h"
#include "UI/SessionNodeData.h"



void USessionNode::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	USessionNodeData* data = Cast<USessionNodeData>(ListItemObject);
	if (data == nullptr) {return;}

	if (IsValid(hostName))
	{
		hostName->SetText(FText::FromString(data->hostName));
	}
	if (IsValid(mapIcon) && data->mapIcon.IsValid())
	{
		mapIcon->SetBrushFromTexture(data->mapIcon.Get());
	}
	if (IsValid(mapName))
	{
		mapName->SetText(FText::FromString(data->mapName));
	}
	if (IsValid(playerCounter))
	{
		FString counterText = FString::Printf(TEXT("%d / %d"),
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
