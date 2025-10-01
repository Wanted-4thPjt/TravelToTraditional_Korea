// Fill out your copyright notice in the Description page of Project Settings.


#include "Public/UI/MainMenu.h"

#include "SocketSubsystem.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"


void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();
	urlInput->SetVisibility(ESlateVisibility::Hidden);
	joinButton->OnClicked.AddDynamic(this, &UMainMenu::ClickJoinButton);
	//urlInput->OnTextChanged.AddDynamic(this, &UMainMenu::InputUrl);
	urlInput->OnTextCommitted.AddDynamic(this, &UMainMenu::JoinToUrl);
	hostButton->OnClicked.AddDynamic(this, &UMainMenu::CreateHost);
}

void UMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
}

void UMainMenu::CreateHost()
{
	bool bCanBindAll;
	TSharedPtr<FInternetAddr> LocalAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);
    
	url = LocalAddr.IsValid() ?
		 LocalAddr->ToString(false) : "127.0.0.1";
	
	UE_LOG(LogTemp, Display, TEXT("Host URL : %s"), *url);
	// Get Level Name from GameInstance or GameMode later.
	UGameplayStatics::OpenLevel(this, FName(""), true, "listen");
	
}

void UMainMenu::ClickJoinButton()
{
	urlInput->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenu::JoinToUrl(const FText& inText, ETextCommit::Type inCommitMethod)
{
	if (inCommitMethod != ETextCommit::OnEnter)
	{
		return;
	}
	//inText 에 해당하는 url 검증 과정 필요
	UGameplayStatics::OpenLevel(this, *inText.ToString(), true);
}

void UMainMenu::InputUrl(const FText& changedText)
{
}
