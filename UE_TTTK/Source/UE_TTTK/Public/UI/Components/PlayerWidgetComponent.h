// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerWidgetComponent.generated.h"

struct FInputActionValue;
class UInputAction;
class UChatLineWidget;
class UChatWidget;
class UPlayerWidget;
class UInputMappingContext;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UPlayerWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerWidgetComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void UpdateChat(const FText& newText);

private:
	void InitPlayerControllerWidget();
	void BindInputMappingContext();

	UFUNCTION()
	void OnInputChatKey(const FInputActionValue& inputActionValue);

	UFUNCTION()
	void OnInputSettingKey(const FInputActionValue& inputActionValue);

	UFUNCTION()
	void OnInputLeaderboardKey(const FInputActionValue& inputActionValue);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input|Context")
	TObjectPtr<UInputMappingContext> IMC_UI;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input|Action")
	TObjectPtr<UInputAction> IA_Chat;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input|Action")
	TObjectPtr<UInputAction> IA_Settings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input|Action")
	TObjectPtr<UInputAction> IA_Leaderboard;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget")
	TSubclassOf<UPlayerWidget> playerWidgetFactory;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget")
	TSubclassOf<UChatLineWidget> chatLineWidgetFactory;
	
	UPROPERTY(EditDefaultsOnly, Category="Widget")
	TObjectPtr<UPlayerWidget> playerWidget;

private:
	UPROPERTY()
	TObjectPtr<APlayerController> ownerPlayerController;

	FInputModeUIOnly uiInputMode;
};
