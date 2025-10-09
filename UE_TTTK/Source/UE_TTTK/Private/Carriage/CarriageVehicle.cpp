// Fill out your copyright notice in the Description page of Project Settings.


#include "Carriage/CarriageVehicle.h"

#include "Carriage/CarriageMovementComponent.h"
#include "Components/BoxComponent.h"

ACarriageVehicle::ACarriageVehicle()
{
	PrimaryActorTick.bCanEverTick = true;

	CarriageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarriageMesh"));
	RootComponent = CarriageMesh;
	WheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelMesh"));
	WheelMesh->SetupAttachment(RootComponent);


	BoardArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BoardingArea"));
	BoardArea->SetupAttachment(RootComponent);
	BoardArea->SetBoxExtent(FVector(200.f,200.f,100.f));

	// Ground Trace Points
	FrontTracePoint = CreateDefaultSubobject<USceneComponent>(TEXT("FrontTracePoint"));
	FrontTracePoint->SetupAttachment(RootComponent);
	FrontTracePoint->SetRelativeLocation(FVector(200.0f, -100.0f, 0.0f));

	RearTracePoint = CreateDefaultSubobject<USceneComponent>(TEXT("RearTracePoint"));
	RearTracePoint->SetupAttachment(RootComponent);
	RearTracePoint->SetRelativeLocation(FVector(-200.0f, 100.0f, 0.0f));

	MovementComponent = CreateDefaultSubobject<UCarriageMovementComponent>(TEXT("MovementComponent"));

	Speed =300.f;
	bLooping=true;
	PathActor = nullptr;

	bReplicates = true;
	SetReplicateMovement(true);
}

void ACarriageVehicle::BeginPlay()
{
	Super::BeginPlay();
	if (HasAuthority())
	{
		StartMovement();
	}

}

void ACarriageVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateWheel();
}

void ACarriageVehicle::StartMovement()
{
	UE_LOG(LogTemp,Warning,TEXT("ACarriageVehicle::StartMovement"));
	if (MovementComponent)
	{
		MovementComponent->StartMovement();
	}
}

void ACarriageVehicle::StopMovement()
{
	UE_LOG(LogTemp,Warning,TEXT("ACarriageVehicle::StopMovement"));
	if (MovementComponent)
	{
		MovementComponent->StopMovement();
	}
}

void ACarriageVehicle::RotateWheel()
{
	if (!MovementComponent->bisMoving)return;
	if (!MovementComponent->bisStoped)
	{
		FRotator wheelRotation = WheelMesh->GetRelativeRotation();
		int  newRotRoll = ((int)wheelRotation.Roll+(int)WheelSpeed)%360;
		FRotator newWheelRotation = FRotator(0.f, 0.f, newRotRoll);
		WheelMesh->SetRelativeRotation(newWheelRotation);
	}
}
