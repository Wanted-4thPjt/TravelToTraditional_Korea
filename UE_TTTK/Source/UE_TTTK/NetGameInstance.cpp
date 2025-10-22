// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

void UNetGameInstance::Init()
{
	Super::Init();

	// 현재 사용하는 서브시스템을 가져오자
	/*IOnlineSubsystem* subsys = Online::GetSubsystem(GetWorld());
	if (subsys)
	{
		// 서브시스템의 인터페이스를 가져오자.
		sessionInterface = subsys->GetSessionInterface();
		// 세션 생성 성공시 호출되는 함수 등록
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
			this, &UNetGameInstance::OnCreateSessionComplete);
		// 세션 조회 성공시 호출되는 함수 등록
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
			this, &UNetGameInstance::OnFindSessionComplete);
		// 세션 참여 성공시 호출되는 함수 등록
		sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(
			this, &UNetGameInstance::OnJoinSessionComplete);
	}*/
}

void UNetGameInstance::CreateMySession(FString displayName, int32 playerCount)
{
	// 세션을 만들기 위한 옵션 담을 변수
	FOnlineSessionSettings sessionSettings;
	// 현재 사용중인 서브시스템 이름 가져오자.
	FName subsysName = Online::GetSubsystem(GetWorld())->GetSubsystemName();
	// 만약에 서브시스템이 이름이 NULL 이면 Lan 을 이용하게 설정
	sessionSettings.bIsLANMatch = subsysName.IsEqual(FName(TEXT("NULL")));
	UE_LOG(LogTemp, Warning, TEXT("서브시스템 : %s"), *subsysName.ToString());

	// Steam 에선 필수 (bUseLobbiesIfAvailable, bUsesPresence)
	// Lobby 사용 여부 설정
	sessionSettings.bUseLobbiesIfAvailable = true;
	// 친구 상태를 확인할 수 있는 여부
	sessionSettings.bUsesPresence = true;

	// 세션 검색 허용 여부
	sessionSettings.bShouldAdvertise = true;
	// 세션 최대 참여 인원 설정
	sessionSettings.NumPublicConnections = playerCount;
	// 커스텀 정보
	sessionSettings.Set(FName("DP_NAME"), displayName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	//sessionSettings.Set(FName("MAP_IDX"), 1, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// sessionSettings 이용해서 세션 생성
	FUniqueNetIdPtr netId =
		GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();
	sessionInterface->CreateSession(*netId, FName(displayName), sessionSettings);	
	//sessionInterface->CreateSession(0, FName(displayName), sessionSettings);
}

void UNetGameInstance::OnCreateSessionComplete(FName sessionName,
	bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 생성 성공"), *sessionName.ToString());
		// 총 쏘는 맵 으로 이동
		GetWorld()->ServerTravel(TEXT("/Game/VaVamVa/NewMap?listen"));
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] 세션 생성 실패"), *sessionName.ToString());
	}
}

void UNetGameInstance::FindOtherSession()
{
	UE_LOG(LogTemp, Warning, TEXT("세션 조회 시작"));
	// sessionSearch 만들자.
	sessionSearch = MakeShared<FOnlineSessionSearch>();
	// 현재 사용중인 서브시스템 이름 가져오자.
	FName subsysName = Online::GetSubsystem(GetWorld())->GetSubsystemName();
	// 만약에 서브시스템이 이름이 NULL 이면 Lan 을 이용하게 설정
	sessionSearch->bIsLanQuery = subsysName.IsEqual(FName(TEXT("NULL")));
	// 어떤 옵션을 기준으로 검색
	sessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	//sessionSearch->QuerySettings.Set(FName("DP_NAME"), FName("Wanted"), EOnlineComparisonOp::Equals);
	// 검색 갯수
	sessionSearch->MaxSearchResults = 100;
	// 위 설정을 가지고 세션 검색
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void UNetGameInstance::OnFindSessionComplete(bool bWasSuccessful)
{
	UE_LOG(LogTemp, Warning, TEXT("세션 조회 끝"));
	if (bWasSuccessful)
	{
		// 검색된 세션 결과들
		auto results = sessionSearch->SearchResults;
		for (int32 i = 0; i < results.Num(); i++)
		{
			// 방 제목 이름 담을 변수
			FString displayName;
			results[i].Session.SessionSettings.Get(FName(TEXT("DP_NAME")), displayName);
			UE_LOG(LogTemp, Warning, TEXT("세션 - %i, 이름 : %s"), i, *displayName);
			// onFindComplete 에 들어있는 함수 실행
			onFindComplete.ExecuteIfBound(i, displayName);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("세션 조회 실패"));
	}

	onFindComplete.ExecuteIfBound(-1, FString());
}

void UNetGameInstance::JoinOtherSession(int32 sessionIdx)
{
	// 검색된 세션 결과들
	auto results = sessionSearch->SearchResults;
	// 5.5 이후 부터 바꼈다...
	results[sessionIdx].Session.SessionSettings.bUseLobbiesIfAvailable = true;
	results[sessionIdx].Session.SessionSettings.bUsesPresence = true;

	// 세션 이름 가져오자.
	FString displayName;
	results[sessionIdx].Session.SessionSettings.Get(FName(TEXT("DP_NAME")), displayName);

	// 세션 참여
	sessionInterface->JoinSession(0, FName(displayName), results[sessionIdx]);
}

void UNetGameInstance::OnJoinSessionComplete(FName sessionName,
	EOnJoinSessionCompleteResult::Type result)
{
	// 만약 참여 성공 했다면
	if (result == EOnJoinSessionCompleteResult::Success)
	{
		// 서버가 만들어 놓은 세션 URL 을 얻어오자.
		FString url;
		sessionInterface->GetResolvedConnectString(sessionName, url);
		UE_LOG(LogTemp, Warning, TEXT("URL : %s"), *url);
		// 서버가 있는 맵으로 이동 (최초 1번)
		APlayerController* pc = GetWorld()->GetFirstPlayerController();
		pc->ClientTravel(url, TRAVEL_Absolute);
	}
}

