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
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE AActor* GetFocusedActor() {return focusingActor;}
	UFUNCTION(BlueprintCallable)
	void Interaction();
	UFUNCTION()
	void FocusInteractableActor(const FHitResult& hitResult);

protected:
	UFUNCTION(Server, Reliable)
	void Server_Interact(UInteractableComponent* interactable);

	UFUNCTION(Server, Reliable)
	void Server_Focus(AActor* focusedActor);

protected:
	UPROPERTY(Replicated)
	TObjectPtr<AActor> focusingActor;
};
