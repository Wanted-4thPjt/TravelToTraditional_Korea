// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"
#include "MainMenuSteam.generated.h"

class UImage;
class UWidgetSwitcher;
class USessionsEntry;
class UCreatingSession;
class UListView;
class FOnlineSessionSearch;
class UVerticalBox;
class UButton;
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
	void ClickLobbyButton();
	UFUNCTION()
	void ClickHostButton();
	
	UFUNCTION()
	void ClickFindButton();
	UFUNCTION()
	void ClickExit();
	

protected:
	UPROPERTY(meta=(BindWidget))
	UWidgetSwitcher* lobbySwitcher;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget), Category=MainWidget)
	UButton* goLobbyButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget), Category=MainWidget)
	UButton* goHostButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget), Category=MainWidget)
	UButton* goFindButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget), Category=MainWidget)
	UButton* exitButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget), Category=MainWidget)
	UImage* backgroundImage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget), Category=HostWidget)
	TObjectPtr<UCreatingSession> creatingSession;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget), Category=FindWidget)
	TObjectPtr<USessionsEntry> sessionsEntry;

};
