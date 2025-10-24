// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/PlayerWidgetComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "UI/ChatLineWidget.h"
#include "UI/PlayerWidget.h"
#include "UI/ChatWidget.h"


// Sets default values for this component's properties
UPlayerWidgetComponent::UPlayerWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	if (ConstructorHelpers::FObjectFinder<UInputMappingContext> tempIMC_UI(TEXT("/Game/Input/IMC_UI.IMC_UI"));
		tempIMC_UI.Succeeded()
	)
	{
		IMC_UI = tempIMC_UI.Object;
	}
	if (ConstructorHelpers::FClassFinder<UPlayerWidget> tempPlayerWidget(TEXT("/Game/UI/InContent/WBP_Player.WBP_Player_C"));
		tempPlayerWidget.Succeeded()
	)
	{
		playerWidgetFactory = tempPlayerWidget.Class;
	}
	if (ConstructorHelpers::FClassFinder<UPlayerWidget> tempPlayerWidget(TEXT("/Game/UI/InContent/WBP_ChatLine.WBP_ChatLine_C"));
		tempPlayerWidget.Succeeded()
	)
	{
		chatLineWidgetFactory = tempPlayerWidget.Class;
	}
}


// Called when the game starts
void UPlayerWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ownerPlayerController = GetOwner<APlayerController>();
	InitPlayerControllerWidget();
	BindInputMappingContext();
}


void UPlayerWidgetComponent::InitPlayerControllerWidget()
{
	if (!IsValid(ownerPlayerController)) {return;}
	playerWidget = CreateWidget<UPlayerWidget>(ownerPlayerController, playerWidgetFactory);
	chatWidget = playerWidget->chatWidget;
}

void UPlayerWidgetComponent::BindInputMappingContext()
{
	if (!IsValid(ownerPlayerController)) {return;}
	
	if (ownerPlayerController == GetWorld()->GetFirstPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ownerPlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_UI, 1);
		}
	}
	if (UEnhancedInputComponent* eic = Cast<UEnhancedInputComponent>(ownerPlayerController->InputComponent))
	{
		for (auto mapping : IMC_UI->GetMappings())
		{
			UE_LOG(LogTemp, Warning, TEXT("FKey Name == %s"), *(mapping.Key.ToString()));
			if (mapping.Key == "Enter")
			{
				eic->BindAction(IA_Chat, ETriggerEvent::Triggered, this, &UPlayerWidgetComponent::OnInputChatKey);
				
			}
		}
		eic->BindAction(IA_Setting, ETriggerEvent::Triggered, this, &UPlayerWidgetComponent::OnInputSettingKey);
	}
}

void UPlayerWidgetComponent::UpdateChat(const FText& newText)
{
	if (!IsValid(ownerPlayerController)) {return;}
	float scrollOffset = chatWidget->chatScrollBox->GetScrollOffset();
	float scrollOffsetOfEnd = chatWidget->chatScrollBox->GetScrollOffsetOfEnd();
	
	UChatLineWidget* chatLine = CreateWidget<UChatLineWidget>(chatWidget->chatScrollBox, chatLineWidgetFactory);
	chatLine->SetChatText(newText);
	chatWidget->chatScrollBox->AddChild(chatLine);

	if (scrollOffset == scrollOffsetOfEnd)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			[this]() -> void
			{
				chatWidget->chatScrollBox->ScrollToEnd();
			}
		);
	}
}

void UPlayerWidgetComponent::OnInputChatKey(const FInputActionValue& inputActionValue)
{
	if (!IsValid(ownerPlayerController)) {return;}

	// Keyboard Focus가 자동으로 Player Key Input을 Consume하기 때문에, UI Mode가 유지되는 동안 추가적으로 Input을 제어할 필요 X
	uiInputMode.SetWidgetToFocus(chatWidget->inputChatBox->TakeWidget());
	uiInputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
	
	ownerPlayerController->SetShowMouseCursor(true);
	ownerPlayerController->SetInputMode(uiInputMode);
}

void UPlayerWidgetComponent::OnInputSettingKey(const FInputActionValue& inputActionValue)
{
	if (!IsValid(ownerPlayerController)) {return;}

	//uiInputMode.SetWidgetToFocus();
	
}

