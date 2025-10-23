// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Crowd.h"
#include "PatrolArmy.generated.h"

/**
 * 
 */
UCLASS()
class UE_TTTK_API APatrolArmy : public ACrowd
{
	GENERATED_BODY()
public :
	UPROPERTY(EditAnywhere)
	class APatrolPath* armyPath;
	int32 idx =0; //current 인덱스
	bool bIsNight = false;
	bool bIsForward = true;
	void PlayHangFireAnimation();
	class APatrolPath* GetPatrolPath();
	void SetPatrolPath(class APatrolPath* path);

	

	struct FPatrolPoint* GetcurrentPatrolPoint();
	struct FPatrolPoint* GetNextPatrolPoint();
	struct FPatrolPoint* GetPreviousPatrolPoint();
	void UpdateIndex();
	void SetIndex(int32 index);
	bool isFinalPatrolPoint();
};
