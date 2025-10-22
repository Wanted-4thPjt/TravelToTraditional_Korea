// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetGameInstance.generated.h"

// 세션 검색 완료시 호출되는 함수 등록하는 Delegate
DECLARE_DELEGATE_TwoParams(FFindComplete, int32, FString);

UCLASS()
class UE_TTTK_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// Begin Play
	virtual void Init() override;

	// 세션의 모든 처리를 진행 하는 객체
	IOnlineSessionPtr sessionInterface;

	// 세션 생성 관련
	// 현재 세션 이름
	FName currSessionName;
	// 세션 생성 함수
	UFUNCTION(BlueprintCallable)
	void CreateMySession(FString displayName, int32 playerCount);
	// 세션 생성 완료 함수
	void OnCreateSessionComplete(FName sessionName, bool bWasSuccessful);

	// 세션 조회 관련
	// 세션 조회할 때 사용하는 객체
	TSharedPtr<FOnlineSessionSearch> sessionSearch;
	// 세션 조회 완료시 세션 갯수 만큼 호출하는 Delegate
	FFindComplete onFindComplete;
	// 세션 조회 함수
	UFUNCTION(BlueprintCallable)
	void FindOtherSession();
	// 세션 조회 완료 함수
	void OnFindSessionComplete(bool bWasSuccessful);

	// 세션 참여 관련
	// 세션 참여 함수
	UFUNCTION(BlueprintCallable)
	void JoinOtherSession(int32 sessionIdx);
	// 세션 참여 완료 함수
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);

	
};


