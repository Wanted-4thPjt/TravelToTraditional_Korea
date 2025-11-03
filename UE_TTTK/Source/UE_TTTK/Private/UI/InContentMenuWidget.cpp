// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InContentMenuWidget.h"

#include "Kismet/KismetSystemLibrary.h"

void UInContentMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
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
