// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerSubSystem.h"

#include "EnhancedInputSubsystems.h"


// bool UPlayerSubSystem::ChangeInputMapping(AMainPlayer* requester, EMappingMode mode)
// {
// 	if (InputMappings.Contains(mode))
// 	{
// 		APlayerController* requesterController = Cast<APlayerController>(requester->GetController());
// 		if (requesterController!= nullptr)
// 		{
// 			ULocalPlayer* Lp = requesterController->GetLocalPlayer();
// 			if (Lp != nullptr)
// 			{
// 				UEnhancedInputLocalPlayerSubsystem* subsy = Lp-> GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
// 				if (subsy != nullptr)
// 				{
// 					UInputMappingContext* changeContext = InputMappings[mode];
// 					subsy->AddMappingContext(changeContext,0);
// 					return true;
// 				}
// 			}
// 		}
// 	}
// 		
// 	return false;
// }
