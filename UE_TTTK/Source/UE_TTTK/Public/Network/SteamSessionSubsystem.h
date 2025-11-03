// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SteamSessionSubsystem.generated.h"

class ULoadingWidget;
class IOnlineSubsystem;
class USteamSessionSettings;
class FOnlineSessionSettings;
DECLARE_DELEGATE_OneParam(FOnFindSessions, const TArray<FOnlineSessionSearchResult>&);

/**
 *
 */
UCLASS()
class UE_TTTK_API USteamSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USteamSessionSubsystem();
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

public:
	// if max players count <= 0 : it sets by map default
	void CreateSession(const FString& mapName, const FString& displayName, int32 maxPlayerCount);
	
	void FindSession();
	void JoinSession(int32 sessionIndex);
	
	void OpenSessionEntry();
	void CloseSessionEntry();
	void RefreshSessionInfo(const int32& deltaParticipantsValue);
	
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
	void OnCompleteUpdateSession(FName SessionName, bool bWasSuccess);
	UFUNCTION()
	void OnCompleteDestroySession(FName SessionName, bool bWasSuccess);

	void BindCloudDelegates(IOnlineSubsystem* onlineSubsys);
	
	void OnCompleteReadUserCloudData(bool bWasSuccess, const FUniqueNetId& netId, const FString& FileName);
	UFUNCTION()
	bool LoadUserSettingsFromCloud(const FString& FilePath);
	UFUNCTION()
	bool SaveUserSettingsToCloud(const FString& FilePath);

protected:
	FOnlineSessionSettings sessionSettings;
	TSharedPtr<FOnlineSessionSearch> sessionSearch;
	IOnlineSessionPtr sessionInterface;
	UPROPERTY()
	const USteamSessionSettings* steamMapSettings;

	TPair<FName, FString> hostNamePair = {"HOST_NAME", ""};
	TPair<FName, FString> displayNamePair = {"DP_NAME", ""};
	TPair<FName, FString> mapNamePair = {"MAP_NAME", ""};
	TPair<FName, int32> participantsCountPair = {"COUNT", 0};

private:
	UPROPERTY()
	TSubclassOf<ULoadingWidget> loadingWidgetFactory;
	UPROPERTY()
	TObjectPtr<ULoadingWidget> loadingWidget;
	FTimerHandle loadingWidgetTimer;
	
	//TSharedPtr<IStreamableProgressManager> loadingManager;
};
