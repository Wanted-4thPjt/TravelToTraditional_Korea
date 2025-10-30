// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingWidget.generated.h"


class UTTTKUserSettings;
class UWidgetSwitcher;
class UButton;

class UGraphicSettingWidget;
class UControllerSettingWidget;
class UNetworkSettingWidget;
class UInfoSettingWidget;

/**
 * 
 */
UCLASS()
class UE_TTTK_API USettingWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void CloseSettingWindow();

	UFUNCTION()
	void SaveCurrentSettings();
	UFUNCTION()
	void InitializeSettings();

	UFUNCTION()
	void SwitchToGraphic();
	UFUNCTION()
	void SwitchToController();
	UFUNCTION()
	void SwitchToNetwork();
	UFUNCTION()
	void SwitchToInfo();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UTTTKUserSettings* GetUserSettings() {return userSettings;}
	void ExecuteCommand(const FString& command);

private:
	void SetActivateButton(UButton* newActivatedButton);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTTTKUserSettings> userSettings;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> settingsSwitcher;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UButton* currentButton = nullptr;

	UPROPERTY()
	UTexture2D* currentTexture = nullptr;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> graphicButton;
	UPROPERTY(meta=(BindWidget))  //-> 현재 클래스의 OnConstruct에서 저장된 정보 불러오기.
	TObjectPtr<UGraphicSettingWidget> graphicSettingWidget;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> controllerButton;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UControllerSettingWidget> controllerSettingWidget;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> networkButton;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UNetworkSettingWidget> networkSettingWidget;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> infoButton;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInfoSettingWidget> infoSettingWidget;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> closeButton;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> saveButton;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> initializeButton;

	const FLinearColor activeColor = FLinearColor(0.384266f,0.358768f,0.031742f,0.7f);
	const FLinearColor inactiveColor = FLinearColor(0.661458f,0.661458f,0.661458f,0.600000f);
};
