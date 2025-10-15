#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

class UInteractableComponent;
class AMainPlayer;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void OnRegister() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
public:
	UFUNCTION(BlueprintCallable)
	void Interaction();

protected:
	UFUNCTION(Server, Reliable)
	void Server_Interact(UInteractableComponent* interactable);

	UFUNCTION()
	void FocusInteractableActor(const FHitResult& hitResult);
	
protected:
	UPROPERTY()
	TObjectPtr<AMainPlayer> ownerPlayer = nullptr;

	UPROPERTY()
	TObjectPtr<APlayerController> ownerController = nullptr;
	
	UPROPERTY(Replicated)
	AActor* focusingActor;
};
