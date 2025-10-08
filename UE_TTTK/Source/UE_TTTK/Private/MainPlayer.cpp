// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayer.h"

#include "PlayerSubSystem.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

void AMainPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr)
	{
		if (PC-> WasInputKeyJustPressed(EKeys::F))
		{
			UE_LOG(LogTemp, Warning, TEXT("상호작용 방금 눌림!"));
			RequestChangeInputMapping(EMappingMode::Content2);
		}
		if (PC-> WasInputKeyJustPressed(EKeys::B))
		{
			UE_LOG(LogTemp, Warning, TEXT("다시 되돌아가기"));
			ReturnToDefaultMode();
		}
	}
	
}

void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMainPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	myInputComponent = PlayerInputComponent;
}



void AMainPlayer::RequestChangeInputMapping(EMappingMode mode)
{

	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* localPlayer = playerController->GetLocalPlayer())
		{
			if (UPlayerSubSystem* playerManager = localPlayer->GetSubsystem<UPlayerSubSystem>())
			{
				
				playerManager->ChangeInputMapping(this, mode);
			}
		}
	}
}

void AMainPlayer::ReturnToDefaultMode()
{
	RequestChangeInputMapping(EMappingMode::Content1);
	SetupPlayerInputComponent(myInputComponent);
}
