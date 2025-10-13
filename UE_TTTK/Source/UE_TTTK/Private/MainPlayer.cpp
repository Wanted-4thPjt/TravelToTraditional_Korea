// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayer.h"

#include "PlayerSubSystem.h"
#include "Interaction/InteractableComponent.h"
#include "Interaction/ViewComponent.h"

AMainPlayer::AMainPlayer()
{
	viewComponent = CreateDefaultSubobject<UViewComponent>(TEXT("View"));
}

void AMainPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		viewComponent->OnViewSthByLineTrace.AddDynamic(this, &AMainPlayer::OnViewInteractableActor);
		viewComponent->EnableTrace(true);
	}
}

void AMainPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMainPlayer::OnViewInteractableActor_Implementation(const FHitResult& hitResult)
{
	if (hitResult.GetActor() != focusedActor)
	{
		focusedActor->FindComponentByClass<UInteractableComponent>()->TryDeactivateInteractable(GetController<APlayerController>());
	}
	UInteractableComponent* interactable = hitResult.GetActor()->FindComponentByClass<UInteractableComponent>();
	if (!IsValid(interactable)) {return;}
	if (GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		interactable->TryActivateInteractable(GetController<APlayerController>());
		focusedActor = hitResult.GetActor();
	}
}


// void AMainPlayer::RequestChangeInputMapping(EMappingMode mode)
// {
// 	UPlayerSubSystem* playerManager = GetGameInstance()->GetSubsystem<UPlayerSubSystem>();
// 	if (playerManager)
// 	{
// 		//playerManager->ChangeInputMapping(this,mode);
// 	}
// }

