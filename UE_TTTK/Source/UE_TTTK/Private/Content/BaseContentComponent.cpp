// Fill out your copyright notice in the Description page of Project Settings.

#include "Content/BaseContentComponent.h"
#include "EnhancedInputComponent.h"
#include "GameFlow/BaseContentManager.h"

UBaseContentComponent::UBaseContentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(false);  // Input은 Client Local
	bAutoActivate = false;  // 자동 활성화 안 함
}

void UBaseContentComponent::ActivateContentInput_Implementation(UBaseContentManager* manager)
{
	if (!manager)
	{
		UE_LOG(LogTemp, Error, TEXT("BaseContentComponent: ActivateContentInput called with null manager"));
		return;
	}

	ownerContentManager = manager;
	bContentActive = true;

	UE_LOG(LogTemp, Log, TEXT("[%s] Content Input Activated"), *GetClass()->GetName());
}

void UBaseContentComponent::DeactivateContentInput_Implementation()
{
	ownerContentManager = nullptr;
	bContentActive = false;

	UE_LOG(LogTemp, Log, TEXT("[%s] Content Input Deactivated"), *GetClass()->GetName());
}

void UBaseContentComponent::SetupInputBindings_Implementation(UEnhancedInputComponent* InputComponent)
{
	// 자식 클래스에서 오버라이드하여 구체적인 Input 바인딩 설정
	if (!InputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseContentComponent: SetupInputBindings called with null InputComponent"));
		return;
	}
}

void UBaseContentComponent::ClearInputBindings_Implementation(UEnhancedInputComponent* InputComponent)
{
	// 필요시 자식 클래스에서 오버라이드
	// Enhanced Input System은 Context 제거로 자동 해제되므로 대부분 불필요
}
