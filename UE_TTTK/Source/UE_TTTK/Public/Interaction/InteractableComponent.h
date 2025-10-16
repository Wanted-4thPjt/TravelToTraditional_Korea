#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/InteractableFeedbackSettings.h"
#include "InteractableComponent.generated.h"

class USphereComponent;
class UWidgetComponent;

UENUM(BlueprintType, meta=(BitFlags))
enum class EInteractableState : uint8
{
	Default UMETA(Hidden),
	OutOfBound = 1 << 1, // 범위에서 벗어났을때
	InRange = 1 << 2, // 범위에 들어왔을때
	UnFocused = 1 << 3, // 바라본 상태에서 --> 안바라보게됨.  4~6번 => inRange가 선행이어야함. 
	Focused = 1 << 4, // 범위에 있으면서 바라보는 상태
	Interacting = 1 << 5 // 내가 FOCUS 하면서 상호작용 키를 누른경우
};
ENUM_CLASS_FLAGS(EInteractableState)
class AMainPlayer;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChangeState, APlayerController*, playerController, const EInteractableState&, newInteractableState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestInteraction, APawn*, player);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractableComponent();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintPure, Category="Interactable|Each Client")
	FORCEINLINE EInteractableState GetState() const {return clientState;}
	UFUNCTION(BlueprintPure, Category="Interactable|Each Client")
	bool IsInteractable() const {return clientState == EInteractableState::Focused;}
	UFUNCTION(BlueprintPure, Category="Interactable|Each Client")
	bool IsInteracting() const {return clientState == EInteractableState::Interacting;}
	UFUNCTION(BlueprintPure, Category="Interactable|All Client")
	bool CanPossess() const {return feedbackSettings.IsNetworkOn() && possessingPlayers.Num() < feedbackSettings.availableInteractionCount;}

	UFUNCTION(BlueprintCallable, Category="Interactable|Each Client")
	void TryDeactivateInteractable(APlayerController* playerController);
	UFUNCTION(BlueprintCallable, Category="Interactable|Each Client")
	void TryActivateInteractable(APlayerController* playerController);
	// @Param : bPossess : 상호작용 결과가 동기화되어야 한다. 
	UFUNCTION(BlueprintCallable, Category="Interactable|Each Client")
	void TryInteract(APlayerController* playerController);
	UFUNCTION(NetMulticast, Reliable, Category="Interactable|Multicast")
	void Multicast_TryInteract(AMainPlayer* player);
	UFUNCTION(BlueprintCallable, Category="Interactable|Each Client")
	void FinishInteracting(APlayerController* Player, const EInteractableState& newState);
	UFUNCTION(NetMulticast, Reliable, Category="Interactable|Multicast")
	void Multicast_FinishInteracting(AMainPlayer* player);

	UFUNCTION(BlueprintCallable, Category="Interactable|Visual")
	void Client_UpdateVisuals(APlayerController* playerController);

protected:	
	UFUNCTION(BlueprintCallable, Category="Interactable|Each Client")
	void OutOfInteractableRange(APlayerController* Player);
	UFUNCTION(BlueprintCallable, Category="Interactable|Each Client")
	void InInteractableRange(APlayerController* Player);
	
	#pragma region Overlap Event
	UFUNCTION()
	void OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	#pragma endregion Overlap Event

	#pragma region Effects
	bool UpdateAvailablePrimitiveComponents();
	void UpdateOutline();
	void UpdateWidget();
	void PlaySound(USoundBase* sound);
	void PlayEffect(UParticleSystem* effect);
	void PlayEffect(UNiagaraSystem* effect);
	#pragma endregion Effects

	UFUNCTION(BlueprintPure)
	bool IsServer() const {AActor* owner = GetOwner();
		return IsValid(owner) && owner->HasAuthority();}
	
public:
	UPROPERTY(BlueprintAssignable, Category="Interactable|Event")
	FOnChangeState onChangeState;
	UPROPERTY(BlueprintAssignable, Category="Interactable|Event")
	FOnRequestInteraction onRequestInteraction;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> interactionGuideComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Range")
	TObjectPtr<USphereComponent> interactionSphere;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Range")
	float interactionRadius = 300.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Server")
	TArray<AMainPlayer*> possessingPlayers;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableFeedback")
	EInteractableState clientState = EInteractableState::Default;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableFeedback")
	FInteractableFeedbackSettings feedbackSettings;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Client", meta=(AllowPrivateAccess=true))
	AMainPlayer* playerInRange;
	
};
