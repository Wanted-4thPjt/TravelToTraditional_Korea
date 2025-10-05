// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ViewComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableTargetChanged, AActor*, newTarget);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UViewComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UViewComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;;

public:
	UFUNCTION(BlueprintCallable)
	void EnableTrace(const bool& bEnable);

private:
	void ShootLineTrace();
	bool IsInViewAngle(AActor* inTarget, float& outDotProduct);

	void UpdateOutline(AActor* newTarget);

public:
	FOnInteractableTargetChanged onInteractableTargetChanged;
	
protected:
	UPROPERTY()
	TObjectPtr<APawn> pawnOwner;
	UPROPERTY()
	TObjectPtr<USceneComponent> ownerEye = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> interactableTarget;

	UPROPERTY()
	TObjectPtr<AActor> outlinedTarget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Line Trace")
	float traceDistance = 1000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Line Trace")
	float traceInterval = 0.05f;  // per 3 frames

	FTimerHandle traceTimer;
};
