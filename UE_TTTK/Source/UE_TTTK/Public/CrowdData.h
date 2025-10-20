// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CrowdData.generated.h"

UENUM(BlueprintType)
enum class ECrowdType : uint8
{
	Marketeer      UMETA(DisplayName = "시장 아지매"),
	Solider      UMETA(DisplayName = "군인"),
	Kid      UMETA(DisplayName = "애기")
};
UCLASS()
class UE_TTTK_API UCrowdData : public UDataAsset
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	ECrowdType CrowdType;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float WalkSpeed;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	bool bReturntoHomeAtNight;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float WanderRadius;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UAnimMontage* GreetingAnimation;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float TalkTime;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UAnimMontage* BumpAnimation;
	UPROPERTY(EditDefaultsOnly)
	FVector2D GoWorkTime;
	UPROPERTY(EditDefaultsOnly)
	FVector2D GoHomeTime;

public:
	UFUNCTION()
	ECrowdType GetCrowdType(){return CrowdType;}
	UFUNCTION()
	FVector2D GetGoWorkTime(){return GoWorkTime;}
	UFUNCTION()
	FVector2D GetGoHomeTime(){return GoHomeTime;}
	
};
