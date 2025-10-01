// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayer.h"

#include "PlayerSubSystem.h"

void AMainPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
}

void AMainPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// void AMainPlayer::RequestChangeInputMapping(EMappingMode mode)
// {
// 	UPlayerSubSystem* playerManager = GetGameInstance()->GetSubsystem<UPlayerSubSystem>();
// 	if (playerManager)
// 	{
// 		//playerManager->ChangeInputMapping(this,mode);
// 	}
// }
