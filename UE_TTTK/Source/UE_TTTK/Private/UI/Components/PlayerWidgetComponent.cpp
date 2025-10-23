// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Components/PlayerWidgetComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/LocalPlayer.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
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

	InitPlayerControllerWidget();
	BindInputMappingContext();
	
}

void UPlayerWidgetComponent::UpdateChat(const FText& newText)
{
	UChatLineWidget* chatLine = CreateWidget<UChatLineWidget>(chatWidget->chatScrollBox, chatLineWidgetFactory);
	chatLine->chatLineText->SetText(newText);
	chatWidget->chatScrollBox->AddChild(chatLine);
	
	chatWidget->chatScrollBox->ScrollToEnd();
	FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Cleared);
	GetOwner<APlayerController>()->SetInputMode(FInputModeGameOnly());
}

void UPlayerWidgetComponent::InitPlayerControllerWidget()
{
	playerWidget = CreateWidget<UPlayerWidget>(GetOwner<APlayerController>(), playerWidgetFactory);
	chatWidget = playerWidget->chatWidget;
}

void UPlayerWidgetComponent::BindInputMappingContext()
{
	APlayerController* pc = GetOwner<APlayerController>();

	if (UEnhancedInputComponent* eic = Cast<UEnhancedInputComponent>(pc->GetPawn()->InputComponent))
	{
		eic->BindAction(IA_Chat, ETriggerEvent::Triggered, this, &UPlayerWidgetComponent::OnInputChatKey);
	}
	
	if (pc == GetWorld()->GetFirstPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_UI, 1);
		}
	}
}

void UPlayerWidgetComponent::OnInputChatKey(const FInputActionValue& inputActionValue)
{
	if (chatWidget->HasKeyboardFocus())
	{
		onInputChatKey.ExecuteIfBound(chatWidget->inputChatBox->GetText());
	}
}

