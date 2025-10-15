// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ViewComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnViewSthByLineTrace, const FHitResult&, hitResult);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UViewComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UViewComponent();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;;

public:
	UFUNCTION(Client, Reliable, BlueprintCallable)
	void EnableTrace(bool bEnable);
	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AActor* GetFocusingActor() const {return focusingActor;}

private:
	void ShootLineTrace();
	bool IsInViewAngle(const AActor* inTarget) const;

public:
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite, Category="Event")
	FOnViewSthByLineTrace OnViewSthByLineTrace;
	
protected:
	UPROPERTY()
	TObjectPtr<APawn> pawnOwner;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> ownerEye = nullptr;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Replication", meta=(AllowPrivateAccess="true"))
	TObjectPtr<AActor> focusingActor = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Line Trace")
	float traceDistance = 1000.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Line Trace")
	float traceInterval = 0.05f;  // per 3 frames
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="View Angle")
	float halfViewAngle = 60.f;

	FTimerHandle traceTimer;

private:
	FCollisionQueryParams queryParams;
	FCollisionResponseParams responseParams;
};
