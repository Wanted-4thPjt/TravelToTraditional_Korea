#include "Interaction/ContentEntryComponent.h"

#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

#include "MainPlayer.h"
#include "GameFlow/BaseContentManager.h"
#include "Interaction/InteractableComponent.h"
#include "UI/EntryInfoWidget.h"


UContentEntryComponent::UContentEntryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
	ComponentTags.Add("Entry");
	
	hostPlayer = nullptr;
	bLobbyActive = false;
	bContentRunning = false;
	
	contentManager = nullptr;
}

void UContentEntryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UContentEntryComponent, readyPlayers);
	DOREPLIFETIME(UContentEntryComponent, hostPlayer);
	DOREPLIFETIME(UContentEntryComponent, bLobbyActive);
	DOREPLIFETIME(UContentEntryComponent, bContentRunning);
}

void UContentEntryComponent::InitializeComponent()
{
	Super::InitializeComponent();

}

void UContentEntryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(GetOwner())) {return;}
	
	if (settings.contentManagerClass)
	{
		contentManager = NewObject<UBaseContentManager>(this,
			settings.contentManagerClass, settings.contentName,
			EObjectFlags::RF_Transient | EObjectFlags::RF_Transactional
		);
	}
	else if (settings.contentName != "")
	{
		
	}
	
}

void UContentEntryComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bContentRunning && IsValid(contentManager))
	{
		contentManager->UpdateContent(DeltaTime);
	}
}

#pragma region Player Actions
void UContentEntryComponent::RequestEntry(AMainPlayer* player)
{
	if (settings.startCondition == EStartCondition::Auto)
	{
		RequestStartContent(player);
		return;
	}
	RequestJoinLobby(player);
}

void UContentEntryComponent::RequestJoinLobby(AMainPlayer* player)
{
	if (!IsServer()) return;
	if (!ValidateJoinRequest(player))
	{
		// TODO: Fail to Join UI or Effects
		return;
	}

	AddPlayerToLobby(player);
	Server_UpdateLobbyState();
}

void UContentEntryComponent::RequestLeaveLobby(AMainPlayer* player)
{
	if (!IsServer()) return;
	if (!IsValid(player)) return;

	RemovePlayerFromLobby(player);
	Server_UpdateLobbyState();
}

void UContentEntryComponent::RequestStartContent(AMainPlayer* player)
{
	if (!ValidateStartRequest(player))
	{
		UE_LOG(LogTemp, Warning, TEXT("Start request validation failed"));
		return;
	}

	StartContentInternal();
	Server_OnContentStarted();
}

void UContentEntryComponent::RequestCancelLobby(AMainPlayer* player)
{
	if (!IsServer()) return;
	if (!IsValid(player)) return;

	if (hostPlayer != player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Only host can cancel lobby"));
		return;
	}

	ResetLobby();
	Server_OnLobbyCancelled();
}
#pragma endregion Player Actions


#pragma region ContentManager Callback

void UContentEntryComponent::RequestFinishContent()
{
	if (!IsServer()) return;

	bContentRunning = false;

	for (AMainPlayer* player : readyPlayers)
	{
		if (!IsValid(player)) continue;
		// TODO: 플레이어를 원래 위치로 이동
	}

	ResetLobby();
	Server_OnContentFinished();
}
#pragma endregion ContentManager Callback

#pragma region Multicast RPC

void UContentEntryComponent::Server_UpdateLobbyState_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Lobby state updated: %d/%d"), readyPlayers.Num(), settings.maxPlayers);
}

void UContentEntryComponent::Server_OnContentStarted_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Content started - Playing start effects"));
	TArray<APlayerController*> playerControllers;
	for (AMainPlayer* player : readyPlayers)
	{
		playerControllers.Add(player->GetController<APlayerController>());
	}
	contentManager->InitializeContent(playerControllers);
	// TODO: 시작 연출, UI 변경 등
}

void UContentEntryComponent::Server_OnContentFinished_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Content finished - Showing results"));
	
}

void UContentEntryComponent::Server_OnLobbyCancelled_Implementation()
{
	OnLobbyStateChanged.Broadcast(0, settings.maxPlayers);
	UE_LOG(LogTemp, Log, TEXT("Lobby cancelled"));
}
#pragma endregion Multicast RPC


#pragma region Internal

bool UContentEntryComponent::ValidateJoinRequest(AMainPlayer* player)
{
	if (!IsValid(player))
	{
		UE_LOG(LogTemp, Warning, TEXT("ValidateJoinRequest: Invalid player"));
		return false;
	}

	if (readyPlayers.Contains(player))
	{
		UE_LOG(LogTemp, Warning, TEXT("ValidateJoinRequest: Player already in lobby"));
		return false;
	}

	if (readyPlayers.Num() >= settings.maxPlayers)
	{
		UE_LOG(LogTemp, Warning, TEXT("ValidateJoinRequest: Lobby is full (%d/%d)"), readyPlayers.Num(), settings.maxPlayers);
		return false;
	}

	if (bContentRunning)
	{
		UE_LOG(LogTemp, Warning, TEXT("ValidateJoinRequest: Content already running"));
		return false;
	}

	return true;
}

