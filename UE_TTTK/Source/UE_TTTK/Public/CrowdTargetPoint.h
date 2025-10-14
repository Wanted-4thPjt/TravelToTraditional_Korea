// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "CrowdTargetPoint.generated.h"

enum class ECrowdType : uint8;
/**
 * 
 */
UCLASS()
class UE_TTTK_API ACrowdTargetPoint : public ATargetPoint
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString targetName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECrowdType Crowdtype;
};
