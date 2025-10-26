#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ContentEntrySettings.h"
#include "ContentEntryComponent.generated.h"

class UEntryInfoWidget;
class USphereComponent;
class UWidgetComponent;
class AMainPlayer;
class ABaseContentManager;
enum class EInteractableState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLobbyStateChanged, int32, currentPlayers, int32, maxPlayersLimit);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UContentEntryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UContentEntryComponent();

protected:
	// Called when the game starts
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// === Player Actions (UI/Input) ===
	#pragma region Player Actions
	UFUNCTION()
	void RequestEntry(AMainPlayer* player);
	
	UFUNCTION(BlueprintCallable, Category = "Content Entry")
	void RequestJoinLobby(AMainPlayer* player);

	UFUNCTION(BlueprintCallable, Category = "Content Entry")
	void RequestLeaveLobby(AMainPlayer* player);

	UFUNCTION(BlueprintCallable, Category = "Content Entry")
	void RequestStartContent(AMainPlayer* player);

	UFUNCTION(BlueprintCallable, Category = "Content Entry")
	void RequestCancelLobby(AMainPlayer* player);

	UFUNCTION(BlueprintCallable, Category = "Content Entry")
	void RequestFinishContent();
	#pragma endregion Player Actions
	
	#pragma region Getter
	UFUNCTION(BlueprintPure, Category = "Content Entry|Getter")
	bool IsLobbyActive() const { return bLobbyActive; }

	UFUNCTION(BlueprintPure, Category = "Content Entry|Getter")
	bool IsContentRunning() const { return bContentRunning; }

	UFUNCTION(BlueprintPure, Category = "Content Entry|Getter")
	AMainPlayer* GetHostPlayer() const { return hostPlayer; }

	UFUNCTION(BlueprintPure, Category = "Content Entry|Getter")
	int32 GetCurrentPlayerCount() const { return readyPlayers.Num(); }

	UFUNCTION(BlueprintPure, Category = "Content Entry|Getter")
	int32 GetOutlineDepthStencilValue() const {return outlineStencilValue;}
	#pragma endregion Getter

protected:
	UFUNCTION(BlueprintPure, Category = "Helper")
	bool IsServer() const {AActor* owner = GetOwner();
		return IsValid(owner) && owner->HasAuthority();}
private:
	// === Multicast RPC ===
	#pragma region Multicast RPC
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateLobbyState();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnContentStarted();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnContentFinished();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnLobbyCancelled();
	#pragma endregion Multicast RPC

	// === Internal Logic ===
	#pragma region Internal
	bool ValidateJoinRequest(AMainPlayer* player);
	bool ValidateStartRequest(AMainPlayer* requestingPlayer);
	void AddPlayerToLobby(AMainPlayer* player);
	void RemovePlayerFromLobby(AMainPlayer* player);
	void StartContentInternal();
	void ResetLobby();
	void OnLobbyTimeout();
	void ReassignHost();
	#pragma endregion Internal

public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnLobbyStateChanged OnLobbyStateChanged;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setting")
	FContentEntrySettings settings;
	
	FTimerHandle lobbyTimer;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Players")
	TArray<AMainPlayer*> readyPlayers;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Players|Host")
	AMainPlayer* hostPlayer;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Players|Host")
	bool bLobbyActive;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Content")
	TObjectPtr<UBaseContentManager> contentManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Content")
	bool bContentRunning = false;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Outline")
	TSubclassOf<UEntryInfoWidget> entryInfoWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Outline")
	int32 outlineStencilValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Outline")
	FLinearColor outlineColor;
};
