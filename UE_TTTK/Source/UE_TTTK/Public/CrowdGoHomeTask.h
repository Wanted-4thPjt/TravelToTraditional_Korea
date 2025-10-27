// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "CrowdGoHomeTask.generated.h"

/**
 * 
 */
UCLASS()
class UE_TTTK_API UCrowdGoHomeTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()
public:
	UCrowdGoHomeTask(const FObjectInitializer& ObjectInitializer);
	class ACrowdTargetPoint* TargetPoint;
	class ACrowd* OwnerCrowd;
	class ACrowdAiController* AIController;

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual void StateCompleted(FStateTreeExecutionContext& Context, const EStateTreeRunStatus CompletionStatus,
		const FStateTreeActiveStates& CompletedActiveStates) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;
};
