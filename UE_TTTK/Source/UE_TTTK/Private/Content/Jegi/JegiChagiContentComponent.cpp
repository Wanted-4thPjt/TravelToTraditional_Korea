// Fill out your copyright notice in the Description page of Project Settings.

#include "Content/Jegi/JegiChagiContentComponent.h"
#include "GameFlow/JegiChagiContentManager.h"
#include "Content/Jegi/Jegi.h"
#include "EnhancedInputComponent.h"
#include "MainPlayer.h"
#include "Data/InputMappingsSettings.h"
#include "Kismet/GameplayStatics.h"

UJegiChagiContentComponent::UJegiChagiContentComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;  // 활성화 시에만 Tick

	// InputMappingsSettings에서 로드
	if (const FInputMappingData* data = UInputMappingsSettings::Get()->inputMappings.Find("IMC_JegiChagi"))
	{
		inputMappingContext = data->inputMappingContext;
		IA_Kick_L = data->inputActions["IA_Kick_L"];
		IA_Kick_R = data->inputActions["IA_Kick_R"];
		IA_MoveLeft = data->inputActions["IA_MoveLeft"];
		IA_MoveRight = data->inputActions["IA_MoveRight"];
	}
}

void UJegiChagiContentComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                               FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bContentActive || !myJegi) return;

	// Client에서 제기 물리 시뮬레이션
	UpdateJegiPhysics(DeltaTime);

	// 땅에 닿았는지 체크
	if (myJegi->GetActorLocation().Z <= 0.f)
	{
		OnJegiHitGround();
	}

	// 경계 이탈 체크
	CheckOutOfBounds();
}

void UJegiChagiContentComponent::ActivateContentInput_Implementation(UBaseContentManager* manager)
{
	Super::ActivateContentInput_Implementation(manager);

	jegiManager = Cast<UJegiChagiContentManager>(manager);
	if (!jegiManager)
	{
		UE_LOG(LogTemp, Error, TEXT("JegiChagiContentComponent: Manager is not UJegiChagiContentManager"));
		return;
	}

	// Tick 활성화
	SetComponentTickEnabled(true);

	UE_LOG(LogTemp, Log, TEXT("JegiChagiContentComponent: Activated"));
}

void UJegiChagiContentComponent::DeactivateContentInput_Implementation()
{
	Super::DeactivateContentInput_Implementation();

	// 상태 초기화
	jegiManager = nullptr;
	myJegi = nullptr;
	localCombo = 0;
	localMaxCombo = 0;
	localPerfectCount = 0;
	localGreatCount = 0;
	localGoodCount = 0;
	localMissedCount = 0;
	horizontalPosition = 0.f;
	jegiVelocity = FVector::ZeroVector;

	// Tick 비활성화
	SetComponentTickEnabled(false);

	UE_LOG(LogTemp, Log, TEXT("JegiChagiContentComponent: Deactivated"));
}

void UJegiChagiContentComponent::SetupInputBindings_Implementation(UEnhancedInputComponent* InputComponent)
{
	Super::SetupInputBindings_Implementation(InputComponent);

	if (!InputComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("JegiChagiContentComponent: SetupInputBindings called with null InputComponent"));
		return;
	}
	
	// Input Action 바인딩
	if (IA_Kick_L)
	{
		InputComponent->BindAction(IA_Kick_L, ETriggerEvent::Started, this, &UJegiChagiContentComponent::OnKickLeftInput);
	}

	if (IA_Kick_R)
	{
		InputComponent->BindAction(IA_Kick_R, ETriggerEvent::Started, this, &UJegiChagiContentComponent::OnKickRightInput);
	}
	
	if (IA_MoveLeft)
	{
		InputComponent->BindAction(IA_MoveLeft, ETriggerEvent::Triggered, this, &UJegiChagiContentComponent::OnMoveLeftInput);
	}

	if (IA_MoveRight)
	{
		InputComponent->BindAction(IA_MoveRight, ETriggerEvent::Triggered, this, &UJegiChagiContentComponent::OnMoveRightInput);
	}

	UE_LOG(LogTemp, Log, TEXT("JegiChagiContentComponent: Input bindings set up"));
}

