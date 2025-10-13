#pragma once

#include "CoreMinimal.h"
#include "ContentConfig.generated.h"

USTRUCT(BlueprintType)
struct UE_TTTK_API FContentConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Intro")
	FName contentName = FName();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Intro")
	FText Description = FText();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Location")
	TArray<FTransform> playerSpawnPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom")
	TArray<FName> customSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay", meta=(ToolTip="@@ TimeLimit == -1 : 제한 시간 없음."))
	float timeLimit = -1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Flag")
	bool bRecordingScore = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Flag")
	bool bRecordingTime = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Custom")
	TArray<FName> customRecordingData;
};