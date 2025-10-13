// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SteamSessionSubsystem.generated.h"

class FOnlineSessionSearch;
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
	void CreateSession(FName mapName, int32 maxPlayerCount = 0);
	void FindSession(int32 maxSearchResult = 10);
	void JoinSession();

private:
	UFUNCTION()
	void OnCompleteCreateSession(FName inSessionName, bool bWasSuccess);
	UFUNCTION()
	void OnCompleteFindSession(bool bWasSuccess);

protected:
	TSharedPtr<FOnlineSessionSearch> sessionSearch;
};
