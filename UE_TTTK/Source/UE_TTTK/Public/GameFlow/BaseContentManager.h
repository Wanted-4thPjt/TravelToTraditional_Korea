// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/ContentEntryComponent.h"
#include "Data/ContentConfig.h"
#include "BaseContentManager.generated.h"

class UContentEntryComponent;

UENUM(BlueprintType)
enum class EContentState : uint8
{
	Default = 0 UMETA(Hidden),
	Ready,
	Start,
	Playing,
	Finished
};

USTRUCT(BlueprintType)
struct UE_TTTK_API FContentParticipatingPlayerData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	APlayerController* playerController = nullptr;

	UPROPERTY(BlueprintReadWrite, meta=(EditCondition=bHasScore))
	int32 recordedScore = 0;
	UPROPERTY(BlueprintReadWrite, meta=(EditCondition=bHasTimeRecorder))
	float recordedTime = 0.f;
	
	/*UPROPERTY(BlueprintReadWrite)
	TMap<FName, FString> recordedCustomData; */
};

UCLASS()
class UE_TTTK_API ABaseContentManager : public AActor
{
	GENERATED_BODY()

public:
	ABaseContentManager();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void InitializeConfig();
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void InitializeContent(const TArray<APlayerController*>& inPlayers);
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void StartContent();
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void UpdateContent(const float& deltaTime);
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void EndContent();
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void ClearContent();
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void OnContentTimerExpired();
	
public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void ReturnPlayersToLobby();
	UFUNCTION()
	virtual void OnPlayerAction(APlayerController* actionPlayer, FName actionName, FVector inData);
	UFUNCTION(Server, Reliable)
	void NotifyContentFinished(FContentParticipatingPlayerData result);

private:
	void TeleportPlayersIntoContent();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	FContentConfig contentConfig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	FName contentRowName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UContentEntryComponent> ownerEntryComponent;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Server")
	EContentState contentState = EContentState::Default;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category="Server")
	int32 finishedPlayersCount;
	
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category="Server")
	TArray<FContentParticipatingPlayerData> contentPlayersData;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Timer")
	float contentTimeLimit = -1.f;  // if time limit is NONE, set this value to NEGATIVE.
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Timer")
	float remainingTime = 0.f;
	
private:
	FTimerHandle contentTimer;
	
};


