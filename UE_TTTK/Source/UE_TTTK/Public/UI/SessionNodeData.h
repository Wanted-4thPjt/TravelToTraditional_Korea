#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "SessionNodeData.generated.h"

UCLASS(BlueprintType)
class UE_TTTK_API USessionNodeData : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category="SessionData")
	FString hostName;
	UPROPERTY(BlueprintReadOnly, Category="SessionData")
	TSoftObjectPtr<UTexture2D> mapIcon;
	UPROPERTY(BlueprintReadOnly, Category="SessionData")
	FString mapName;
	UPROPERTY(BlueprintReadOnly, Category="SessionData")
	int32 currentPlayerCount;
	UPROPERTY(BlueprintReadOnly, Category="SessionData")
	int32 maxPlayerCount;
	UPROPERTY(BlueprintReadOnly, Category="SessionData")
	int32 ping;

	UPROPERTY(BlueprintReadOnly, Category="SessionData|JoinSession")
	int32 searchResultIndex;
	
};
