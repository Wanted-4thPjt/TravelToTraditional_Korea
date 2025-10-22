// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "ArmyPatrolTask.generated.h"

/**
 * 
 */
UCLASS()
class UE_TTTK_API UArmyPatrolTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
	
	

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
		const FStateTreeActiveStates& CompletedActiveStates) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
public:
	class APatrolArmy* OwnerArmy;
	class ACrowdAiController* AiController;
	UArmyPatrolTask(const FObjectInitializer& ObjectInitializer);
	
};
