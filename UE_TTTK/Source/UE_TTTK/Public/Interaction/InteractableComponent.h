#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/InteractableFeedbackSettings.h"
#include "InteractableComponent.generated.h"

class UWidgetComponent;

UENUM(BlueprintType)
enum class EInteractableState : uint8
{
	Default UMETA(Hidden),
	OutOfBound,
	Inactive,
	Active,
	Interacting
};
class AMainPlayer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeState, const EInteractableState&, newInteractableState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractRequest, AMainPlayer*, player);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractableComponent();

protected:
	virtual void BeginPlay() override;

public:
	
	UFUNCTION(BlueprintPure, Category="Interactable|Getter")
	FORCEINLINE EInteractableState GetState() const {return state;}
	UFUNCTION(BlueprintPure, Category="Interactable|Getter")
	bool IsInteractable() const {return state == EInteractableState::Active;}
	UFUNCTION(BlueprintPure, Category="Interactable|Getter")
	bool IsInteracting() const {return state == EInteractableState::Interacting;}

	UFUNCTION(BlueprintCallable, Category="Interactable")
	void OutOfBound();
	UFUNCTION(BlueprintCallable, Category="Interactable")
	void ActivateInteractable();
	UFUNCTION(BlueprintCallable, Category="Interactable")
	void DeactivateInteractable();
	UFUNCTION(BlueprintCallable, Category="Interactable")
	bool TryInteract(AMainPlayer* player);
	UFUNCTION(BlueprintCallable, Category="Interactable")
	void FinishInteracting(const EInteractableState& newState);

protected:
	void UpdateVisuals();
	void UpdateOutline();
	void UpdateWidget();
	void PlaySound(USoundBase* sound);
	void PlayEffect(UParticleSystem* effect);
	void PlayEffect(UNiagaraSystem* effect);
	
public:
	UPROPERTY(BlueprintAssignable, Category="Interactable|Event")
	FOnInteractRequest onInteract;
	UPROPERTY(BlueprintAssignable, Category="Interactable|Event")
	FOnChangeState onChangeState;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableFeedback")
	EInteractableState state = EInteractableState::Default;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableFeedback")
	FInteractableFeedbackSettings feedbackSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	TObjectPtr<UWidgetComponent> interactionGuideComponent;

private:
	
};
