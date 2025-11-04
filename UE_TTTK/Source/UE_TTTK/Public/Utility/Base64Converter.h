// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Base64Converter.generated.h"

/**
 * 
 */
UCLASS()
class UE_TTTK_API UBase64Converter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Base64|Encoding")
	static FString StringConvertToBase64(const FString& InString);

	UFUNCTION(BlueprintCallable, Category = "Base64|Decoding")
	static FString StringConvertFromBase64(const FString& InString);
};
