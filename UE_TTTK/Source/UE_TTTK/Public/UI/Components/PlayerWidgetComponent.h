// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerWidgetComponent.generated.h"

DECLARE_DELEGATE_OneParam(FOnInputChatKey, const FText&)



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

public:
	FOnInputChatKey onInputChatKey;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputMappingContext> IMC_UI;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Input")
	TObjectPtr<UInputAction> IA_Chat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget")
	TSubclassOf<UPlayerWidget> playerWidgetFactory;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Widget")
	TSubclassOf<UChatLineWidget> chatLineWidgetFactory;
	
	UPROPERTY()
	TObjectPtr<UPlayerWidget> playerWidget;
	UPROPERTY()
	TObjectPtr<UChatWidget> chatWidget;

	
};
