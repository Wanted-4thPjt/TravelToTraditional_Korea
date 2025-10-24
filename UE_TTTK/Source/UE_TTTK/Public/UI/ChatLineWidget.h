// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatLineWidget.generated.h"

/**
 * 
 */
UCLASS()
class UE_TTTK_API UChatLineWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetChatText(const FText& inputText);
	
protected:
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* chatLineText;
	
};
