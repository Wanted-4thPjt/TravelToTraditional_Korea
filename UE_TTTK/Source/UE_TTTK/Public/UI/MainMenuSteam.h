// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuSteam.generated.h"

class USessionsList;
class UCreatingSession;
class UListView;
class UOverlay;
class FOnlineSessionSearch;
class UVerticalBox;
class UButton;
class UCanvasPanel;
/**
 * 
 */
UCLASS()
class UE_TTTK_API UMainMenuSteam : public UUserWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void CreateHost();
	UFUNCTION()
	void OnCompleteCreateSession(FName inSessionName, bool bWasSuccess);
	
	UFUNCTION()
	void ClickFindButton();
	UFUNCTION()
	void OnFindSessionComplete(bool bWasSuccess);
	


protected:
	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* canvas;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* buttonContainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget), Category=Button)
	UButton* hostButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget), Category=Button)
	UButton* findButton;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget), Category=Button)
	UButton* exitButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* creatingSessionOverlay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	UCreatingSession* creatingSession;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	UOverlay* sessionsOverlay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	USessionsList* sessionsList;

private:
	TSharedPtr<FOnlineSessionSearch> sessionSearch;
};
