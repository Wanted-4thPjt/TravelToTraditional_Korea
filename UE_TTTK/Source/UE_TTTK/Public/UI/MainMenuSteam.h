// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "MainMenuSteam.generated.h"

class USessionsEntry;
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
	void OnSessionsFound(const TArray<FOnlineSessionSearchResult>& SearchResults);
	UFUNCTION()
	void ClickExit();
	

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
	TObjectPtr<UOverlay> creatingSessionOverlay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UCreatingSession> creatingSession;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<UOverlay> sessionsOverlay;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	TObjectPtr<USessionsEntry> sessionsEntry;

};
