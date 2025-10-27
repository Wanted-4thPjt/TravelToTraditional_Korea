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

		// UI 닫기 이벤트에 바인딩 (재확인 가능하도록)
		DiscoveryManager->OnHeritageUIClose.AddDynamic(
			this, &UHeritageObjectComponent::OnUICloseRequested);
	}
	else if (HeritageObjectID.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Heritage] ID가 설정되지 않음: %s"), *GetOwner()->GetName());
	}

	// 상태 체크 타이머 시작 (0.1초마다 체크)
	// 커스텀 아웃라인과 UI 닫기 감지를 위해 필요
	if (CachedInteractableComponent)
	{
		GetWorld()->GetTimerManager().SetTimer(
			StateCheckTimerHandle,
			this,
			&UHeritageObjectComponent::CheckInteractableState,
			0.1f,
			true
		);
	}
}

void UHeritageObjectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 타이머 정리
	if (StateCheckTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(StateCheckTimerHandle);
	}

	// InteractableComponent 연결 해제
	if (CachedInteractableComponent)
	{
		CachedInteractableComponent->OnClientInteraction.RemoveDynamic(
			this, &UHeritageObjectComponent::OnClientInteraction);
		CachedInteractableComponent->OnMultiInteraction.RemoveDynamic(
			this, &UHeritageObjectComponent::OnMultiInteraction);
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
		CachedInteractableComponent = Owner->FindComponentByClass<UInteractableComponent>();

		if (CachedInteractableComponent)
		{
			// 상호작용 이벤트에 연결
			CachedInteractableComponent->OnClientInteraction.AddDynamic(
				this, &UHeritageObjectComponent::OnClientInteraction);
			CachedInteractableComponent->OnMultiInteraction.AddDynamic(
				this, &UHeritageObjectComponent::OnMultiInteraction);
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
	// 서버에 발견 처리 요청
	if (PlayerController->IsLocalController())
	{
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
		Server_ProcessDiscovery(PlayerController, HeritageObjectID);
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

void UHeritageObjectComponent::ProcessDiscovery(APawn* Player)
{
	if (!DiscoveryManager || HeritageObjectID.IsEmpty())
	{
		return;
	}

	// 이 함수는 이제 사용하지 않음 (RPC로 대체됨)
	// Discovery Manager에게 발견 처리 위임
	DiscoveryManager->ProcessHeritageDiscovery(Player, HeritageObjectID);
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

void UHeritageObjectComponent::UpdateCustomOutline(bool bShowOutline)
{
	if (!bUseCustomOutline || !CustomOutlineMaterial)
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// 모든 MeshComponent 가져오기
	TArray<UMeshComponent*> MeshComponents;
	Owner->GetComponents<UMeshComponent>(MeshComponents);

	if (MeshComponents.Num() == 0)
	{
		return;
	}

	if (bShowOutline)
	{
		if (bUseOverlayMaterial)
		{
			// Overlay Material 방식 (기존 머티리얼 유지, 위에 오버레이)
			for (UMeshComponent* MeshComp : MeshComponents)
			{
				if (MeshComp)
				{
					MeshComp->SetOverlayMaterial(CustomOutlineMaterial);
				}
			}
		}
		else
		{
			// 머티리얼 교체 방식 (기존 방식)
			OriginalMaterials.Empty();

			for (UMeshComponent* MeshComp : MeshComponents)
			{
				if (!MeshComp)
				{
					continue;
				}

				// 원본 머티리얼 백업
				int32 NumMaterials = MeshComp->GetNumMaterials();
				for (int32 i = 0; i < NumMaterials; i++)
				{
					OriginalMaterials.Add(MeshComp->GetMaterial(i));
				}

				// 커스텀 아웃라인 머티리얼로 오버라이드
				for (int32 i = 0; i < NumMaterials; i++)
				{
					MeshComp->SetMaterial(i, CustomOutlineMaterial);
				}
			}
		}
	}
	else
	{
		if (bUseOverlayMaterial)
		{
			// Overlay Material 제거
			for (UMeshComponent* MeshComp : MeshComponents)
			{
				if (MeshComp)
				{
					MeshComp->SetOverlayMaterial(nullptr);
				}
			}
		}
		else
		{
			// 원본 머티리얼 복원
			if (OriginalMaterials.Num() > 0)
			{
				int32 MaterialIndex = 0;
				for (UMeshComponent* MeshComp : MeshComponents)
				{
					if (!MeshComp)
					{
						continue;
					}

					int32 NumMaterials = MeshComp->GetNumMaterials();
					for (int32 i = 0; i < NumMaterials && MaterialIndex < OriginalMaterials.Num(); i++)
					{
						MeshComp->SetMaterial(i, OriginalMaterials[MaterialIndex]);
						MaterialIndex++;
					}
				}

				OriginalMaterials.Empty();
			}
		}
	}
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

	// 로컬 플레이어만 서버에 종료 요청
	if (Player->IsLocalController())
	{
		Server_FinishInteraction(Player);
	}
}

void UHeritageObjectComponent::Server_FinishInteraction_Implementation(APlayerController* Player)
{
	// 상호작용은 InteractableComponent에서 자동으로 종료됨
	// 여기서는 추가 처리가 필요한 경우에만 사용
}

void UHeritageObjectComponent::CheckInteractableState()
{
	if (!CachedInteractableComponent)
	{
		return;
	}

	EInteractableState CurrentState = CachedInteractableComponent->GetState();

	// 상태가 변경된 경우에만 업데이트
	if (CurrentState != LastState)
	{
		// 커스텀 아웃라인: Focused 상태일 때 아웃라인 표시
		if (bUseCustomOutline)
		{
			UpdateCustomOutline(CurrentState == EInteractableState::Focused);
		}

		LastState = CurrentState;

		// OutOfBound 상태가 되면 UI 닫기 처리
		// 단, UI를 보고 있는 중이 아닐 때만 (bIsInteracting이 false일 때)
		if (CurrentState == EInteractableState::OutOfBound && !bIsInteracting)
		{
			APlayerController* LocalPC = GetWorld()->GetFirstPlayerController();
			if (LocalPC && ViewingPlayers.Contains(LocalPC) && DiscoveryManager)
			{
				ViewingPlayers.Remove(LocalPC);
				DiscoveryManager->RequestCloseHeritageUI(LocalPC, HeritageObjectID);
			}
		}
	}
}
