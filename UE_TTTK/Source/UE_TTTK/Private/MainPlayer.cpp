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

void AMainPlayer::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr)
	{
		if (PC->WasInputKeyJustPressed(EKeys::F))
		{
			HandleFKeyPress();
		}
	
	}
}

void AMainPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMainPlayer::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	myInputComponent = PlayerInputComponent;
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
		RequestChangeInputMapping(EMappingMode::Content2);
	}
}

ACarriageVehicle* AMainPlayer::FindNearbyCarriage()
{
	// 주변 모든 CarriageVehicle 찾기
	TArray<AActor*> FoundCarriages;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACarriageVehicle::StaticClass(), FoundCarriages);

	float SearchRadius = 500.0f;  // 5m 반경
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
