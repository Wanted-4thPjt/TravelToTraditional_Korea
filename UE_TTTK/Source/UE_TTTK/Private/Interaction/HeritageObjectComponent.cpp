#include "Interaction/HeritageObjectComponent.h"
#include "Interaction/InteractableComponent.h"
#include "GameFlow/HeritageDiscoveryManager.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"

UHeritageObjectComponent::UHeritageObjectComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// 컴포넌트 태그 추가 (검색용)
	ComponentTags.Add(TEXT("Heritage"));
}

void UHeritageObjectComponent::BeginPlay()
{
	Super::BeginPlay();

	// Discovery Manager 가져오기
	if (UGameInstance* GameInstance = GetWorld()->GetGameInstance())
	{
		DiscoveryManager = GameInstance->GetSubsystem<UHeritageDiscoveryManager>();
	}

	// InteractableComponent와 연결
	BindToInteractableComponent();

	// Discovery Manager에 등록
	if (DiscoveryManager && !HeritageObjectID.IsEmpty())
	{
		DiscoveryManager->RegisterHeritageObject(this);

		// UI 닫기 이벤트에 바인딩
		DiscoveryManager->OnHeritageUIClose.AddDynamic(
			this, &UHeritageObjectComponent::OnUICloseRequested);
	}
	else if (HeritageObjectID.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Heritage] ID가 설정되지 않음: %s"), *GetOwner()->GetName());
	}

	// Timer 제거 - ShouldFinishInteraction 델리게이트로 대체
}

void UHeritageObjectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// InteractableComponent 연결 해제
	if (CachedInteractableComponent)
	{
		CachedInteractableComponent->OnClientInteraction.RemoveDynamic(
			this, &UHeritageObjectComponent::OnClientInteraction);
		CachedInteractableComponent->OnMultiInteraction.RemoveDynamic(
			this, &UHeritageObjectComponent::OnMultiInteraction);
		CachedInteractableComponent->OnStateChanged.RemoveDynamic(
			this, &UHeritageObjectComponent::OnInteractableStateChanged);
	}

	// Discovery Manager에서 제거
	if (DiscoveryManager)
	{
		DiscoveryManager->OnHeritageUIClose.RemoveDynamic(
			this, &UHeritageObjectComponent::OnUICloseRequested);
		DiscoveryManager->UnregisterHeritageObject(this);
	}

	Super::EndPlay(EndPlayReason);
}

void UHeritageObjectComponent::BindToInteractableComponent()
{
	// 같은 액터에서 InteractableComponent 찾기
	if (AActor* Owner = GetOwner())
	{
		CachedInteractableComponent = Owner->FindComponentByTag<UInteractableComponent>("Interactable");

		if (CachedInteractableComponent)
		{
			// 상호작용 이벤트에 연결
			CachedInteractableComponent->OnClientInteraction.AddDynamic(
				this, &UHeritageObjectComponent::OnClientInteraction);
			CachedInteractableComponent->OnMultiInteraction.AddDynamic(
				this, &UHeritageObjectComponent::OnMultiInteraction);
			CachedInteractableComponent->OnStateChanged.AddDynamic(
				this, &UHeritageObjectComponent::OnInteractableStateChanged);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Heritage] InteractableComponent 없음: %s"), *Owner->GetName());
		}
	}
}

void UHeritageObjectComponent::OnClientInteraction(APlayerController* PlayerController)
{
	if (!PlayerController)
	{
		return;
	}

	// 로컬 클라이언트에서만 호출됨
	if (PlayerController->IsLocalController())
	{
		// 즉시 ViewingPlayers에 추가하여 상호작용 유지 (ShouldFinishedInteraction이 false 반환하도록)
		if (!ViewingPlayers.Contains(PlayerController))
		{
			ViewingPlayers.Add(PlayerController);
			bIsInteracting = true;
		}

		// 서버에 발견 처리 요청
		Server_ProcessDiscovery(PlayerController, HeritageObjectID);
	}
}

void UHeritageObjectComponent::OnMultiInteraction(APawn* InteractingPlayer)
{
	if (!InteractingPlayer)
	{
		return;
	}

	APlayerController* PlayerController = InteractingPlayer->GetController<APlayerController>();
	if (!PlayerController)
	{
		return;
	}

	// 모든 클라이언트에서 호출됨
	// 로컬 클라이언트에서만 서버에 요청
	if (PlayerController->IsLocalController())
	{
		// 즉시 ViewingPlayers에 추가하여 상호작용 유지 (ShouldFinishedInteraction이 false 반환하도록)
		if (!ViewingPlayers.Contains(PlayerController))
		{
			ViewingPlayers.Add(PlayerController);
			bIsInteracting = true;
		}

		// 서버에 발견 처리 요청
		Server_ProcessDiscovery(PlayerController, HeritageObjectID);
	}
}

