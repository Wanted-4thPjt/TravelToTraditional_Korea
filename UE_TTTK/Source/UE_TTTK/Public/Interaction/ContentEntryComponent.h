#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ContentEntrySettings.h"
#include "ContentEntryComponent.generated.h"

class USphereComponent;
class UWidgetComponent;
class AMainPlayer;
class ABaseContentManager;

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
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	// === Player Actions (UI/Input) ===
	#pragma region Player Actions
	
	UFUNCTION(BlueprintCallable, Category = "Content Entry")
	void RequestJoinLobby(AMainPlayer* player);

	UFUNCTION(BlueprintCallable, Category = "Content Entry")
	void RequestLeaveLobby(AMainPlayer* player);

	UFUNCTION(BlueprintCallable, Category = "Content Entry")
	void RequestStartContent(AMainPlayer* player);

	UFUNCTION(BlueprintCallable, Category = "Content Entry")
	void RequestCancelLobby(AMainPlayer* player);
	#pragma endregion Player Actions

	// === ContentManager Callback ===
	#pragma region ContentManager Callback
	
	UFUNCTION(BlueprintCallable, Category = "Content Entry")
	void OnContentFinished();
	#pragma endregion ContentManager Callback

	// === Getters ===
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


private:
	// === Server RPC ===
	#pragma region Server RPC
	
	UFUNCTION(Server, Reliable)
	void Server_RequestJoin(AMainPlayer* player);
	UFUNCTION(Server, Reliable)
	void Server_RequestLeave(AMainPlayer* player);
	UFUNCTION(Server, Reliable)
	void Server_RequestStart(AMainPlayer* requestingPlayer);
	UFUNCTION(Server, Reliable)
	void Server_RequestCancel(AMainPlayer* requestingPlayer);
	#pragma endregion Server RPC

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

	#pragma region Overlap Event
	UFUNCTION()
	void OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	void OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	#pragma endregion Overlap Event

	UFUNCTION(BlueprintCallable, Category = "Helper")
	bool IsServer() const {AActor* owner = GetOwner();
		return IsValid(owner) && owner->HasAuthority();}

public:
	UPROPERTY(BlueprintAssignable, Category = "Event")
	FOnLobbyStateChanged OnLobbyStateChanged;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Players")
	TArray<AMainPlayer*> playersInRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Players")
	TArray<AMainPlayer*> readyPlayers;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Players|Host")
	AMainPlayer* hostPlayer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Players|Host")
	bool bLobbyActive;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Range")
	TObjectPtr<USphereComponent> interactionSphere;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Range")
	float interactionRadius;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Content")
	TObjectPtr<ABaseContentManager> contentManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = "Content")
	bool bContentRunning = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setting")
	FContentEntrySettings settings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Setting")
	float maxWaitSeconds = 0.f;
	FTimerHandle lobbyTimer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Outline")
	TObjectPtr<UWidgetComponent> entryInfoWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Outline")
	int32 outlineStencilValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Outline")
	FLinearColor outlineColor;
};
