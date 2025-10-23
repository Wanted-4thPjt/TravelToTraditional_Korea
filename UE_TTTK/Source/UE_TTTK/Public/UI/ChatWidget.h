// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

class USizeBox;
class UEditableTextBox;
class UScrollBox;
/**
 * 
 */
UCLASS()
class UE_TTTK_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnFocusChanging(const FWeakWidgetPath& PreviousFocusPath, const FWidgetPath& NewWidgetPath, const FFocusEvent& InFocusEvent) override;

	UFUNCTION()
	void OnChatCommitted(const FText& inputText, ETextCommit::Type commitType);

	
public:
	UPROPERTY(meta=(BindWidget))
	USizeBox* chatSizeBox;
	UPROPERTY(meta=(BindWidget))
	UScrollBox* chatScrollBox;
	UPROPERTY(meta=(BindWidget))
	UEditableTextBox* inputChatBox;
};
