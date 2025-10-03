// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionsList.generated.h"

class UButton;
class UListView;
/**
 * 
 */
UCLASS()
class UE_TTTK_API USessionsList : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(BindWidget))
	UListView* sessionsListContainer;
	
	UPROPERTY(meta=(BindWidget))
	UButton* joinButton;
	UPROPERTY(meta=(BindWidget))
	UButton* cancelButton;
};
