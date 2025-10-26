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
	Focused = 1 << 3, // 범위에 있으면서 바라보는 상태
	UnFocused = 1 << 4
};
ENUM_CLASS_FLAGS(EInteractableState)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClientInteraction, APlayerController*, playerController);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMultiInteraction, APawn*, player);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractableComponent();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif //WITH_EDITOR

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintPure, Category="Interactable|Each Client")
	FORCEINLINE EInteractableState GetState() const {return clientState;}
	UFUNCTION(BlueprintPure, Category="Interactable|Each Client")
	FORCEINLINE bool IsInteractable() const {return clientState == EInteractableState::Focused;}
	UFUNCTION(BlueprintPure, Category="Interactable|All Client")
	FORCEINLINE bool IsMultiPlayable() const {return feedbackSettings.IsNetworkOn();}

	UFUNCTION(BlueprintCallable, Category="Interactable|Each Client")
	void TryChangeState(APlayerController* playerController, EInteractableState newState);
	UFUNCTION(BlueprintCallable, Category="Interactable|Each Client")
	void TryInteract(APlayerController* playerController);
	UFUNCTION(NetMulticast, Reliable, Category="Interactable|Multicast")
	void Multicast_TryInteract(APawn* player);
	

protected:	
	UFUNCTION(BlueprintCallable, Category="Interactable|Each Client")
	void FinishInteracting(APlayerController* Player, bool bSuccess);
	UFUNCTION(BlueprintCallable, Category="Interactable|Visual")
	void UpdateVisuals(APlayerController* playerController);
	UFUNCTION(BlueprintCallable, Category="Interactable|Effect")
	void PlayEffects(bool bSuccess);
	
	#pragma region Overlap Event
	UFUNCTION()
	void OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	#pragma endregion Overlap Event

	#pragma region Effects
	bool UpdateAvailablePrimitiveComponents();
	void PlaySound(USoundBase* sound);
	void PlayEffect(UParticleSystem* effect);
	void PlayEffect(UNiagaraSystem* effect);
	#pragma endregion Effects

	UFUNCTION(BlueprintPure)
	bool IsServer() const {AActor* owner = GetOwner();
		return IsValid(owner) && owner->HasAuthority();}
	
public:
	UPROPERTY(BlueprintAssignable, Category="Interactable|Event")
	FOnClientInteraction OnClientInteraction;
	UPROPERTY(BlueprintAssignable, Category="Interactable|Event")
	FOnMultiInteraction OnMultiInteraction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "InteractableFeedback")
	FInteractableFeedbackSettings feedbackSettings;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DynamicComponents|UI")
	TObjectPtr<UWidgetComponent> interactionGuideComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DynamicComponents|Range")
	TObjectPtr<USphereComponent> interactionSphere;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DynamicComponents|Range")
	float interactionRadius = 300.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "InteractableFeedback")
	EInteractableState clientState = EInteractableState::Default;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Client", meta=(AllowPrivateAccess=true))
	APawn* playerInRange = nullptr;
	
};
