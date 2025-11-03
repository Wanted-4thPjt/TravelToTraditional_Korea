// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu/MainMenuPawn.h"

#include "Camera/CameraComponent.h"


// Sets default values
AMainMenuPawn::AMainMenuPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cam->SetupAttachment(GetRootComponent());
	cam->SetProjectionMode(ECameraProjectionMode::Orthographic);
}

// Called when the game starts or when spawned
void AMainMenuPawn::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called to bind functionality to input
void AMainMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

