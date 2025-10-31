// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/StateTreeTaskBlueprintBase.h"
#include "CrowdTalkTask.generated.h"

/**
 * StateTree Task for Crowd dialogue system
 */
UCLASS()
class UE_TTTK_API UCrowdTalkTask : public UStateTreeTaskBlueprintBase
{
	GENERATED_BODY()

protected:
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition) override;
	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) override;
	virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) override;

public:
	UPROPERTY()
	class ACrowd* OwnerCrowd;

	UPROPERTY()
	class ACrowdAiController* AIController;
};
