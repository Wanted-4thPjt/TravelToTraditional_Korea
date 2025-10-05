#include "Interaction/ContentEntryComponent.h"

#include "Net/UnrealNetwork.h"
#include "MainPlayer.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"


UContentEntryComponent::UContentEntryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	
	interactionRadius = 300.0f;
	maxWaitSeconds = 60.0f;
	bLobbyActive = false;
	bContentRunning = false;
	hostPlayer = nullptr;
	contentManager = nullptr;
	outlineStencilValue = 252;
	outlineColor = FLinearColor::Green;
	
	if (interactionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionArea")))
	{
		interactionSphere->SetSphereRadius(interactionRadius);
		interactionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		interactionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		interactionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		interactionSphere->SetGenerateOverlapEvents(true);
	}

	if (entryInfoWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget")))
	{
		entryInfoWidget->SetWidgetSpace(EWidgetSpace::Screen);
		entryInfoWidget->SetDrawSize(FVector2D(200.f, 50.f));
		entryInfoWidget->SetRelativeLocation(FVector(0.f, 0.f, 100.f));
	}
}

void UContentEntryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(GetOwner())) {return;}
	
	if (IsValid(interactionSphere))
	{
		interactionSphere->SetupAttachment(GetOwner()->GetRootComponent());
		interactionSphere->RegisterComponent();
		
		interactionSphere->OnComponentBeginOverlap.AddDynamic(this, &UContentEntryComponent::OnInteractionSphereBeginOverlap);
		interactionSphere->OnComponentEndOverlap.AddDynamic(this, &UContentEntryComponent::OnInteractionSphereEndOverlap);
	}

	if (IsValid(entryInfoWidget))
	{
		entryInfoWidget->SetupAttachment(GetOwner()->GetRootComponent());
		entryInfoWidget->RegisterComponent();
		entryInfoWidget->SetVisibility(false);
	}
}

void UContentEntryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UContentEntryComponent, playersInRange);
	DOREPLIFETIME(UContentEntryComponent, readyPlayers);
	DOREPLIFETIME(UContentEntryComponent, hostPlayer);
	DOREPLIFETIME(UContentEntryComponent, bLobbyActive);
	DOREPLIFETIME(UContentEntryComponent, bContentRunning);
}


#pragma region Player Actions

void UContentEntryComponent::RequestJoinLobby(AMainPlayer* player)
{
	if (!IsValid(player)) return;
	Server_RequestJoin(player);
}

void UContentEntryComponent::RequestLeaveLobby(AMainPlayer* player)
{
	if (!IsValid(player)) return;
	Server_RequestLeave(player);
}

void UContentEntryComponent::RequestStartContent(AMainPlayer* player)
{
	if (!IsValid(player)) return;
	Server_RequestStart(player);
}

void UContentEntryComponent::RequestCancelLobby(AMainPlayer* player)
{
	if (!IsValid(player)) return;
	Server_RequestCancel(player);
}
#pragma endregion Player Actions


#pragma region ContentManager Callback

void UContentEntryComponent::OnContentFinished()
{
	if (!IsServer()) return;

	bContentRunning = false;

	for (AMainPlayer* player : readyPlayers)
	{
		if (!IsValid(player)) continue;
		// TODO: 플레이어를 원래 위치로 이동
	}

	/* ContentManager 삭제
	if (contentManager)
	{
		contentManager->Destroy();
		contentManager = nullptr;
	}
	*/

	ResetLobby();
	Multicast_OnContentFinished();
}
#pragma endregion ContentManager Callback


#pragma region Server RPC

void UContentEntryComponent::Server_RequestJoin_Implementation(AMainPlayer* player)
{
	if (!IsServer()) return;

	if (!ValidateJoinRequest(player))
	{
		UE_LOG(LogTemp, Warning, TEXT("Join request validation failed for %s"), *player->GetName());
		return;
	}

	AddPlayerToLobby(player);
	Multicast_UpdateLobbyState();
}

void UContentEntryComponent::Server_RequestLeave_Implementation(AMainPlayer* player)
{
	if (!IsServer()) return;
	if (!IsValid(player)) return;

	RemovePlayerFromLobby(player);
	Multicast_UpdateLobbyState();
}

void UContentEntryComponent::Server_RequestStart_Implementation(AMainPlayer* requestingPlayer)
{
	if (!IsServer()) return;

	if (!ValidateStartRequest(requestingPlayer))
	{
		UE_LOG(LogTemp, Warning, TEXT("Start request validation failed"));
		return;
	}

	StartContentInternal();
	Multicast_OnContentStarted();
}

void UContentEntryComponent::Server_RequestCancel_Implementation(AMainPlayer* requestingPlayer)
{
	if (!IsServer()) return;
	if (!IsValid(requestingPlayer)) return;

	// Host만 취소 가능
	if (hostPlayer != requestingPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Only host can cancel lobby"));
		return;
	}

	ResetLobby();
	Multicast_OnLobbyCancelled();
}
#pragma endregion Server RPC


#pragma region Multicast RPC

