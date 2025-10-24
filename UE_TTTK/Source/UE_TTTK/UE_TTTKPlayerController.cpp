// Copyright Epic Games, Inc. All Rights Reserved.


#include "UE_TTTKPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "TTTK_GameState.h"
#include "Blueprint/UserWidget.h"
#include "UE_TTTK.h"
#include "UI/Components/PlayerWidgetComponent.h"
#include "Widgets/Input/SVirtualJoystick.h"

AUE_TTTKPlayerController::AUE_TTTKPlayerController()
{
	playerWidgetComponent = CreateDefaultSubobject<UPlayerWidgetComponent>("PlayerWidgets");
}

void AUE_TTTKPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {
			UE_LOG(LogUE_TTTK, Error, TEXT("Could not spawn mobile controls widget."));
		}
	}
}

void AUE_TTTKPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

void AUE_TTTKPlayerController::UpdateChat(const FText& inText)
{
	playerWidgetComponent->UpdateChat(inText);
}

void AUE_TTTKPlayerController::Server_SendChat_Implementation(const FText& inText)
{
	if (ATTTK_GameState* gs = GetWorld()->GetGameState<ATTTK_GameState>())
	{
		gs->ReceiveChat(inText);
	}
}