void UJegiChagiContentComponent::SetMyJegi(AJegi* jegi)
{
	myJegi = jegi;
	if (myJegi)
	{
		lastJegiPosition = myJegi->GetActorLocation();
		jegiVelocity = FVector::ZeroVector;
		UE_LOG(LogTemp, Log, TEXT("JegiChagiContentComponent: Jegi set to %s"), *myJegi->GetName());
	}
}

void UJegiChagiContentComponent::StartRound()
{
	// 라운드 상태 초기화
	localCombo = 0;
	localMaxCombo = 0;
	localPerfectCount = 0;
	localGreatCount = 0;
	localGoodCount = 0;
	localMissedCount = 0;
	horizontalPosition = 0.f;
	roundStartTime = GetWorld()->GetTimeSeconds();
	lastKickTime = 0.f;

	// 제기 초기화
	if (myJegi)
	{
		FVector initialLocation = myJegi->GetActorLocation();
		initialLocation.Z = optimalKickHeight;
		myJegi->SetActorLocation(initialLocation);
		lastJegiPosition = initialLocation;
		jegiVelocity = FVector::ZeroVector;
	}

	UE_LOG(LogTemp, Log, TEXT("JegiChagiContentComponent: Round started"));
}

void UJegiChagiContentComponent::OnKickRightInput()
{
	if (!bContentActive || !myJegi) return;

	// AnimMontage 재생
	if (AMainPlayer* player = Cast<AMainPlayer>(GetOwner()))
	{
		if (kickRightMontage)
		{
			player->PlayAnimMontage(kickRightMontage);
			UE_LOG(LogTemp, Verbose, TEXT("JegiChagiContentComponent: Playing Right Kick Montage"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("JegiChagiContentComponent: kickRightMontage is not set!"));
			// Montage가 없으면 즉시 호출
			OnAnimNotify_KickContact();
		}
	}
}

void UJegiChagiContentComponent::OnKickLeftInput()
{
	if (!bContentActive || !myJegi) return;

	// AnimMontage 재생
	if (AMainPlayer* player = Cast<AMainPlayer>(GetOwner()))
	{
		if (kickLeftMontage)
		{
			player->PlayAnimMontage(kickLeftMontage);
			UE_LOG(LogTemp, Verbose, TEXT("JegiChagiContentComponent: Playing Left Kick Montage"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("JegiChagiContentComponent: kickLeftMontage is not set!"));
			// Montage가 없으면 즉시 호출
			OnAnimNotify_KickContact();
		}
	}
}

// ========== AnimNotify Callback ==========

void UJegiChagiContentComponent::OnAnimNotify_KickContact()
{
	if (!bContentActive || !myJegi)
	{
		UE_LOG(LogTemp, Warning, TEXT("JegiChagiContentComponent: KickContact called but not active or no Jegi"));
		return;
	}

	// 1. 로컬에서 즉시 타이밍 판정
	EKickTiming timing = CalculateLocalTiming();

	// 2. 로컬 콤보 업데이트
	UpdateLocalCombo(timing);

	// 3. 즉시 피드백 (지연 없음!)
	PlayLocalFeedback(timing);

	// 4. 제기에 힘 가하기 (로컬)
	if (timing != EKickTiming::Missed)
	{
		// 위로 킥
		jegiVelocity.Z = kickForce;

		// 약간 위로 이동 (시각적 효과)
		FVector newLocation = myJegi->GetActorLocation();
		newLocation.Z += 10.f;
		myJegi->SetActorLocation(newLocation);
		lastJegiPosition = newLocation;
	}

	// 5. Server에 결과만 전송 (비동기, 논블로킹)
	float timestamp = GetWorld()->GetTimeSeconds();
	Server_ReportKick(timing, timestamp);

	lastKickTime = timestamp;

	UE_LOG(LogTemp, Verbose, TEXT("JegiChagiContentComponent: Kick Contact - Timing: %s, Combo: %d"),
	       *UEnum::GetValueAsString(timing), localCombo);
}

void UJegiChagiContentComponent::OnMoveLeftInput()
{
	if (!bContentActive || !myJegi) return;

	horizontalPosition -= moveSpeed;
	horizontalPosition = FMath::Clamp(horizontalPosition, -1.0f, 1.0f);

	// 제기 위치 업데이트 (로컬)
	FVector newLocation = myJegi->GetActorLocation();
	newLocation.Y = horizontalPosition * maxHorizontalRange;
	myJegi->SetActorLocation(newLocation);
	lastJegiPosition = newLocation;

	// 캐릭터도 같이 이동 (선택적)
	if (AMainPlayer* player = Cast<AMainPlayer>(GetOwner()))
	{
		FVector playerLocation = player->GetActorLocation();
		playerLocation.Y = newLocation.Y;
		player->SetActorLocation(playerLocation);
	}
}

void UJegiChagiContentComponent::OnMoveRightInput()
{
	if (!bContentActive || !myJegi) return;

	horizontalPosition += moveSpeed;
	horizontalPosition = FMath::Clamp(horizontalPosition, -1.0f, 1.0f);

	// 제기 위치 업데이트 (로컬)
	FVector newLocation = myJegi->GetActorLocation();
	newLocation.Y = horizontalPosition * maxHorizontalRange;
	myJegi->SetActorLocation(newLocation);
	lastJegiPosition = newLocation;

	// 캐릭터도 같이 이동 (선택적)
	if (AMainPlayer* player = Cast<AMainPlayer>(GetOwner()))
	{
		FVector playerLocation = player->GetActorLocation();
		playerLocation.Y = newLocation.Y;
		player->SetActorLocation(playerLocation);
	}
}

// ========== Client-Side 게임 로직 ==========

EKickTiming UJegiChagiContentComponent::CalculateLocalTiming()
{
	if (!myJegi) return EKickTiming::Missed;

	// 제기의 현재 높이로 판정
	float jegiHeight = myJegi->GetActorLocation().Z;
	float heightDiff = FMath::Abs(jegiHeight - optimalKickHeight);

	if (heightDiff < perfectWindow * 100.f)
		return EKickTiming::Perfect;
	else if (heightDiff < greatWindow * 100.f)
		return EKickTiming::Great;
	else if (heightDiff < goodWindow * 100.f)
		return EKickTiming::Good;
	else
		return EKickTiming::Missed;
}

void UJegiChagiContentComponent::UpdateLocalCombo(EKickTiming timing)
{
	if (timing == EKickTiming::Missed)
	{
		localCombo = 0;
		localMissedCount++;
	}
	else
	{
		localCombo++;
		if (localCombo > localMaxCombo)
		{
			localMaxCombo = localCombo;
		}

		switch (timing)
		{
		case EKickTiming::Perfect:
			localPerfectCount++;
			break;
		case EKickTiming::Great:
			localGreatCount++;
			break;
		case EKickTiming::Good:
			localGoodCount++;
			break;
		default:
			break;
		}
	}

	// UI 업데이트 (즉시!)
	// TODO: Update UI Widget
	UE_LOG(LogTemp, Log, TEXT("[CLIENT] Combo: %d, Max: %d, P:%d G:%d G:%d M:%d"),
	       localCombo, localMaxCombo, localPerfectCount, localGreatCount, localGoodCount, localMissedCount);
}

void UJegiChagiContentComponent::PlayLocalFeedback(EKickTiming timing)
{
	// 이펙트/사운드 재생 (즉시!)
	switch (timing)
	{
	case EKickTiming::Perfect:
		UE_LOG(LogTemp, Warning, TEXT("⭐ PERFECT! Combo: %d"), localCombo);
		// TODO: Play Perfect Particle, Sound, Camera Shake
		break;

	case EKickTiming::Great:
		UE_LOG(LogTemp, Warning, TEXT("👍 GREAT! Combo: %d"), localCombo);
		// TODO: Play Great effects
		break;

	case EKickTiming::Good:
		UE_LOG(LogTemp, Warning, TEXT("✓ GOOD Combo: %d"), localCombo);
		// TODO: Play Good effects
		break;

	case EKickTiming::Missed:
		UE_LOG(LogTemp, Error, TEXT("❌ MISSED - Combo Reset"));
		// TODO: Play Missed effects
		break;

	default:
		break;
	}
}

void UJegiChagiContentComponent::UpdateJegiPhysics(float deltaTime)
{
	if (!myJegi) return;

	// 콤보에 따라 중력 증가
	float gravityScale = 1.0f + (localCombo / comboPerGravityIncrease) * comboGravityIncrement;
	FVector gravity = FVector(0, 0, -baseGravity * gravityScale) * deltaTime;

	// 속도 업데이트
	jegiVelocity += gravity;

	// 위치 업데이트
	FVector newLocation = myJegi->GetActorLocation() + jegiVelocity * deltaTime;
	myJegi->SetActorLocation(newLocation);

	lastJegiPosition = newLocation;
}

void UJegiChagiContentComponent::OnJegiHitGround()
{
	if (!bContentActive) return;

	// 라운드 종료!
	bContentActive = false;
	SetComponentTickEnabled(false);

	// 최종 결과 생성
	FJegiRoundRecord result;
	result.maxCombo = localMaxCombo;
	result.perfectCount = localPerfectCount;
	result.greatCount = localGreatCount;
	result.goodCount = localGoodCount;
	result.missedCount = localMissedCount;
	result.totalKicks = localPerfectCount + localGreatCount + localGoodCount + localMissedCount;
	result.playTime = GetWorld()->GetTimeSeconds() - roundStartTime;

	// Server에 최종 결과 제출
	Server_SubmitRoundResult(result);
	Server_ReportJegiHitGround();

	UE_LOG(LogTemp, Log, TEXT("[CLIENT] Round Finished! Max Combo: %d, Total Kicks: %d, Time: %.2f"),
	       result.maxCombo, result.totalKicks, result.playTime);
}

void UJegiChagiContentComponent::CheckOutOfBounds()
{
	if (FMath::Abs(horizontalPosition) >= 1.0f)
	{
		// 경계 이탈!
		bContentActive = false;
		SetComponentTickEnabled(false);

		// Server에 보고
		Server_ReportOutOfBounds();

		UE_LOG(LogTemp, Warning, TEXT("[CLIENT] Out of bounds! Position: %.2f"), horizontalPosition);
	}
}

// ========== Server RPC ==========

void UJegiChagiContentComponent::Server_ReportKick_Implementation(EKickTiming timing, float timestamp)
{
	if (!jegiManager) return;

	// 간단한 검증
	bool isValid = true;

	// 1. 시간 검증 (너무 빠르게 연속 킥 불가)
	if (lastKickTime > 0.f && timestamp - lastKickTime < 0.05f)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SERVER] Kick too fast! Interval: %.3f"), timestamp - lastKickTime);
		isValid = false;
	}

	// 2. 타이밍 검증 (통계적 이상치 체크)
	int32 totalHits = localPerfectCount + localGreatCount + localGoodCount;
	if (totalHits > 0)
	{
		float perfectRatio = (float)localPerfectCount / totalHits;
		if (perfectRatio > 0.95f && localPerfectCount > 20)
		{
			UE_LOG(LogTemp, Warning, TEXT("[SERVER] Suspiciously high perfect ratio: %.2f%%"), perfectRatio * 100.f);
			// 경고만, 차단은 안 함 (실력 좋을 수도)
		}
	}

	if (isValid)
	{
		// Manager에 알림 (통계용)
		if (APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController()))
		{
			jegiManager->OnPlayerKickReported(PC, timing);
		}
	}
	else
	{
		// 검증 실패 - Client에 알림
		Client_OnValidationFailed(TEXT("Invalid kick timing detected"));
	}

	lastKickTime = timestamp;
}

