// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ChatWidget.h"

#include "UE_TTTKPlayerController.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/SizeBox.h"

void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	inputChatBox->OnTextCommitted.AddDynamic(this, &UChatWidget::OnChatCommitted);
}

void UChatWidget::OnChatCommitted(const FText& inputText, ETextCommit::Type commitType)
{
	AUE_TTTKPlayerController* pc = Cast<AUE_TTTKPlayerController>(GetOwningLocalPlayer()->GetPlayerController(GetWorld()));
	
	
}

void UChatWidget::NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath,
	const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusChanging(PreviousFocusPath, NewWidgetPath, InFocusEvent);

	switch (InFocusEvent.GetCause())
	{
		case EFocusCause::SetDirectly:
			{
				chatSizeBox->SetHeightOverride(300.f);
				chatScrollBox->ScrollToEnd();
			}
			break;
		case EFocusCause::Cleared:
			{
				chatSizeBox->SetHeightOverride(800.f);
			}
			break;
	}
}
