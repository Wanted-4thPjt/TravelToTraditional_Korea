#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "MapInfo.generated.h"

USTRUCT(BlueprintType)
struct UE_TTTK_API FMapInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapInfo")
	FText displayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapInfo")
	FString mapName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapInfo", meta=(AllowedClasses="/Script/Engine.World"))
	TSoftObjectPtr<UWorld> mapAsset;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapInfo")
	TSoftObjectPtr<UTexture2D> mapIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapInfo", meta=(ClampMin=1, ClampMax=10))
	int32 maxPlayers;

	FMapInfo()
		:displayName(FText::GetEmpty()), mapName("")
		, mapIcon(nullptr), maxPlayers(1)
	{}
};
