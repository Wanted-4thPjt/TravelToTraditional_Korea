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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	APlayerController* playerController = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	AActor* originalViewTarget = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 recordedScore = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float recordedTime = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	TMap<FName, FString> recordedCustomData;
};

UCLASS()
class UE_TTTK_API UBaseContentManager : public UObject
{
	GENERATED_BODY()

public:
	UBaseContentManager();

protected:
	virtual void PostLoad() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void InitializeConfig();
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void InitializeContent(const TArray<APlayerController*>& inPlayers);
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void StartContent();
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void UpdateContent(const float& deltaTime);
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void RoundStart();
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void RoundUpdate();
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void RoundEnd();
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void EndContent();
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void ClearContent();

protected:
	UFUNCTION(BlueprintNativeEvent, Category="LifeCycle")
	void OnUpdateTimer();

	UFUNCTION()
	void ReturnPlayersToLobby();
	UFUNCTION()
	virtual void OnPlayerAction(APlayerController* actionPlayer, FName actionName, FVector inData);
	UFUNCTION()
	void ContentFinished(const FContentParticipatingPlayerData& result);

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
	int32 finishedPlayersCount = 0;
	
	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category="Server")
	TArray<FContentParticipatingPlayerData> contentPlayersData;

	// Recommendation : Using with Enum for Each Camera Role
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Camera")
	TArray<AActor*> contentCameras;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category="Timer")
	float startTime = 0.f;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Timer")
	float remainingTime = 0.f;
	
private:
	FTimerHandle contentTimer;
	
};


