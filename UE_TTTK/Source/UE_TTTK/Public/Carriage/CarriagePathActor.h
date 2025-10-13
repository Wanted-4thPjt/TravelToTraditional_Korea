// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarriagePathActor.generated.h"

UCLASS()
class UE_TTTK_API ACarriagePathActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACarriagePathActor();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Component)
	class USplineComponent* PathSpline;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = path)
	FString PathNme;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = path)
	TArray<class ACarriageStopPoint*> StopPoints;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = path)
	float StopPointSearchRadius;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedEvent) override;
	#endif

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintPure,Category=path)
	float GetPathLength() const;
	UFUNCTION(BlueprintPure,Category=path)
	FVector GetLocationAtDistance(float Distance) const;
	UFUNCTION(BlueprintPure,Category=path)
	FRotator GetRotationAtDistance(float Distance) const;
	UFUNCTION(BlueprintCallable,Category=path)
	void CollectStopPoint();
	UFUNCTION(BlueprintPure,Category=path)
	ACarriageStopPoint* FindNearestStopPoint(float Distance,float Tolerance = 50.f);
	
};
