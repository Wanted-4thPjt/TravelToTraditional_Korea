// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingWidget.generated.h"

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
	
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	bool bInEditable = false;
};
