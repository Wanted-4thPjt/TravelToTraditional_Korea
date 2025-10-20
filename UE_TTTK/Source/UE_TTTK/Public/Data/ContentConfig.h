#pragma once

#include "CoreMinimal.h"
#include "ContentConfig.generated.h"

enum class EMappingMode : uint8;

USTRUCT(BlueprintType)
struct UE_TTTK_API FContentConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Intro")
	FName contentName = FName();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Intro")
	FText Description = FText();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Intro")
	UTexture2D* thumbnailImage = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DynamicSetting|InputMode", meta=(ValidEnumValues="/Script/UE_TTTK.EMappingMode"))
	EMappingMode mappingMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DynamicSetting|SpawnLocation")
	TArray<FTransform> playerSpawnPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DynamicSetting|Camera")
	TArray<FTransform> cameraTransform;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DynamicSetting|Custom")
	TArray<FName> customSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Time", meta=(ToolTip="@ TimeLimit < 1 : 제한 시간 없음."))
	float contentTimeLimit = -1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Time", meta=(ToolTip="@ TimeLimit < 1 : 제한 시간 없음."))
	float roundTimeLimit = -1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Flag")
	bool bAllowMidGameQuit = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Flag")
	bool bIsTeamBased = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Flag")
	bool bChangeCameraView = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Flag")
	bool bRecordingScore = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Flag")
	bool bRecordingTime = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Custom")
	TArray<FName> customRecordingData;
};