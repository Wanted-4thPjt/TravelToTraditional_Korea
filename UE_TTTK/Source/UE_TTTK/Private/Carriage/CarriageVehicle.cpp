// Fill out your copyright notice in the Description page of Project Settings.


#include "Carriage/CarriageVehicle.h"

#include "Carriage/CarriageMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "MainPlayer.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACarriageVehicle::ACarriageVehicle()
{
	PrimaryActorTick.bCanEverTick = true;

	CarriageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarriageMesh"));
	RootComponent = CarriageMesh;
	WheelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WheelMesh"));
	WheelMesh->SetupAttachment(RootComponent);


	BoardArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BoardingArea"));
	BoardArea->SetupAttachment(RootComponent);
	BoardArea->SetBoxExtent(FVector(200.f,200.f,100.f));

	// Ground Trace Points
	FrontTracePoint = CreateDefaultSubobject<USceneComponent>(TEXT("FrontTracePoint"));
	FrontTracePoint->SetupAttachment(RootComponent);
	FrontTracePoint->SetRelativeLocation(FVector(200.0f, -100.0f, 0.0f));

	RearTracePoint = CreateDefaultSubobject<USceneComponent>(TEXT("RearTracePoint"));
	RearTracePoint->SetupAttachment(RootComponent);
	RearTracePoint->SetRelativeLocation(FVector(-200.0f, 100.0f, 0.0f));

	// Seat System (위치는 에디터에서 설정)
	Seat1 = CreateDefaultSubobject<USceneComponent>(TEXT("Seat1"));
	Seat1->SetupAttachment(RootComponent);

	Seat2 = CreateDefaultSubobject<USceneComponent>(TEXT("Seat2"));
	Seat2->SetupAttachment(RootComponent);  

	MovementComponent = CreateDefaultSubobject<UCarriageMovementComponent>(TEXT("MovementComponent"));

	Speed =300.f;
	bLooping=true;
	PathActor = nullptr;

	bReplicates = true;
	SetReplicateMovement(true);
}

void ACarriageVehicle::BeginPlay()
{
	Super::BeginPlay();

	// BoardArea Overlap 이벤트 바인딩
	if (BoardArea)
	{
		BoardArea->OnComponentBeginOverlap.AddDynamic(this, &ACarriageVehicle::OnBoardAreaBeginOverlap);
		BoardArea->OnComponentEndOverlap.AddDynamic(this, &ACarriageVehicle::OnBoardAreaEndOverlap);
	}

	if (HasAuthority())
	{
		StartMovement();
	}
}

void ACarriageVehicle::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACarriageVehicle, PassengerList);
}

void ACarriageVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateWheel();
}

void ACarriageVehicle::StartMovement()
{
	UE_LOG(LogTemp,Warning,TEXT("ACarriageVehicle::StartMovement"));
	if (MovementComponent)
	{
		MovementComponent->StartMovement();
	}
}

void ACarriageVehicle::StopMovement()
{
	UE_LOG(LogTemp,Warning,TEXT("ACarriageVehicle::StopMovement"));
	if (MovementComponent)
	{
		MovementComponent->StopMovement();
	}
}

void ACarriageVehicle::RotateWheel()
{
	if (!MovementComponent->bisMoving)return;
	if (!MovementComponent->bisStoped)
	{
		FRotator wheelRotation = WheelMesh->GetRelativeRotation();
		int  newRotRoll = ((int)wheelRotation.Roll+(int)WheelSpeed)%360;
		FRotator newWheelRotation = FRotator(0.f, 0.f, newRotRoll);
		WheelMesh->SetRelativeRotation(newWheelRotation);
	}
}


void ACarriageVehicle::OnBoardAreaBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMainPlayer* Player = Cast<AMainPlayer>(OtherActor))
	{
		PlayersInRange.AddUnique(Player);
		UE_LOG(LogTemp, Log, TEXT("플레이어 [%s]가 마차 BoardArea 진입"), *Player->GetName());
	}
}

void ACarriageVehicle::OnBoardAreaEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMainPlayer* Player = Cast<AMainPlayer>(OtherActor))
	{
		PlayersInRange.Remove(Player);
		UE_LOG(LogTemp, Log, TEXT("플레이어 [%s]가 마차 BoardArea 이탈"), *Player->GetName());
	}
}

