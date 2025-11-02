// Fill out your copyright notice in the Description page of Project Settings.

#include "CompassManager.h"
#include "GameFlow/HeritageDiscoveryManager.h"
#include "Interaction/HeritageObjectComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

ACompassManager::ACompassManager()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ACompassManager::BeginPlay()
{
	Super::BeginPlay();

	// HeritageDiscoveryManager 가져오기
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		DiscoveryManager = GameInstance->GetSubsystem<UHeritageDiscoveryManager>();
	}

	// 디버그 시각화 활성화
	if (bShowDebugArrows)
	{
		SetActorTickEnabled(true);
	}
}

TArray<FCompassHeritageInfo> ACompassManager::GetHeritagesForPlayer(APlayerController* Player) const
{
	TArray<FCompassHeritageInfo> Result;

	if (!Player || !GetWorld())
	{
		return Result;
	}

	APawn* PlayerPawn = Player->GetPawn();
	if (!PlayerPawn)
	{
		return Result;
	}

	// 플레이어 위치와 방향
	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector PlayerForward = PlayerPawn->GetActorForwardVector();

	// 2D 평면에서만 계산 (Z축 무시)
	PlayerForward.Z = 0.0f;
	PlayerForward.Normalize();

	// 월드의 모든 액터를 순회하면서 HeritageObjectComponent 찾기
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor)
		{
			continue;
		}

		// HeritageObjectComponent 찾기
		UHeritageObjectComponent* HeritageComp = Actor->FindComponentByClass<UHeritageObjectComponent>();
		if (HeritageComp && !HeritageComp->HeritageObjectID.IsEmpty())
		{
			// 이미 발견한 유물은 제외
			if (HeritageComp->HasPlayerDiscovered(Player))
			{
				continue;
			}

			FVector HeritageLocation = Actor->GetActorLocation();

			// 거리 계산
			float Distance = FVector::Dist(PlayerLocation, HeritageLocation);

			// 최대 거리 체크
			if (Distance > MaxDisplayDistance)
			{
				continue;
			}

			// 플레이어 -> 유물 방향 벡터 (2D)
			FVector ToHeritage = HeritageLocation - PlayerLocation;
			ToHeritage.Z = 0.0f;
			ToHeritage.Normalize();

			// 각도 계산 (-180 ~ 180)
			// Forward를 기준으로 오른쪽이 +, 왼쪽이 -
			float DotProduct = FVector::DotProduct(PlayerForward, ToHeritage);
			float CrossProductZ = FVector::CrossProduct(PlayerForward, ToHeritage).Z;

			// Atan2를 사용해서 각도 계산
			float AngleRadians = FMath::Atan2(CrossProductZ, DotProduct);
			float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

			// 결과 추가
			FCompassHeritageInfo Info;
			Info.HeritageID = HeritageComp->HeritageObjectID;
			Info.WorldLocation = HeritageLocation;
			Info.Distance = Distance;
			Info.AngleDegrees = AngleDegrees;

			Result.Add(Info);
		}
	}

	return Result;
}

int32 ACompassManager::GetTotalHeritageCount() const
{
	if (!GetWorld())
	{
		return 0;
	}

	int32 Count = 0;

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor)
		{
			continue;
		}

		UHeritageObjectComponent* HeritageComp = Actor->FindComponentByClass<UHeritageObjectComponent>();
		if (HeritageComp && !HeritageComp->HeritageObjectID.IsEmpty())
		{
			Count++;
		}
	}

	return Count;
}

void ACompassManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bShowDebugArrows || !GetWorld())
	{
		return;
	}

	// 플레이어 컨트롤러 가져오기
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	APawn* PlayerPawn = PC->GetPawn();
	if (!PlayerPawn)
	{
		return;
	}

	FVector PlayerLocation = PlayerPawn->GetActorLocation();
	FVector PlayerForward = PlayerPawn->GetActorForwardVector();

	// 플레이어 정면 방향 표시 (파란색 화살표)
	DrawDebugDirectionalArrow(
		GetWorld(),
		PlayerLocation + FVector(0, 0, 100),
		PlayerLocation + PlayerForward * 500.0f + FVector(0, 0, 100),
		100.0f,
		FColor::Blue,
		false,
		-1.0f,
		0,
		10.0f
	);

	// 모든 유물 위치에 화살표 표시
	TArray<FCompassHeritageInfo> Heritages = GetHeritagesForPlayer(PC);
	for (const FCompassHeritageInfo& Info : Heritages)
	{
		// 유물 위치에 구체
		DrawDebugSphere(GetWorld(), Info.WorldLocation, 100.0f, 12, FColor::Yellow, false, -1.0f, 0, 5.0f);

		// 플레이어 -> 유물 방향 화살표 (녹색)
		FVector Direction = (Info.WorldLocation - PlayerLocation).GetSafeNormal();
		DrawDebugDirectionalArrow(
			GetWorld(),
			PlayerLocation + FVector(0, 0, 50),
			PlayerLocation + Direction * FMath::Min(Info.Distance * 0.5f, 1000.0f) + FVector(0, 0, 50),
			50.0f,
			FColor::Green,
			false,
			-1.0f,
			0,
			5.0f
		);

		// 각도 텍스트 표시
		FString AngleText = FString::Printf(TEXT("%.1f°\n%.0fm"), Info.AngleDegrees, Info.Distance / 100.0f);
		DrawDebugString(GetWorld(), Info.WorldLocation + FVector(0, 0, 150), AngleText, nullptr, FColor::White, 0.0f, true);
	}
}
