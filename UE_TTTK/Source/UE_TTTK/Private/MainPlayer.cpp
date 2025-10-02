// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayer.h"

#include "PlayerSubSystem.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

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



void AMainPlayer::RequestChangeInputMapping(EMappingMode mode)
{
	if (!IsLocallyControlled) //다른 캐릭터에서 기존 캐릭터의 매핑 컨텍스트를 막으려는 경우 방지
		return;
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
