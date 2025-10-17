// Fill out your copyright notice in the Description page of Project Settings.


#include "Jegi/Jegi.h"

#include "MainPlayer.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AJegi::AJegi()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Engine/EngineMeshes/Sphere.Sphere"));
	if (tempMesh.Succeeded())
	{
		meshComponent->SetStaticMesh(tempMesh.Object);
	}
	
	movementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	movementComponent->SetIsReplicated(true);
	movementComponent->bSimulationEnabled = true;
	movementComponent->MaxSpeed = 300.f;
}

// Called when the game starts or when spawned
void AJegi::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AJegi::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AJegi::OnKick(EKickTiming inTiming)
{
	Server_OnKicked(inTiming);
}

void AJegi::Server_OnKicked_Implementation(EKickTiming inTiming)
{
	switch (inTiming)
	{
	case EKickTiming::Default:
		return;
	case EKickTiming::Missed:
		return;
	case EKickTiming::Good:
		return;
	case EKickTiming::Great:
		return;
	case EKickTiming::Perfect:
		return;
	}

	
}

