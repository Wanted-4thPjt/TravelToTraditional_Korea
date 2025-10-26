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
	APatrolArmy();
	UPROPERTY(EditAnywhere)
	class APatrolPath* armyPath;
	int32 idx =0; //current 인덱스
	bool bIsNight = false;
	bool bIsForward = true;
	bool bIsHangingFireAnimCompleted = false; // HangingFire 애니메이션 완료 플래그
	UPROPERTY(EditAnywhere)
	UAnimMontage* HangingFireMontage;
	
	void PlayHangFireAnimation();
	class APatrolPath* GetPatrolPath();
	void SetPatrolPath(class APatrolPath* path);
	
	struct FPatrolPoint* GetcurrentPatrolPoint();
	struct FPatrolPoint* GetNextPatrolPoint();
	struct FPatrolPoint* GetPreviousPatrolPoint();
	void UpdateIndex();
	void SetIndex(int32 index);
	bool isFinalPatrolPoint();
	UPROPERTY()
	class AActor* handFire;
	UPROPERTY(EditAnywhere,Category= "Torch")
	TSubclassOf<AActor> FireActorClass;

	UFUNCTION(BlueprintCallable)
	bool GetIsNight(){return bIsNight;};
	bool GetHangingFireAnimCompleted() const { return bIsHangingFireAnimCompleted; }
	void SetHangingFireAnimCompleted(bool bCompleted) { bIsHangingFireAnimCompleted = bCompleted; }

public:
	void PlayHaningMontage();
	UFUNCTION(BlueprintCallable)
	void EqiqueFire();
	virtual void BeginPlay() override;
	
};
