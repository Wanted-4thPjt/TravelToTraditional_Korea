// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Content/BaseContentComponent.h"
#include "GameFramework/Actor.h"
#include "Interaction/ContentEntryComponent.h"
#include "Data/ContentConfig.h"
#include "BaseContentManager.generated.h"

class UContentEntryComponent;
class UBaseContentComponent;  // Content별 Input Component
class UInputMappingContext;

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

	UPROPERTY(VisibleAnywhere)
	FTransform prevTransform;

	UPROPERTY(BlueprintReadWrite)
	AActor* originalViewTarget = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 recordedScore = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float recordedTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 rank = -1;

	// Custom 데이터 저장 (String 타입)
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> recordedCustomValues;

	// Custom 데이터 저장 (Int32 타입 - Key-Value)
	UPROPERTY(BlueprintReadWrite)
	TMap<FString, int32> recordedCustomInts;
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

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	TSubclassOf<UBaseContentComponent> contentInputComponentClass;

	/** Input Context 우선순위 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Input")
	int32 inputContextPriority = 1;

	/** 이 Content가 커스텀 Input을 사용하는가? */
	UFUNCTION(BlueprintPure, Category="Content|Input")
	bool HasCustomInput() const
	{
		return contentInputComponentClass != nullptr;
	}

	FContentConfig GetConfig() const  { return contentConfig; }

	/** Player에게 Input 활성화 요청 */
	UFUNCTION(BlueprintCallable, Category="Content|Input")
	void ActivateInputForPlayer(APlayerController* player);

	/** Player에게 Input 비활성화 요청 */
	UFUNCTION(BlueprintCallable, Category="Content|Input")
	void DeactivateInputForPlayer(APlayerController* player);

	/** 모든 플레이어에게 Input 활성화 */
	UFUNCTION(BlueprintCallable, Category="Content|Input")
	void ActivateInputForAllPlayers();

	/** 모든 플레이어에게 Input 비활성화 */
	UFUNCTION(BlueprintCallable, Category="Content|Input")
	void DeactivateInputForAllPlayers();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	FName contentRowName;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category="Data")
	FContentConfig contentConfig;
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Server")
	EContentState contentState = EContentState::Default;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadWrite, Category="Server|Round")
	int32 currentRound = -1;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Server|Round")
	int32 totalRounds = 0;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category="Server|Round")
	int32 currentPlayerIndex = -1;

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


