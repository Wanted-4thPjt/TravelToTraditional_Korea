// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InContentMenuWidget.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UInContentMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	continueButton->OnClicked.AddDynamic(this, &UInContentMenuWidget::OnClickedContinue);
	exitButton->OnClicked.AddDynamic(this, &UInContentMenuWidget::OnClickedExit);
}

void UInContentMenuWidget::OnClickedContinue()
{
	RemoveFromParent();
	GetOwningPlayer()->SetShowMouseCursor(false);
	GetOwningPlayer()->SetInputMode(FInputModeGameOnly());
}

void UInContentMenuWidget::OnClickedExit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, false);
}
