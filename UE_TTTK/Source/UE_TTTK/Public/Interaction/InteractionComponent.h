#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

struct FInputActionValue;
class UContentEntryComponent;
class UInputAction;
class UInputMappingContext;
class UInteractableComponent;
class AMainPlayer;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	
public:	
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AActor* GetFocusedActor() {return focusingActor;}
	UFUNCTION()
	void InteractKeyInput(const FInputActionValue& value);
	UFUNCTION()
	void FocusInteractableActor(const FHitResult& hitResult);

protected:
	UFUNCTION(Server, Reliable)
	void Server_Interact(UInteractableComponent* interactable);
	UFUNCTION(Server, Reliable)
	void Server_FinishInteraction();

	UFUNCTION(Server, Reliable)
	void Server_Focus(AActor* focusedActor);

protected:
	UPROPERTY(Replicated)
	TObjectPtr<AActor> focusingActor;
	UPROPERTY(Replicated)
	TObjectPtr<UContentEntryComponent> possessingInteractedTarget;
	
	UPROPERTY()
	APlayerController* ownerPlayerController = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Input|Context")
	TObjectPtr<UInputMappingContext> IMC_Interaction;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Input|Action")
	TObjectPtr<UInputAction> IA_Interaction;
};