void ACarriageVehicle::Server_RequestBoard_Implementation(AMainPlayer* Player)
{
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_RequestBoard: Player가 nullptr입니다!"));
		return;
	}

	// 탑승 가능 여부 체크
	if (!CanBoardCarriage())
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_RequestBoard: 탑승 불가 (정차 중이 아니거나 만석)"));
		return;
	}

	// 빈 좌석 찾기
	int32 SeatIndex = GetAvailableSeatIndex();
	if (SeatIndex == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_RequestBoard: 빈 좌석이 없습니다!"));
		return;
	}

	// 탑승자 목록에 추가
	PassengerList.Add(Player);

	// 모든 클라이언트에 탑승 알림
	Multicast_OnPlayerBoarded(Player, SeatIndex);

	UE_LOG(LogTemp, Log, TEXT("플레이어 [%s]가 좌석 %d에 탑승했습니다."), *Player->GetName(), SeatIndex);
}

void ACarriageVehicle::Multicast_OnPlayerBoarded_Implementation(AMainPlayer* Player, int32 SeatIndex)
{
	if (!Player)
	{
		return;
	}

	USceneComponent* SeatComponent = GetSeatComponent(SeatIndex);
	if (!SeatComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("Multicast_OnPlayerBoarded: 좌석 컴포넌트가 nullptr입니다!"));
		return;
	}

	// 1단계: 모든 Collision과 Movement를 먼저 끄기
	if (ACharacter* Character = Cast<ACharacter>(Player))
	{
		// SpringArm Collision Test 끄기 (카메라 충돌로 인한 밀림 방지)
		if (USpringArmComponent* SpringArm = Player->GetCameraBoom())
		{
			SpringArm->bDoCollisionTest = false;
			UE_LOG(LogTemp, Log, TEXT("[Multicast] SpringArm Collision Test 비활성화"));
		}

		// Capsule Collision 끄기
		if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			UE_LOG(LogTemp, Log, TEXT("[Multicast] Capsule Collision 비활성화"));
		}

		// SkeletalMesh Collision 끄기
		if (USkeletalMeshComponent* SkelMesh = Character->GetMesh())
		{
			SkelMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			UE_LOG(LogTemp, Log, TEXT("[Multicast] SkeletalMesh Collision 비활성화"));
		}

		// Movement Component 비활성화
		if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
		{
			MovementComp->DisableMovement();
			UE_LOG(LogTemp, Log, TEXT("[Multicast] Movement Component 비활성화"));
		}
	}

	// 2단계: 0.05초 후 Attach (Collision이 완전히 꺼진 후)
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Player, SeatIndex]()
	{
		DelayedAttachPlayer(Player, SeatIndex);
	}, 0.05f, false);

	UE_LOG(LogTemp, Log, TEXT("[Multicast] Collision 비활성화 완료, 0.05초 후 Attach 실행"));
}

void ACarriageVehicle::DelayedAttachPlayer(AMainPlayer* Player, int32 SeatIndex)
{
	if (!Player)
	{
		return;
	}

	USceneComponent* SeatComponent = GetSeatComponent(SeatIndex);
	if (!SeatComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("DelayedAttachPlayer: 좌석 컴포넌트가 nullptr입니다!"));
		return;
	}

	// Attach (Collision이 꺼진 상태에서 충돌 없이 깔끔하게 이동)
	Player->AttachToComponent(SeatComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Seat의 로컬 좌표 (0,0,0)으로 정확히 이동
	Player->SetActorRelativeLocation(FVector::ZeroVector);
	Player->SetActorRelativeRotation(FRotator::ZeroRotator);

	// 컨트롤러 회전 리셋 (카메라 방향)
	if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
	{
		FRotator CarriageRotation = GetActorRotation();
		PC->SetControlRotation(CarriageRotation);
	}

	// 탑승 상태 설정 (모든 클라이언트에서)
	Player->bIsRidingCarriage = true;
	Player->CurrentCarriage = this;

	// 로컬 플레이어면 카메라도 전환
	if (Player->IsLocallyControlled())
	{
		Player->SwitchToFirstPersonCamera();
		UE_LOG(LogTemp, Log, TEXT("[DelayedAttach] 로컬 플레이어 카메라 1인칭 전환"));
	}

	UE_LOG(LogTemp, Log, TEXT("[DelayedAttach] 플레이어 [%s]가 좌석 %d에 탑승 완료"), *Player->GetName(), SeatIndex);
}

