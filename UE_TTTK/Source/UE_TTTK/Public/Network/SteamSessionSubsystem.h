// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/StreamableManager.h"
#include "SteamSessionSubsystem.generated.h"

class USteamSessionSettings;
DECLARE_DELEGATE_OneParam(FOnFindSessions, const TArray<FOnlineSessionSearchResult>&);

/**
 *
 */
UCLASS()
class UE_TTTK_API USteamSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	// if max players count <= 0 : it sets by map default
	void CreateSession(const FString& mapName, const FString& displayName, int32 maxPlayerCount);
	UFUNCTION(BlueprintCallable)
	void FindSession();
	void JoinSession(int32 sessionIndex);
	bool DestroySession();
	FString GetSteamNickName() const;

	FOnFindSessions OnFindSessions;

	FORCEINLINE FName GetHostNameKey() const {return hostNamePair.Key;}
	FORCEINLINE FName GetDisplayNameKey() const {return displayNamePair.Key;}
	FORCEINLINE FName GetMapNameKey() const {return mapNamePair.Key;}
	
private:
	UFUNCTION()
	void OnCompleteCreateSession(FName inSessionName, bool bWasSuccess);
	UFUNCTION()
	void OnCompleteFindSession(bool bWasSuccess);
	void OnCompleteJoinSession(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnCompleteDestroySession(FName SessionName, bool bWasSuccess);

protected:
	TSharedPtr<FOnlineSessionSearch> sessionSearch;
	IOnlineSessionPtr sessionInterface;
	UPROPERTY()
	const USteamSessionSettings* steamMapSettings;

	TPair<FName, FString> hostNamePair = {"HOST_NAME", ""};
	TPair<FName, FString> displayNamePair = {"DP_NAME", ""};
	TPair<FName, FString> mapNamePair = {"MAP_NAME", ""};

	//TSharedPtr<IStreamableProgressManager> loadingManager;
};
