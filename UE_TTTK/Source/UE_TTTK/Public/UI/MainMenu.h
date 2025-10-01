// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/SlateEnums.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class UEditableTextBox;
class UVerticalBox;
class UButton;
class UCanvasPanel;

/**
 * 
 */
UCLASS()
class UE_TTTK_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void CreateHost();
	
	UFUNCTION()
	void ClickJoinButton();
	UFUNCTION()
	void InputUrl(const FText& changedText);
	UFUNCTION()
	void JoinToUrl(const FText& inText, ETextCommit::Type inCommitMethod);

protected:
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* canvas;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* buttonContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	UButton* hostButton;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	UButton* joinButton;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	UButton* exitButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	UEditableTextBox* urlInput;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString url;
};