void ACarriageVehicle::Server_RequestExit_Implementation(AMainPlayer* Player)
{
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_RequestExit: Player가 nullptr입니다!"));
		return;
	}

	// 하차 가능 여부 체크 (정차 중인지)
	if (!MovementComponent || !MovementComponent->bisStoped || !MovementComponent->CurrentStopPoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server_RequestExit: 하차 불가 (정류장에 정차 중이 아님)"));
		return;
	}

	// 탑승자 목록에서 제거
	PassengerList.Remove(Player);

	// 모든 클라이언트에 하차 알림
	Multicast_OnPlayerExited(Player);

	UE_LOG(LogTemp, Log, TEXT("플레이어 [%s]가 하차했습니다."), *Player->GetName());
}

void ACarriageVehicle::Multicast_OnPlayerExited_Implementation(AMainPlayer* Player)
{
	if (!Player)
	{
		return;
	}

	// 플레이어 Detach
	Player->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 모든 Collision 및 Movement Component 복구
	if (ACharacter* Character = Cast<ACharacter>(Player))
	{
		// SpringArm Collision Test 복구
		if (USpringArmComponent* SpringArm = Player->GetCameraBoom())
		{
			SpringArm->bDoCollisionTest = true;
			UE_LOG(LogTemp, Log, TEXT("[Multicast] SpringArm Collision Test 복구"));
		}

		// Capsule Collision 복구
		if (UCapsuleComponent* Capsule = Character->GetCapsuleComponent())
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			UE_LOG(LogTemp, Log, TEXT("[Multicast] Capsule Collision 복구"));
		}

		// SkeletalMesh Collision 복구
		if (USkeletalMeshComponent* SkelMesh = Character->GetMesh())
		{
			SkelMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			UE_LOG(LogTemp, Log, TEXT("[Multicast] SkeletalMesh Collision 복구"));
		}

		// Movement Component 복구 (Walking Mode로 전환)
		if (UCharacterMovementComponent* MovementComp = Character->GetCharacterMovement())
		{
			MovementComp->SetMovementMode(MOVE_Walking);
			UE_LOG(LogTemp, Log, TEXT("[Multicast] Movement Component 복구 (Walking Mode)"));
		}
	}

	// 하차 위치 설정 (BoardArea 옆)
	if (BoardArea)
	{
		FVector ExitLocation = BoardArea->GetComponentLocation();
		ExitLocation += GetActorRightVector() * 200.0f;  // 마차 오른쪽으로 200cm
		Player->SetActorLocation(ExitLocation);
	}

	// 탑승 상태 해제 (모든 클라이언트에서)
	Player->bIsRidingCarriage = false;
	Player->CurrentCarriage = nullptr;

	// 로컬 플레이어면 카메라와 Input도 복구
	if (Player->IsLocallyControlled())
	{
		Player->SwitchToThirdPersonCamera();
		Player->ReturnToDefaultMode();
		UE_LOG(LogTemp, Log, TEXT("[Multicast] 로컬 플레이어 카메라 3인칭 복귀"));
	}

	UE_LOG(LogTemp, Log, TEXT("[Multicast] 플레이어 [%s]가 하차"), *Player->GetName());
}

// ========================================
// Helper Functions
// ========================================

bool ACarriageVehicle::CanBoardCarriage() const
{
	// 정차 중이고, 현재 정류장이 있으며, 빈 좌석이 있는지 체크
	if (!MovementComponent)
	{
		return false;
	}

	bool bIsStopped = MovementComponent->bisStoped;
	bool bHasStopPoint = MovementComponent->CurrentStopPoint != nullptr;
	bool bHasAvailableSeat = PassengerList.Num() < 2;  // 최대 2명

	return bIsStopped && bHasStopPoint && bHasAvailableSeat;
}

int32 ACarriageVehicle::GetAvailableSeatIndex() const
{
	// PassengerList 개수로 빈 좌석 인덱스 반환 (0 또는 1)
	// 0명 탑승 → 0번 좌석
	// 1명 탑승 → 1번 좌석
	// 2명 탑승 → -1 (만석)

	int32 PassengerCount = PassengerList.Num();

	if (PassengerCount >= 2)
	{
		return -1;  // 만석
	}

	return PassengerCount;  // 0 또는 1
}

USceneComponent* ACarriageVehicle::GetSeatComponent(int32 SeatIndex) const
{
	switch (SeatIndex)
	{
	case 0:
		return Seat1;
	case 1:
		return Seat2;
	default:
		return nullptr;
	}
}
