// Fill out your copyright notice in the Description page of Project Settings.


#include "Carriage/CarriageStopPoint.h"
#include "Components/BillboardComponent.h"

// Sets default values
ACarriageStopPoint::ACarriageStopPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	// Billboard Component 생성 (에디터에서 위치 표시용)
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	RootComponent = BillboardComponent;

	// 기본값 설정
	StopDuration = 5.0f;
	StopName = TEXT("정류장");
	bIsTerminal = false;
	StopIndex = 0;
	bVisited = false;
}

void ACarriageStopPoint::OnCarriageArrived_Implementation(class ACarriageVehicle* Carriage)
{
	
}

void ACarriageStopPoint::OnCarriageDeparted_Implementation(class ACarriageVehicle* Carriage)
{
}

void ACarriageStopPoint::BeginPlay()
{
	Super::BeginPlay();
	bVisited = false;
}

void ACarriageStopPoint::ResetVisited()
{
	bVisited = false;
}

#if WITH_EDITOR
void ACarriageStopPoint::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 에디터에서 속성 변경 시 처리할 로직 (필요시 추가)
}
#endif