void UHeritageObjectComponent::OnInteractableStateChanged(APlayerController* PlayerController, EInteractableState NewState)
{
	if (!PlayerController || !PlayerController->IsLocalController())
	{
		return;
	}

	// OutOfBound 상태가 되면 UI 자동 닫기
	if (NewState == EInteractableState::OutOfBound && bIsInteracting)
	{
		if (ViewingPlayers.Contains(PlayerController) && DiscoveryManager)
		{
			ViewingPlayers.Remove(PlayerController);
			bIsInteracting = false;
			DiscoveryManager->RequestCloseHeritageUI(PlayerController, HeritageObjectID);
		}
	}
}

void UHeritageObjectComponent::Server_ProcessDiscovery_Implementation(APlayerController* PlayerController, const FString& HeritageID)
{
	if (!PlayerController || !DiscoveryManager || HeritageID.IsEmpty())
	{
		return;
	}

	// 서버에서만 실행
	APawn* PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn)
	{
		return;
	}

	// ViewingPlayers에 추가 (서버)
	if (!ViewingPlayers.Contains(PlayerController))
	{
		ViewingPlayers.Add(PlayerController);
		bIsInteracting = true;
	}

	// 이미 발견했는지 확인
	bool bAlreadyDiscovered = DiscoveryManager->HasPlayerDiscovered(PlayerController, HeritageID);

	// Heritage 데이터 가져오기
	FHeritageObjectData HeritageData = DiscoveryManager->GetHeritageDataByID(HeritageID);
	if (HeritageData.ObjectID.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[Heritage] 데이터 없음: %s"), *HeritageID);
		return;
	}

	// 첫 발견인 경우에만 점수 및 카운트 추가
	bool bIsFirstDiscovery = false;
	if (!bAlreadyDiscovered)
	{
		// Discovery Manager에게 발견 처리 위임
		DiscoveryManager->ProcessHeritageDiscovery(PlayerPawn, HeritageID);
		bIsFirstDiscovery = true;

		UE_LOG(LogTemp, Warning, TEXT("[Heritage] 첫 발견: %s (점수: %d, 플레이어: %s)"),
			*HeritageID, HeritageData.DiscoveryScore, *PlayerController->GetName());
	}

	// 클라이언트에게 UI 표시 요청
	Client_ShowHeritageUI(PlayerController, HeritageID, HeritageData, bIsFirstDiscovery);
}

void UHeritageObjectComponent::Client_ShowHeritageUI_Implementation(APlayerController* PlayerController, const FString& HeritageID, const FHeritageObjectData& HeritageData, bool bIsFirstDiscovery)
{
	if (!PlayerController || !DiscoveryManager)
	{
		return;
	}

	// ViewingPlayers에 추가 (클라이언트)
	if (!ViewingPlayers.Contains(PlayerController))
	{
		ViewingPlayers.Add(PlayerController);
		bIsInteracting = true;
	}

	// 클라이언트에서 UI 표시 (델리게이트 브로드캐스트)
	DiscoveryManager->OnHeritageDiscovered.Broadcast(PlayerController, HeritageID, HeritageData, bIsFirstDiscovery);

	UE_LOG(LogTemp, Log, TEXT("[Heritage Client] UI 표시: %s (첫발견: %s)"),
		*HeritageID, bIsFirstDiscovery ? TEXT("예") : TEXT("아니오"));
}

FHeritageObjectData UHeritageObjectComponent::GetHeritageData() const
{
	if (DiscoveryManager)
	{
		return DiscoveryManager->GetHeritageDataByID(HeritageObjectID);
	}
	return FHeritageObjectData(); // 빈 데이터 반환
}

bool UHeritageObjectComponent::HasPlayerDiscovered(APlayerController* Player) const
{
	if (DiscoveryManager && Player)
	{
		return DiscoveryManager->HasPlayerDiscovered(Player, HeritageObjectID);
	}
	return false;
}

void UHeritageObjectComponent::OnUICloseRequested(APlayerController* Player, const FString& HeritageID)
{
	// 이 Heritage의 UI가 닫힌 경우에만 처리
	if (HeritageID != HeritageObjectID || !Player)
	{
		return;
	}

	// ViewingPlayers에서 제거
	if (ViewingPlayers.Contains(Player))
	{
		ViewingPlayers.Remove(Player);
		bIsInteracting = false;
	}

	// 로컬 플레이어만 상호작용 종료 (OutOfBound로 상태 전환)
	if (Player->IsLocalController() && CachedInteractableComponent)
	{
		CachedInteractableComponent->TryChangeState(Player, EInteractableState::OutOfBound);
	}
}

