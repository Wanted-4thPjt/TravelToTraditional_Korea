// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayer.h"

#include "PlayerSubSystem.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"
#include "Carriage/CarriageVehicle.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Interaction/InteractionComponent.h"
#include "Interaction/ViewComponent.h"
#include "GameFlow/BaseContentManager.h"
#include "Content/BaseContentComponent.h"
#include "EnhancedInputComponent.h"

AMainPlayer::AMainPlayer()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	
	viewComponent = CreateDefaultSubobject<UViewComponent>(TEXT("View"));
	interactionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction"));
}

void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocallyControlled())
	{
		viewComponent->OnViewSthByLineTrace.AddDynamic(this, &AMainPlayer::OnViewInteractableActor);
		viewComponent->EnableTrace(true);
	}
	if (HandFireClass)
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Owner = this;
		handFire = GetWorld() -> SpawnActor<AActor>(HandFireClass,FVector::ZeroVector,FRotator::ZeroRotator,spawnParams);
		if (handFire)
		{
			handFire->AttachToComponent(GetMesh(),
				 FAttachmentTransformRules::SnapToTargetIncludingScale,
				 TEXT("hand_r"));
			handFire->SetActorRelativeLocation(FVector(-11.800000,0.300000,-13.333340));
			handFire->SetActorRelativeRotation(FRotator(0,0,0));
				
			handFire->SetActorHiddenInGame(true);
			

		}
	}
}

void AMainPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	myInputComponent = PlayerInputComponent;
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		EnhancedInputComponent->BindAction(ia_Swing, ETriggerEvent::Started, this, &AMainPlayer::Swing);
		
	}
}



void AMainPlayer::RequestChangeInputMapping(EMappingMode mode)
{

	if (APlayerController* playerController = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* localPlayer = playerController->GetLocalPlayer())
		{
			if (UPlayerSubSystem* playerManager = localPlayer->GetSubsystem<UPlayerSubSystem>())
			{
				playerManager->ChangeInputMapping(this, mode);
			}
		}
	}
}

void AMainPlayer::ReturnToDefaultMode()
{
	RequestChangeInputMapping(EMappingMode::Content1);
	SetupPlayerInputComponent(myInputComponent);
}

void AMainPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainPlayer, bIsRidingCarriage);
}

// ========================================
// Boarding System Implementation
// ========================================

void AMainPlayer::HandleFKeyPress()
{
	UE_LOG(LogTemp, Warning, TEXT("[DEBUG] F키 눌림 - bIsRidingCarriage: %s"), bIsRidingCarriage ? TEXT("true") : TEXT("false"));
	
	// 탑승 중이면 하차 시도
	if (bIsRidingCarriage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DEBUG] 하차 시도"));
		RequestExitCarriage();
		return;
	}

	// 탑승 중이 아니면 근처 마차 찾아서 탑승 시도
	ACarriageVehicle* NearbyCarriage = FindNearbyCarriage();
	if (NearbyCarriage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[DEBUG] 마차 발견! 탑승 시도"));
		RequestBoardCarriage(NearbyCarriage);
	}
	else
	{
		// 기존 F키 기능 (Input Mapping 변경)
		UE_LOG(LogTemp, Warning, TEXT("[DEBUG] 마차 못 찾음 - 상호작용 모드"));
		//RequestChangeInputMapping(EMappingMode::Content2);
	}
}

ACarriageVehicle* AMainPlayer::FindNearbyCarriage()
{
	// 주변 모든 CarriageVehicle 찾기
	TArray<AActor*> FoundCarriages;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACarriageVehicle::StaticClass(), FoundCarriages);

	float SearchRadius = 500.0f;  
	FVector PlayerLocation = GetActorLocation();

	for (AActor* Actor : FoundCarriages)
	{
		ACarriageVehicle* Carriage = Cast<ACarriageVehicle>(Actor);
		if (Carriage && Carriage->CanBoardCarriage())
		{
			float Distance = FVector::Dist(PlayerLocation, Carriage->GetActorLocation());
			if (Distance <= SearchRadius)
			{
				return Carriage;
			}
		}
	}

	return nullptr;
}

void AMainPlayer::RequestBoardCarriage(ACarriageVehicle* Carriage)
{
	if (!Carriage)
	{
		UE_LOG(LogTemp, Warning, TEXT("RequestBoardCarriage: Carriage가 nullptr입니다!"));
		return;
	}

	// Server RPC 호출 (MainPlayer가 소유한 RPC이므로 가능)
	Server_RequestBoardCarriage(Carriage);

	UE_LOG(LogTemp, Log, TEXT("플레이어가 마차 탑승을 요청했습니다."));
}

void AMainPlayer::Server_RequestBoardCarriage_Implementation(ACarriageVehicle* Carriage)
{
	if (!Carriage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_RequestBoardCarriage: Carriage가 nullptr입니다!"));
		return;
	}

	// 서버에서 마차의 탑승 함수 호출
	Carriage->Server_RequestBoard(this);
}

void AMainPlayer::RequestExitCarriage()
{
	if (!CurrentCarriage)
	{
		UE_LOG(LogTemp, Warning, TEXT("RequestExitCarriage: CurrentCarriage가 nullptr입니다!"));
		return;
	}

	// Server RPC 호출
	Server_RequestExitCarriage(CurrentCarriage);

	UE_LOG(LogTemp, Log, TEXT("플레이어가 마차 하차를 요청했습니다."));
}

