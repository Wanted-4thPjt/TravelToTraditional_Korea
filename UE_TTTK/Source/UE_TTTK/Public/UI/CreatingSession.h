// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreatingSession.generated.h"

class UComboBoxString;
class USpinBox;
class UButton;
class UTextBlock;
class UPDA_MapList;

/**
 * 
 */
UCLASS()
class UE_TTTK_API UCreatingSession : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnCreateButtonClick();
	UFUNCTION()
	void OnCancelButtonClick();
	UFUNCTION()
	void OnSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	
private:
	FString GetSteamNickName() const;
	void InitializeMapSelector();

protected:
	UPROPERTY(meta=(BindWidget))
	UComboBoxString* mapSelector;

	UPROPERTY(meta=(BindWidget))
	USpinBox* maxPlayerSpinBox;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* hostNameDisplay;

	UPROPERTY(meta=(BindWidget))
	UButton* createButton;


	UPROPERTY(meta=(BindWidget))
	UButton* cancelButton;

private:
	UPROPERTY()
	UPDA_MapList* mapList;
};
