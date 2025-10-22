// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
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

	FOnFindSessions OnFindSessions;

private:
	UFUNCTION()
	void OnCompleteCreateSession(FName inSessionName, bool bWasSuccess);
	UFUNCTION()
	void OnCompleteFindSession(bool bWasSuccess);
	void OnCompleteJoinSession(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

protected:
	TSharedPtr<FOnlineSessionSearch> sessionSearch;
	IOnlineSessionPtr sessionInterface;
	const USteamSessionSettings* steamMapSettings;

	FString displayNamePrefix = "===";
	FString sessionMapAsset;
};