void AMainPlayer::Server_RequestExitCarriage_Implementation(ACarriageVehicle* Carriage)
{
	if (!Carriage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_RequestExitCarriage: Carriage가 nullptr입니다!"));
		return;
	}

	// 서버에서 마차의 하차 함수 호출
	Carriage->Server_RequestExit(this);
}

void AMainPlayer::SwitchToFirstPersonCamera()
{
	// UE_TTTKCharacter에서 상속받은 카메라 컴포넌트 찾기
	UCameraComponent* Camera = FindComponentByClass<UCameraComponent>();
	USpringArmComponent* SpringArm = FindComponentByClass<USpringArmComponent>();

	if (Camera && SpringArm)
	{
		// SpringArm 길이를 0으로 (1인칭 효과)
		SpringArm->TargetArmLength = 0.0f;
		SpringArm->bUsePawnControlRotation = true;

		UE_LOG(LogTemp, Log, TEXT("1인칭 카메라로 전환"));
	}
}

void AMainPlayer::SwitchToThirdPersonCamera()
{
	
	UCameraComponent* Camera = FindComponentByClass<UCameraComponent>();
	USpringArmComponent* SpringArm = FindComponentByClass<USpringArmComponent>();

	if (Camera && SpringArm)
	{
		
		SpringArm->TargetArmLength = 400.0f;  
		SpringArm->bUsePawnControlRotation = true;

		UE_LOG(LogTemp, Log, TEXT("3인칭 카메라로 복귀"));
	}
}

void AMainPlayer::EqiqueHandFire_Implementation()
{
	if (!bIsHandfire)
	{
		handFire->SetActorHiddenInGame(false);
		bIsHandfire = true;
	}
}



AActor* AMainPlayer::GetFocusedActor() const
{
	if (!IsValid(interactionComponent)) {return nullptr;}
	return interactionComponent->GetFocusedActor();
}

void AMainPlayer::OnViewInteractableActor(const FHitResult& hitResult)
{
	interactionComponent->FocusInteractableActor(hitResult);
}

// ========================================
// Content Input 관리 구현
// ========================================

void AMainPlayer::ActivateContentInput(UBaseContentManager* manager)
{
	if (!manager || !manager->contentInputComponentClass)
	{
		UE_LOG(LogTemp, Error, TEXT("MainPlayer: Invalid manager or component class"));
		return;
	}

	// 1. 기존 활성화된 Component 비활성화
	if (activeContentInput)
	{
		DeactivateContentInput();
	}

	// 2. Component 생성 또는 캐시에서 가져오기 (Lazy Initialization)
	UBaseContentComponent* contentComponent = GetOrCreateContentInput(manager->contentInputComponentClass);
	if (!contentComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("MainPlayer: Failed to create content input component"));
		return;
	}

	// 3. Enhanced Input Binding 설정 (최초 1회만, 이후 재사용)
	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(myInputComponent))
	{
		contentComponent->SetupInputBindings(EnhancedInput);
	}

	// 4. Component 활성화
	contentComponent->ActivateContentInput(manager);
	activeContentInput = contentComponent;

	UE_LOG(LogTemp, Log, TEXT("MainPlayer: Content input activated - %s"),
	       *manager->contentInputComponentClass->GetName());
}

void AMainPlayer::DeactivateContentInput()
{
	if (!activeContentInput)
	{
		return;
	}

	// Component 비활성화 (제거하지 않고 캐시에 보관)
	activeContentInput->DeactivateContentInput();

	UE_LOG(LogTemp, Log, TEXT("MainPlayer: Content input deactivated - %s"),
	       *activeContentInput->GetClass()->GetName());

	activeContentInput = nullptr;
}

UBaseContentComponent* AMainPlayer::GetOrCreateContentInput(TSubclassOf<UBaseContentComponent> componentClass)
{
	if (!componentClass)
	{
		UE_LOG(LogTemp, Error, TEXT("MainPlayer: componentClass is null"));
		return nullptr;
	}

	// 1. 캐시에 이미 존재하는지 확인
	if (UBaseContentComponent** CachedComponent = cachedContentInputs.Find(componentClass))
	{
		if (IsValid(*CachedComponent))
		{
			UE_LOG(LogTemp, Verbose, TEXT("MainPlayer: Reusing cached component - %s"),
			       *componentClass->GetName());
			return *CachedComponent;
		}
	}

	// 2. 캐시에 없으면 새로 생성
	UBaseContentComponent* NewComponent = NewObject<UBaseContentComponent>(
		this,
		componentClass,
		NAME_None,
		RF_Transient
	);

	if (!NewComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("MainPlayer: Failed to create component - %s"),
		       *componentClass->GetName());
		return nullptr;
	}

	// 3. Component 초기화
	NewComponent->RegisterComponent();

	// 4. 캐시에 저장
	cachedContentInputs.Add(componentClass, NewComponent);

	UE_LOG(LogTemp, Log, TEXT("MainPlayer: Created new content component - %s"),
	       *componentClass->GetName());

	return NewComponent;
}

void AMainPlayer::Swing()
{
	if (bIsHandfire && !bIsSwing)
	{
		PlayAnimMontage(SwingMontage);
	}
}


// void AMainPlayer::RequestChangeInputMapping(EMappingMode mode)
// {
// 	UPlayerSubSystem* playerManager = GetGameInstance()->GetSubsystem<UPlayerSubSystem>();
// 	if (playerManager)
// 	{
// 		//playerManager->ChangeInputMapping(this,mode);
// 	}
// }