bool UContentEntryComponent::ValidateStartRequest(AMainPlayer* requestingPlayer)
{
	if (!IsValid(requestingPlayer))
	{
		UE_LOG(LogTemp, Warning, TEXT("ValidateStartRequest: Invalid requesting player"));
		return false;
	}
	
	if (settings.startCondition == EStartCondition::Manual && hostPlayer != requestingPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("ValidateStartRequest: Only host can start (Manual mode)"));
		return false;
	}

	if (readyPlayers.Num() < settings.minPlayers)
	{
		UE_LOG(LogTemp, Warning, TEXT("ValidateStartRequest: Not enough players (%d/%d)"), readyPlayers.Num(), settings.minPlayers);
		return false;
	}

	if (bContentRunning)
	{
		UE_LOG(LogTemp, Warning, TEXT("ValidateStartRequest: Content already running"));
		return false;
	}

	// Ready 모드일 때 모든 플레이어가 Ready 상태인지 체크
	if (settings.startCondition == EStartCondition::Ready)
	{
		// TODO: Player에 bIsReady 변수 추가 후 구현
		/*
		for (AMainPlayer* player : readyPlayers)
		{
			if (!player || !player->bIsReady)
			{
				UE_LOG(LogTemp, Warning, TEXT("ValidateStartRequest: Not all players are ready"));
				return false;
			}
		}
		*/
	}

	return true;
}

void UContentEntryComponent::AddPlayerToLobby(AMainPlayer* player)
{
	if (!IsServer()) return;
	if (!IsValid(player)) return;

	readyPlayers.Add(player);

	// 첫 참가자면 Host로 지정
	if (readyPlayers.Num() == 1)
	{
		hostPlayer = player;
		bLobbyActive = true;

		// 타이머 시작 (maxWaitSeconds > 0일 경우)
		if (settings.maxWaitSeconds > 0.0f)
		{
			if (UWorld* world = GetWorld())
			{
				world->GetTimerManager().SetTimer(
					lobbyTimer,
					this,
					&UContentEntryComponent::OnLobbyTimeout,
					settings.maxWaitSeconds,
					false
				);
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Player %s is now the host"), *player->GetName());
	}

	UE_LOG(LogTemp, Log, TEXT("Player %s joined lobby (%d/%d)"), *player->GetName(), readyPlayers.Num(), settings.maxPlayers);
	
	if (settings.startCondition == EStartCondition::Auto && readyPlayers.Num() >= settings.minPlayers)
	{
		UE_LOG(LogTemp, Log, TEXT("Auto-starting content (Auto mode)"));
		StartContentInternal();
		Server_OnContentStarted();
	}
}

void UContentEntryComponent::RemovePlayerFromLobby(AMainPlayer* player)
{
	if (!IsServer()) return;
	if (!IsValid(player)) return;


	// Host였으면 재할당
	if (hostPlayer == player)
	{
		ReassignHost();
	}
	readyPlayers.Remove(player);

	UE_LOG(LogTemp, Log, TEXT("Player %s left lobby (%d/%d)"), *player->GetName(), readyPlayers.Num(), settings.maxPlayers);
}

void UContentEntryComponent::StartContentInternal()
{
	if (!IsServer()) return;
	if (!IsValid(contentManager)) {return;}

	UE_LOG(LogTemp, Log, TEXT("Starting content with %d players"), readyPlayers.Num());

	// 타이머 정리
	UWorld* world = GetWorld();
	if (world && lobbyTimer.IsValid())
	{
		world->GetTimerManager().ClearTimer(lobbyTimer);
	}

	bContentRunning = true;
	bLobbyActive = false;
	contentManager->StartContent();
}

void UContentEntryComponent::ResetLobby()
{
	if (!IsServer()) return;
	bLobbyActive = false;
	hostPlayer = nullptr;
	readyPlayers.Empty();

	// 타이머 정리
	UWorld* world = GetWorld();
	if (world && lobbyTimer.IsValid())
	{
		world->GetTimerManager().ClearTimer(lobbyTimer);
	}

	UE_LOG(LogTemp, Log, TEXT("Lobby reset"));
}

void UContentEntryComponent::OnLobbyTimeout()
{
	if (!IsServer()) return;

	ResetLobby();
	Server_OnLobbyCancelled();
}

void UContentEntryComponent::ReassignHost()
{
	if (!IsServer()) return;
	if (readyPlayers.Num() > 1)
	{
		AMainPlayer* swapPtr = hostPlayer;
		hostPlayer = readyPlayers[1];
		readyPlayers[0] = hostPlayer;
		UE_LOG(LogTemp, Log, TEXT("Host reassigned to %s"), *hostPlayer->GetName());
		return;
	}
	
	UE_LOG(LogTemp, Log, TEXT("No players left - lobby closed"));
	ResetLobby();
}

#pragma endregion Internal


