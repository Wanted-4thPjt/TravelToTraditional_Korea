// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

class USizeBox;
class UChatWidget;
class UTextBlock;
/**
 * 
 */
UCLASS()
class UE_TTTK_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

	
protected:
	virtual void NativeConstruct() override;

public:
	FORCEINLINE void EnableShowFPS(const bool& enableShowFPS) {bShowFPS = enableShowFPS;}

protected:
	UFUNCTION()
	float GetCurrentFPS();
	
public:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UChatWidget> chatWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> fpsText;

private:
	bool bShowFPS = false;
};
