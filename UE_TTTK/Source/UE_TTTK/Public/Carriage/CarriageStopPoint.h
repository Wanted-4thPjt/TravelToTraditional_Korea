// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CarriageStopPoint.generated.h"

UCLASS()
class UE_TTTK_API ACarriageStopPoint : public AActor
{
	GENERATED_BODY()

public:
	ACarriageStopPoint();

	//~ Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBillboardComponent* BillboardComponent;

	//~ Stop Settings
	/** 정차 시간 (초) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carriage Stop")
	float StopDuration;

	/** 정류장 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carriage Stop")
	FString StopName;

	/** 종점 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carriage Stop")
	bool bIsTerminal;

	/** 정류장 순서 (0부터 시작) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Carriage Stop")
	int32 StopIndex;

	//~ Runtime State
	/** 방문 여부 (런타임) */
	UPROPERTY(BlueprintReadOnly, Category = "Carriage Stop")
	bool bVisited;

	//~ Functions
	/** 방문 상태 초기화 */
	UFUNCTION(BlueprintCallable, Category = "Carriage Stop")
	void ResetVisited();

	/** 정차 시간 반환 */
	UFUNCTION(BlueprintPure, Category = "Carriage Stop")
	float GetStopDuration() const { return StopDuration; }

	/** 종점 확인 */
	UFUNCTION(BlueprintPure, Category = "Carriage Stop")
	bool IsTerminal() const { return bIsTerminal; }

	/** 마차 도착 이벤트 */
	UFUNCTION(BlueprintNativeEvent, Category = "Carriage Stop")
	void OnCarriageArrived(class ACarriageVehicle* Carriage);

	/** 마차 출발 이벤트 */
	UFUNCTION(BlueprintNativeEvent, Category = "Carriage Stop")
	void OnCarriageDeparted(class ACarriageVehicle* Carriage);

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
