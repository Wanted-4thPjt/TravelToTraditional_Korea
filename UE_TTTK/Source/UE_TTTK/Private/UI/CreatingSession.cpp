// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CreatingSession.h"

#include "OnlineSubsystem.h"
#include "Network/SteamSessionSettings.h"
#include "Components/Button.h"
#include "Components/SpinBox.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "Data/MapInfo.h"
#include "Data/PDA_MapList.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Network/SteamSessionSubsystem.h"

void UCreatingSession::NativeConstruct()
{
	Super::NativeConstruct();
	
	const USteamSessionSettings* settings = USteamSessionSettings::Get();
	if (!settings || !settings->mapListAsset.IsValid()) {return;}
	
	mapList = settings->mapListAsset.LoadSynchronous();
	if (!IsValid(mapList)) {return;}
	
	InitializeMapSelector();
	
	if (IsValid(createButton))
	{
		createButton->OnClicked.AddDynamic(this, &UCreatingSession::OnCreateButtonClick);
	}
	if (IsValid(cancelButton))
	{
		cancelButton->OnClicked.AddDynamic(this, &UCreatingSession::OnCancelButtonClick);
	}
	if (IsValid(hostNameDisplay))
	{
		hostNameDisplay->SetText(FText::FromString(GetSteamNickName()));
	}
	if (IsValid(maxPlayerSpinBox))
	{
		maxPlayerSpinBox->SetDelta(1.f);
	}
}

void UCreatingSession::OnCreateButtonClick()
{
	if (!IsValid(mapList) || !IsValid(mapSelector)) {return;}

	FString selectedDisplayName = mapSelector->GetSelectedOption();
	FMapInfo mapInfo;
	if (!mapList->GetMapInfoByDisplayName(FText::FromString(selectedDisplayName), mapInfo)) {return;}
	
	USteamSessionSubsystem* sessionSubsystem = GetGameInstance()->GetSubsystem<USteamSessionSubsystem>();
	if (IsValid(sessionSubsystem))
	{
		sessionSubsystem->CreateSession(FName(*mapInfo.mapName), static_cast<int32>(maxPlayerSpinBox->GetValue()));
	}
	
	RemoveFromParent();
}

void UCreatingSession::OnCancelButtonClick()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UCreatingSession::OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (!IsValid(mapList) || !IsValid(maxPlayerSpinBox)) {return;}
	
	FMapInfo SelectedMapInfo;
	if (mapList->GetMapInfoByDisplayName(FText::FromString(SelectedItem), SelectedMapInfo))
	{
		maxPlayerSpinBox->SetMaxValue(static_cast<float>(SelectedMapInfo.maxPlayers));
		maxPlayerSpinBox->SetMinValue(1.f);

		if (maxPlayerSpinBox->GetValue() > SelectedMapInfo.maxPlayers)
		{
			maxPlayerSpinBox->SetValue(static_cast<float>(SelectedMapInfo.maxPlayers));
		}
	}

}


FString UCreatingSession::GetSteamNickName() const
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (!OnlineSub) return TEXT("Unknown");

	IOnlineIdentityPtr Identity = OnlineSub->GetIdentityInterface();
	if (!Identity.IsValid()) return TEXT("Unknown");

	FString Nickname = Identity->GetPlayerNickname(0);

	return Nickname.IsEmpty() ? TEXT("Unknown") : Nickname;
}

void UCreatingSession::InitializeMapSelector()
{
	if (!IsValid(mapList) || !IsValid(mapSelector)) {return;}

	mapSelector->ClearOptions();
	for (const FMapInfo& mapInfo : mapList->availableMaps)
	{
		mapSelector->AddOption(mapInfo.displayName.ToString());
	}
	if (mapList->availableMaps.Num() > 0)
	{
		mapSelector->SetSelectedIndex(0);
		OnSelectionChanged(mapSelector->GetSelectedOption(), ESelectInfo::Direct);
	}


	mapSelector->OnSelectionChanged.AddDynamic(this, &UCreatingSession::OnSelectionChanged);
}