void UJegiChagiContentComponent::Server_ReportOutOfBounds_Implementation()
{
	if (!jegiManager) return;

	APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	if (!PC) return;

	UE_LOG(LogTemp, Log, TEXT("[SERVER] Player %s reported out of bounds"), *PC->GetName());

	// 실패 처리
	FJegiRoundRecord result;
	result.maxCombo = localMaxCombo;
	result.perfectCount = localPerfectCount;
	result.greatCount = localGreatCount;
	result.goodCount = localGoodCount;
	result.missedCount = localMissedCount;
	result.totalKicks = localPerfectCount + localGreatCount + localGoodCount + localMissedCount;
	result.playTime = GetWorld()->GetTimeSeconds() - roundStartTime;

	jegiManager->SaveRoundResult(PC, result, false);  // false = 실패
}

void UJegiChagiContentComponent::Server_ReportJegiHitGround_Implementation()
{
	if (!jegiManager) return;

	APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	if (!PC) return;

	UE_LOG(LogTemp, Log, TEXT("[SERVER] Player %s jegi hit ground"), *PC->GetName());

	jegiManager->OnJegiHitGroundReported(PC);
}

void UJegiChagiContentComponent::Server_SubmitRoundResult_Implementation(const FJegiRoundRecord& result)
{
	if (!jegiManager) return;

	APlayerController* PC = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	if (!PC) return;

	// 최종 결과 검증
	bool isValid = true;
	FJegiRoundRecord validatedResult = result;

	// 1. 물리적 가능성 검증
	if (result.playTime > 0.f)
	{
		float kickRate = result.totalKicks / result.playTime;
		if (kickRate > 10.f)  // 1초에 10번 이상 킥 불가
		{
			UE_LOG(LogTemp, Error, TEXT("[SERVER] Impossible kick rate: %.2f kicks/sec"), kickRate);
			isValid = false;
		}
	}

	// 2. 콤보 검증
	if (result.maxCombo > result.totalKicks)
	{
		UE_LOG(LogTemp, Error, TEXT("[SERVER] Impossible combo: %d > %d"), result.maxCombo, result.totalKicks);
		validatedResult.maxCombo = result.totalKicks;
		isValid = false;
	}

	// 3. 카운트 검증
	int32 totalCount = result.perfectCount + result.greatCount + result.goodCount + result.missedCount;
	if (totalCount != result.totalKicks)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SERVER] Count mismatch: %d vs %d"), totalCount, result.totalKicks);
		validatedResult.totalKicks = totalCount;
	}

	if (isValid)
	{
		// 검증 통과 - 저장
		jegiManager->SaveRoundResult(PC, validatedResult, true);
		UE_LOG(LogTemp, Log, TEXT("[SERVER] Round result validated and saved for %s"), *PC->GetName());
	}
	else
	{
		// 검증 실패 - 정정된 값 저장 및 Client에 알림
		jegiManager->SaveRoundResult(PC, validatedResult, true);
		Client_OnScoreCorrected(validatedResult.maxCombo, validatedResult.maxCombo);
		UE_LOG(LogTemp, Warning, TEXT("[SERVER] Round result corrected for %s"), *PC->GetName());
	}
}

// ========== Client RPC ==========

void UJegiChagiContentComponent::Client_OnValidationFailed_Implementation(const FString& reason)
{
	UE_LOG(LogTemp, Error, TEXT("[CLIENT] Server validation failed: %s"), *reason);
	// TODO: UI에 경고 표시
}

void UJegiChagiContentComponent::Client_OnScoreCorrected_Implementation(int32 correctedCombo, int32 correctedMaxCombo)
{
	UE_LOG(LogTemp, Warning, TEXT("[CLIENT] Score corrected by server: %d → %d"), localMaxCombo, correctedMaxCombo);

	localCombo = correctedCombo;
	localMaxCombo = correctedMaxCombo;

	// TODO: UI 업데이트
}
