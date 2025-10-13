// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSubSystem.h"

#include "EnhancedInputSubsystems.h"
#include "MainPlayer.h"
#include "SubSystemSettings.h"


void UPlayerSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	const USubSystemSettings* Settings = USubSystemSettings::Get();
	if (Settings)
	{
		InputMappings.Empty();

		// Settings에서 가져와서 TMap 채우기
		if (UInputMappingContext* Context1 = Settings->Content1Context.LoadSynchronous())
		{
			InputMappings.Add(EMappingMode::Content1, Context1);
		}

		if (UInputMappingContext* Context2 = Settings->Content2Context.LoadSynchronous())
		{
			InputMappings.Add(EMappingMode::Content2, Context2);
		}

		if (UInputMappingContext* Context3 = Settings->Content3Context.LoadSynchronous())
		{
			InputMappings.Add(EMappingMode::Content3, Context3);
		}
		UE_LOG(LogTemp, Warning, TEXT("초기화"));
	}
}

bool UPlayerSubSystem::ChangeInputMapping(AMainPlayer* requester, EMappingMode mode)
{
	UE_LOG(LogTemp, Warning, TEXT("InputMappings체인지"));
	if (InputMappings.Contains(mode))
	{
		APlayerController* requesterController = Cast<APlayerController>(requester->GetController());
		if (requesterController!= nullptr)
		{
			ULocalPlayer* Lp = requesterController->GetLocalPlayer();
			if (Lp != nullptr && requesterController->IsLocalController())
			{
				UEnhancedInputLocalPlayerSubsystem* subsy = Lp-> GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
				if (subsy != nullptr)
				{
					UInputMappingContext* oldContext = InputMappings[requester->GetCurrentMode()];
					UInputMappingContext* changeContext = InputMappings[mode];
					subsy->RemoveMappingContext(oldContext);
					subsy->AddMappingContext(changeContext,0);
					UE_LOG(LogTemp, Warning, TEXT("바꼇겟지?"));
					return true;
				}
			}
		}
	}
		
	return false;
}

UInputMappingContext* UPlayerSubSystem::GetInputMappingContext(EMappingMode mode)
{
	if (InputMappings.Contains(mode))
	{
		return InputMappings[mode];
	}
	UE_LOG(LogTemp,Warning,TEXT("mode를 통한 GetMappingContext nullptr"));
	return nullptr;
}


