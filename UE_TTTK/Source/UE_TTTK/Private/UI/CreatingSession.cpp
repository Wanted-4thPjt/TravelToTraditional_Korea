// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CreatingSession.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Network/SteamSessionSettings.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
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
}

void UCreatingSession::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	if (!IsValid(mapList)) {return;}
	
	InitializeMapSelector();

	if (IsValid(displayNameText))
	{
		displayNameText->OnTextChanged.AddDynamic(this, &UCreatingSession::OnSessionNameInputChanged);
	}
	if (IsValid(createButton))
	{
		createButton->OnClicked.AddDynamic(this, &UCreatingSession::OnCreateButtonClick);
	}
	if (IsValid(hostNameDisplay))
	{
		hostNameDisplay->SetText(FText::FromString(GetSteamNickName()));
	}
	if (IsValid(maxPlayerCounterSlider))
	{
		maxPlayerCounterSlider->OnValueChanged.AddDynamic(this, &UCreatingSession::OnPlayerCounterValueChanged);
		maxPlayerCounterSlider->SetValue(0.f);
		maxPlayerCounterSlider->SetStepSize(1.f);
		if (IsValid(maxPlayerCounterText))
		{
			maxPlayerCounterText->SetText(FText::AsNumber(maxPlayerCounterSlider->GetValue()));
			maxPlayerCounterValue = 0;
		}
	}
	createButton->SetIsEnabled(false);
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
		UE_LOG(LogTemp, Warning, TEXT("Creating name : %s"), *mapInfo.mapName)
		sessionSubsystem->CreateSession(mapInfo.mapName, displayNameText->GetText().ToString(), maxPlayerCounterValue);
	}
}

void UCreatingSession::OnPlayerCounterValueChanged(float inValue)
{
	maxPlayerCounterValue = FMath::RoundToInt(inValue);
	maxPlayerCounterText->SetText(FText::FromString(FString::FromInt(maxPlayerCounterValue)));
	maxPlayerCounterSlider->SetValue(static_cast<float>(maxPlayerCounterValue));
}

void UCreatingSession::OnSessionNameInputChanged(const FText& inputText)
{
	createButton->SetIsEnabled(!inputText.IsEmpty());
}

void UCreatingSession::OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	if (!IsValid(mapList)) {return;}
	
	FMapInfo SelectedMapInfo;
	if (mapList->GetMapInfoByDisplayName(FText::FromString(SelectedItem), SelectedMapInfo))
	{
		maxPlayerCounterSlider->SetMaxValue(static_cast<float>(SelectedMapInfo.maxPlayers));
		maxPlayerCounterSlider->SetMinValue(0.f);
		maxPlayerCounterSlider->SetValue(0.f);
		maxPlayerCounterValue = 0;
		maxPlayerCounterText->SetText(FText::FromString(FString::FromInt(maxPlayerCounterValue)));
	}

}

FString UCreatingSession::GetSteamNickName() const
{
	IOnlineSubsystem* OnlineSub = Online::GetSubsystem(GetWorld());
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
