// Fill out your copyright notice in the Description page of Project Settings.


#include "Carriage/CarriageVehicle.h"

#include "Carriage/CarriageMovementComponent.h"
#include "Components/BoxComponent.h"

ACarriageVehicle::ACarriageVehicle()
{
	PrimaryActorTick.bCanEverTick = false;

	CarriageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarriageMesh"));
	RootComponent = CarriageMesh;

	BoardArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BoardingArea"));
	BoardArea->SetupAttachment(RootComponent);
	BoardArea->SetBoxExtent(FVector(200.f,200.f,100.f));

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
