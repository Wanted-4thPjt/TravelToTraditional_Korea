// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ChatLineWidget.h"

#include "Components/TextBlock.h"

void UChatLineWidget::SetChatText(const FText& inputText)
{
	chatLineText->SetText(inputText);
}
