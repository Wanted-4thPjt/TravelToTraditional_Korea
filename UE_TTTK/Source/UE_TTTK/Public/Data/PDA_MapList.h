// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PDA_MapList.generated.h"

struct FMapInfo;

/**
 * 
 */
UCLASS()
class UE_TTTK_API UPDA_MapList : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Maps")
	TArray<FMapInfo> availableMaps;

	UFUNCTION(BlueprintCallable, Category = "Maps")
	bool GetMapInfoByName(const FString& inMapName, FMapInfo& outMapinfo);
	UFUNCTION(BlueprintCallable, Category = "Maps")
	bool GetMapInfoByDisplayName(const FText& inDisplayMapName, FMapInfo& outMapinfo);

	UFUNCTION(BlueprintCallable, Category = "Maps")
	bool GetMapObjectPathByName(const FString& inMapName, FString& outPath);
	UFUNCTION(BlueprintCallable, Category = "Maps")
	bool GetMapObjectPathByDisplayName(const FText& inDisplayMapName, FString& outPath);

	// Have to override for Inherits PDA
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
