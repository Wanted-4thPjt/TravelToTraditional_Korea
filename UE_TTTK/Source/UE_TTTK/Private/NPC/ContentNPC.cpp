// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/ContentNPC.h"


// Sets default values
AContentNPC::AContentNPC()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AContentNPC::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AContentNPC::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AContentNPC::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

