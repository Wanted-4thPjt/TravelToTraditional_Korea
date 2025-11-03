// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "SessionsEntry.generated.h"

class UTextBlock;
class UOverlay;
class USessionNode;
class UButton;
class UListView;
/**
 *
 */
UCLASS()
class UE_TTTK_API USessionsEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	void PopulateSessionsList(const TArray<FOnlineSessionSearchResult>& SearchResults);
	void ResetSessionsList();
	
private:
	UFUNCTION()
	void OnJoinButtonClicked();

	UFUNCTION()
	void OnSearchSessionsClicked();
	void OnSearchSessionsCompleted(const TArray<FOnlineSessionSearchResult>& results);
	/*UFUNCTION()
	void OnSessionNodeClicked(UObject* Item);*/
	UFUNCTION()
	void RefreshList();

protected:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	UListView* sessionsListContainer;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	UButton* searchButton;
	UPROPERTY(meta=(BindWidget))
	UButton* joinButton;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* sessionFindText;

private:
	FTimerHandle refreshListTimer;
};