void UContentEntryComponent::Multicast_UpdateLobbyState_Implementation()
{
	OnLobbyStateChanged.Broadcast(readyPlayers.Num(), settings.maxPlayers);
	UE_LOG(LogTemp, Log, TEXT("Lobby state updated: %d/%d"), readyPlayers.Num(), settings.maxPlayers);
}

void UContentEntryComponent::Multicast_OnContentStarted_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Content started - Playing start effects"));
	// TODO: 시작 연출, UI 변경 등
}

void UContentEntryComponent::Multicast_OnContentFinished_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Content finished - Showing results"));
	// TODO: 결과 UI 표시, 보상 연출 등
}

void UContentEntryComponent::Multicast_OnLobbyCancelled_Implementation()
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

	if (!playersInRange.Contains(player))
	{
		UE_LOG(LogTemp, Warning, TEXT("ValidateJoinRequest: Player not in range"));
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
	if (!IsValid(player)) return;

	readyPlayers.Add(player);

	// 첫 참가자면 Host로 지정
	if (readyPlayers.Num() == 1)
	{
		hostPlayer = player;
		bLobbyActive = true;

		// 타이머 시작 (maxWaitSeconds > 0일 경우)
		if (maxWaitSeconds > 0.0f)
		{
			UWorld* world = GetWorld();
			if (world)
			{
				world->GetTimerManager().SetTimer(
					lobbyTimer,
					this,
					&UContentEntryComponent::OnLobbyTimeout,
					maxWaitSeconds,
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
		Multicast_OnContentStarted();
	}
}

void UContentEntryComponent::RemovePlayerFromLobby(AMainPlayer* player)
{
	if (!IsValid(player)) return;

	readyPlayers.Remove(player);

	// Host였으면 재할당
	if (hostPlayer == player)
	{
		ReassignHost();
	}

	UE_LOG(LogTemp, Log, TEXT("Player %s left lobby (%d/%d)"), *player->GetName(), readyPlayers.Num(), settings.maxPlayers);

	// 대기실이 비었으면 초기화
	if (readyPlayers.Num() == 0)
	{
		ResetLobby();
	}
}

void UContentEntryComponent::StartContentInternal()
{
	if (!IsServer()) return;

	UE_LOG(LogTemp, Log, TEXT("Starting content with %d players"), readyPlayers.Num());

	// 타이머 정리
	UWorld* world = GetWorld();
	if (world && lobbyTimer.IsValid())
	{
		world->GetTimerManager().ClearTimer(lobbyTimer);
	}

	bContentRunning = true;
	bLobbyActive = false;

	// TODO: ContentManager 생성
	/*
	if (settings.contentManagerClass)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = GetOwner();

		contentManager = world->SpawnActor<ABaseContentManager>(
			settings.contentManagerClass,
			GetOwner()->GetActorLocation(),
			FRotator::ZeroRotator,
			spawnParams
		);

		if (contentManager)
		{
			contentManager->StartContent(readyPlayers);
		}
	}
	*/

	// TODO: 플레이어 이동
	/*
	for (int32 i = 0; i < readyPlayers.Num(); ++i)
	{
		AMainPlayer* player = readyPlayers[i];
		if (!player) continue;

		if (settings.playerSpawnTransforms.IsValidIndex(i))
		{
			player->SetActorTransform(settings.playerSpawnTransforms[i]);
		}
	}
	*/
}

void UContentEntryComponent::ResetLobby()
{
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

	UE_LOG(LogTemp, Warning, TEXT("Lobby timeout - cancelling"));

	ResetLobby();
	Multicast_OnLobbyCancelled();
}

void UContentEntryComponent::ReassignHost()
{
	if (readyPlayers.Num() > 0)
	{
		hostPlayer = readyPlayers[0];
		UE_LOG(LogTemp, Log, TEXT("Host reassigned to %s"), *hostPlayer->GetName());
	}
	else
	{
		hostPlayer = nullptr;
		bLobbyActive = false;

		// 타이머 정리
		UWorld* world = GetWorld();
		if (world && lobbyTimer.IsValid())
		{
			world->GetTimerManager().ClearTimer(lobbyTimer);
		}

		UE_LOG(LogTemp, Log, TEXT("No players left - lobby closed"));
	}
}
#pragma endregion Internal


#pragma region Overlap Event

void UContentEntryComponent::OnInteractionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                                             AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                                             const FHitResult& SweepResult)
{
	if (!IsServer()) return;
	if (AMainPlayer* player = Cast<AMainPlayer>(OtherActor))
	{
		if (playersInRange.Contains(player)) {return;}
		playersInRange.Add(player);
	}
}

void UContentEntryComponent::OnInteractionSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!IsServer()) return;
	if (AMainPlayer* player = Cast<AMainPlayer>(OtherActor))
	{
		playersInRange.Remove(player);
		
		if (readyPlayers.Contains(player))
		{
			RemovePlayerFromLobby(player);
			Multicast_UpdateLobbyState();
		}

	}
}
#pragma endregion Overlap Event