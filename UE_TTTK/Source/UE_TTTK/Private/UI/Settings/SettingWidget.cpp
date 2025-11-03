// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Settings/SettingWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Data/TTTKUserSettings.h"

#include "UI/Settings/ControllerSettingWidget.h"
#include "UI/Settings/GraphicSettingWidget.h"
#include "UI/Settings/InfoSettingWidget.h"
#include "UI/Settings/NetworkSettingWidget.h"

void USettingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bool currentEditableState = GetWorld()->GetFirstPlayerController()->GetNetMode() == NM_Standalone;

	closeButton->OnClicked.AddDynamic(this, &USettingWidget::CloseSettingWindow);
	
	saveButton->OnClicked.AddDynamic(this, &USettingWidget::SaveCurrentSettings);
	initializeButton->OnClicked.AddDynamic(this, &USettingWidget::InitializeSettings);

	graphicButton->OnClicked.AddDynamic(this, &USettingWidget::SwitchToGraphic);
	graphicSettingWidget->bEditable = currentEditableState;
	controllerButton->OnClicked.AddDynamic(this, &USettingWidget::SwitchToController);
	controllerSettingWidget->bEditable = currentEditableState;
	networkButton->OnClicked.AddDynamic(this, &USettingWidget::SwitchToNetwork);
	networkSettingWidget->bEditable = currentEditableState;
	infoButton->OnClicked.AddDynamic(this, &USettingWidget::SwitchToInfo);
	infoSettingWidget->bEditable = currentEditableState;

	SwitchToGraphic();
}

void USettingWidget::CloseSettingWindow()
{
	RemoveFromParent();
}

void USettingWidget::SaveCurrentSettings()
{
	OnSaveSettings.Broadcast();
	
	// TODO : 하위 Setting Widget들에서 정보 다 가져와서 저장.
	graphicSettingWidget;	
	controllerSettingWidget;
	networkSettingWidget;
	infoSettingWidget;	
}

void USettingWidget::InitializeSettings()
{
	// TODO: 하위 Setting Widget들의 정보들 다 기본값으로 바꾸기 (저장X).
	// 기본값 : 각 Setting Widget이 별도로 가지고 있기? 아니면 저장파일에서 저장해두기?
}

void USettingWidget::SwitchToGraphic()
{
	if (currentButton == graphicButton) {return;}
	settingsSwitcher->SetActiveWidget(graphicSettingWidget);
	SetActivateButton(graphicButton);
}

void USettingWidget::SwitchToController()
{
	if (currentButton == controllerButton) {return;}
	settingsSwitcher->SetActiveWidget(controllerSettingWidget);
	SetActivateButton(controllerButton);
}

void USettingWidget::SwitchToNetwork()
{
	if (currentButton == networkButton) {return;}
	settingsSwitcher->SetActiveWidget(networkSettingWidget);
	SetActivateButton(networkButton);
}

void USettingWidget::SwitchToInfo()
{
	if (currentButton == infoButton) {return;}
	settingsSwitcher->SetActiveWidget(infoSettingWidget);
	SetActivateButton(infoButton);
}

void USettingWidget::ExecuteCommand(const FString& command)
{
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		pc->ConsoleCommand(command);
	}
}

void USettingWidget::SetActivateButton(UButton* newActivatedButton)
{
	currentButton->SetBackgroundColor(inactiveColor);
	currentButton = newActivatedButton;
	currentButton->SetBackgroundColor(activeColor);
}

