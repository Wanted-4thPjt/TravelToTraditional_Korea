#include "TTTK_GameState.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "UE_TTTKPlayerController.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"

ATTTK_GameState::ATTTK_GameState()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentTimeOfDay = 8.0f;
	LastBroadcastMinute = -1;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ATTTK_GameState::BeginPlay()
{
	Super::BeginPlay();

	CurrentTimeOfDay = DefaultStartTime;

	// 서버만 타이머 시작
	if (HasAuthority())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			// 시간 업데이트 타이머 (0.1초)
			World->GetTimerManager().SetTimer(
				TimeUpdateTimerHandle,
				this,
				&ATTTK_GameState::UpdateTimeOfDay,
				0.1f,
				true
			);

			// 델리게이트 브로드캐스트 타이머
			World->GetTimerManager().SetTimer(
				BroadcastTimerHandle,
				this,
				&ATTTK_GameState::BroadcastTimeChange,
				BroadcastInterval,
				true
			);
		}
	}
}

void ATTTK_GameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimeUpdateTimerHandle);
		World->GetTimerManager().ClearTimer(BroadcastTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void ATTTK_GameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATTTK_GameState, CurrentTimeOfDay);
}

// 시간 업데이트 (서버만 실행)
void ATTTK_GameState::UpdateTimeOfDay()
{
	if (!HasAuthority()) return;

	float DeltaTime = 0.1f;
	float InGameSeconds = DeltaTime * TimeScale;
	float InGameHours = InGameSeconds / 3600.0f;

	CurrentTimeOfDay += InGameHours;

	if (CurrentTimeOfDay >= 24.0f)
	{
		CurrentTimeOfDay -= 24.0f;
	}
}

// 델리게이트 브로드캐스트 (분 단위 중복 방지)
void ATTTK_GameState::BroadcastTimeChange()
{
	int32 Hour = FMath::FloorToInt(CurrentTimeOfDay);
	int32 Minute = FMath::FloorToInt((CurrentTimeOfDay - Hour) * 60.0f);

	if (Minute != LastBroadcastMinute)
	{
		LastBroadcastMinute = Minute;

		FTimeOfDayData TimeData;
		TimeData.TimeInHours = CurrentTimeOfDay;
		TimeData.Hour = Hour;
		TimeData.Minute = Minute;
		TimeData.NormalizedTime = CurrentTimeOfDay / 24.0f;
		TimeData.bIsDaytime = IsDaytime();

		OnTimeOfDayChanged.Broadcast(TimeData);
	}
}

void ATTTK_GameState::ReceiveChat(const FText& inputText)
{
	for (TObjectPtr<APlayerState> ps : PlayerArray)
	{
		if (AUE_TTTKPlayerController* pc = Cast<AUE_TTTKPlayerController>(ps->GetPlayerController()))
		{
			pc->Client_UpdateChat(inputText);
		}
	}
}

// 복제 콜백 (클라이언트)
void ATTTK_GameState::OnRep_CurrentTimeOfDay()
{
	int32 Hour = FMath::FloorToInt(CurrentTimeOfDay);
	int32 Minute = FMath::FloorToInt((CurrentTimeOfDay - Hour) * 60.0f);

	FTimeOfDayData TimeData;
	TimeData.TimeInHours = CurrentTimeOfDay;
	TimeData.Hour = Hour;
	TimeData.Minute = Minute;
	TimeData.NormalizedTime = CurrentTimeOfDay / 24.0f;
	TimeData.bIsDaytime = IsDaytime();

	OnTimeOfDayChanged.Broadcast(TimeData);
}

void ATTTK_GameState::GetCurrentTime(int32& OutHour, int32& OutMinute) const
{
	OutHour = FMath::FloorToInt(CurrentTimeOfDay);
	OutMinute = FMath::FloorToInt((CurrentTimeOfDay - OutHour) * 60.0f);
}

bool ATTTK_GameState::IsDaytime() const
{
	return CurrentTimeOfDay >= 6.0f && CurrentTimeOfDay < 20.0f;
}

// 시간 설정 (디버깅용)
void ATTTK_GameState::SetTimeOfDay(float NewTime)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Warning, TEXT("TTTK_GameState: Client cannot set time"));
		return;
	}

	NewTime = FMath::Clamp(NewTime, 0.0f, 24.0f);
	CurrentTimeOfDay = NewTime;

	int32 Hour, Minute;
	GetCurrentTime(Hour, Minute);

	FTimeOfDayData TimeData;
	TimeData.TimeInHours = CurrentTimeOfDay;
	TimeData.Hour = Hour;
	TimeData.Minute = Minute;
	TimeData.NormalizedTime = CurrentTimeOfDay / 24.0f;
	TimeData.bIsDaytime = IsDaytime();

	OnTimeOfDayChanged.Broadcast(TimeData);
	LastBroadcastMinute = Minute;
}

// 콘솔 명령어 자동 등록
class FTimeConsoleCommands
{
public:
	FTimeConsoleCommands()
	{
		IConsoleManager::Get().RegisterConsoleCommand(
			TEXT("Time.Set"),
			TEXT("Set time of day. Usage: Time.Set [0-24]"),
			FConsoleCommandWithWorldAndArgsDelegate::CreateStatic(&ATTTK_GameState::ConsoleCommand_SetTime),
			ECVF_Default
		);
	}
};

static FTimeConsoleCommands GTimeConsoleCommands;

// 콘솔 명령어: Time.Set
void ATTTK_GameState::ConsoleCommand_SetTime(const TArray<FString>& Args, UWorld* World)
{
	if (!World || Args.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Usage: Time.Set [0-24]"));
		return;
	}

	ATTTK_GameState* GameState = World->GetGameState<ATTTK_GameState>();
	if (!GameState)
	{
		UE_LOG(LogTemp, Warning, TEXT("TTTK_GameState not found"));
		return;
	}

	float NewTime = FCString::Atof(*Args[0]);
	GameState->SetTimeOfDay(NewTime);
}
