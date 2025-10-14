// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TargetPoint.h"
#include "CrowdTargetPoint.generated.h"


class ACrowd;
enum class ECrowdType : uint8;

USTRUCT()
struct FCrowdPoint
{
	GENERATED_BODY()

	FVector location;
	bool inCrowd = false;
	class ACrowd* currentCrowd = nullptr;

	FCrowdPoint(){}

	// 매개변수 생성자
	FCrowdPoint(FVector InLocation, bool bInCrowd, ACrowd* InCrowd)
		: location(InLocation), inCrowd(bInCrowd), currentCrowd(InCrowd) {}
};
UCLASS()
class UE_TTTK_API ACrowdTargetPoint : public ATargetPoint
{
	GENERATED_BODY()
public:
	ACrowdTargetPoint();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString targetName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECrowdType Crowdtype;
	TArray<struct FCrowdPoint*> subTargets;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 NumPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float radius;
		
	UFUNCTION()
	void InitializeCrowdPoint_circle();
	UFUNCTION()
	bool IsSubTargetFull();
	UFUNCTION()
	FVector FindEmptySubTarget();
	UFUNCTION()
	void ProcessSubTargetIn(class ACrowd* InCrowd,FVector location);
	UFUNCTION()
	void ProcessSubTargetOut(class ACrowd* OutCrowd);
	UFUNCTION()
	int32 FindIndexByCrowd(class ACrowd* crowd);
	UFUNCTION()
	int32 FindIndexByLocation(FVector location);
	UFUNCTION()
	void DrawDebugPoint();
	
	
};
