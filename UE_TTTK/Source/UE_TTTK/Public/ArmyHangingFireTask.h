// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "ArmyHangingFireTask.generated.h"

/**
 * 
 */
UCLASS()
class UE_TTTK_API UArmyHangingFireTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
public:
	class ACrowdAiController* AiController;
	class APatrolArmy* OwnerArmy;
	float currentTime =0.f;

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
		const FStateTreeActiveStates& CompletedActiveStates) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
};
