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
}

void UHeritageObjectComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// InteractableComponent 연결 해제
	if (CachedInteractableComponent)
	{
		CachedInteractableComponent->onRequestInteraction.RemoveDynamic(
			this, &UHeritageObjectComponent::OnInteraction);
		/*CachedInteractableComponent->onMultiInteraction.RemoveDynamic(
			this, &UHeritageObjectComponent::OnInteraction);*/
		CachedInteractableComponent->onChangeState.RemoveDynamic(
			this, &UHeritageObjectComponent::OnStateChanged);
		/*CachedInteractableComponent->onClientInteraction.RemoveDynamic(
			this, &UHeritageObjectComponent::OnStateChanged);*/
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
			CachedInteractableComponent->onRequestInteraction.AddDynamic(
				this, &UHeritageObjectComponent::OnInteraction);
			/*CachedInteractableComponent->onMultiInteraction.AddDynamic(
				this, &UHeritageObjectComponent::OnInteraction);*/
			// 상태 변경 이벤트에 연결 (거리 체크용)
			CachedInteractableComponent->onChangeState.AddDynamic(
				this, &UHeritageObjectComponent::OnStateChanged);
			/*CachedInteractableComponent->onClientInteraction.AddDynamic(
				this, &UHeritageObjectComponent::OnStateChanged);*/
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("[Heritage] InteractableComponent 없음: %s"), *Owner->GetName());
		}
	}
}

void UHeritageObjectComponent::OnInteraction(APawn* InteractingPlayer)
{
	if (!InteractingPlayer)
	{
		return;
	}

	// 발견 처리
	ProcessDiscovery(InteractingPlayer);
}

void UHeritageObjectComponent::OnStateChanged(APlayerController* PlayerController, const EInteractableState& NewState)
{
	// 커스텀 아웃라인 업데이트
	if (bUseCustomOutline)
	{
		UpdateCustomOutline(NewState == EInteractableState::Focused);
	}

	// Interacting 상태가 되면 플레이어를 ViewingPlayers에 추가하고 발견 처리
	if (NewState == EInteractableState::Interacting)
	{
		if (PlayerController && !ViewingPlayers.Contains(PlayerController))
		{
			ViewingPlayers.Add(PlayerController);
			bIsInteracting = true;

			// F키를 눌러 상호작용 시작 -> 발견 처리 실행
			APawn* PlayerPawn = PlayerController->GetPawn();
			if (PlayerPawn)
			{
				ProcessDiscovery(PlayerPawn);
			}
		}
	}

	// OutOfBound 상태가 되면 (범위를 벗어나면)
	if (NewState == EInteractableState::OutOfBound)
	{
		// 해당 플레이어가 UI를 보고 있었다면 UI 닫기 요청
		if (ViewingPlayers.Contains(PlayerController) && DiscoveryManager)
		{
			ViewingPlayers.Remove(PlayerController);
			bIsInteracting = false;

			// Discovery Manager에 UI 닫기 요청
			DiscoveryManager->RequestCloseHeritageUI(PlayerController, HeritageObjectID);
		}
	}
}

void UHeritageObjectComponent::ProcessDiscovery(APawn* Player)
{
	if (!DiscoveryManager || HeritageObjectID.IsEmpty())
	{
		return;
	}

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
	// 서버에서 모든 클라이언트에게 상호작용 종료 브로드캐스트
	if (CachedInteractableComponent && Player && Player->GetPawn())
	{
		CachedInteractableComponent->Multicast_FinishInteracting(Player->GetPawn());
	}
}
