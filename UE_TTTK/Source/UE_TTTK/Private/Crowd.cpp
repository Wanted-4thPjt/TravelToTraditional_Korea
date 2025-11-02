// Fill out your copyright notice in the Description page of Project Settings.


#include "Crowd.h"

#include "CrowdAiController.h"
#include "CrowdTargetPoint.h"
#include "MainPlayer.h"
#include "TTTK_GameState.h"
#include "AssetTypeActions/AssetDefinition_SoundBase.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "Components/StateTreeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ACrowd::ACrowd()
{
 	// 성능 최적화: Tick 비활성화 (필요 없음)
	PrimaryActorTick.bCanEverTick = false;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 플래그 초기화
	bIsOuchAnimCompleted = false;
}

// Called when the game starts or when spawned
void ACrowd::BeginPlay()
{
	Super::BeginPlay();

	// ========== 스켈레탈 메시 최적화 (핵심!) ==========
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (MeshComp)
	{
		// 클로스 완전 비활성화
		MeshComp->ClothMaxDistanceScale = 0.0f;
		MeshComp->bDisableClothSimulation = true;

		
		MeshComp->bEnableUpdateRateOptimizations = true;
		MeshComp->AnimUpdateRateParams = new FAnimUpdateRateParameters();
		MeshComp->AnimUpdateRateParams->ShiftBucket = EUpdateRateShiftBucket::ShiftBucket2;
		MeshComp->AnimUpdateRateParams->bInterpolateSkippedFrames = true;
		MeshComp->AnimUpdateRateParams->UpdateRate = 2; 
		MeshComp->AnimUpdateRateParams->EvaluationRate = 2;

		// LOD 설정 (자동)
		MeshComp->SetForcedLOD(0);

		// 불필요한 기능 비활성화
		MeshComp->bPropagateCurvesToFollowers = false;
	}

	// Marketeer 타입일 때만 AudioComponent 생성 (성능 최적화)
	if (CrowdData && CrowdData->GetCrowdType() == ECrowdType::Marketeer)
	{
		AudioComp = NewObject<UAudioComponent>(this, TEXT("AudioComponent"));
		if (AudioComp)
		{
			AudioComp->RegisterComponent();
			AudioComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			AudioComp->bAutoActivate = false;
			AudioComp->bAllowSpatialization = true;
		}
	}

	// ========== CharacterMovement 최적화 ==========
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (Movement)
	{
		Movement->MaxWalkSpeed = CrowdData->GetWalkSpeed();

		// 이동 방향으로 회전 설정
		Movement->bOrientRotationToMovement = true;  // 이동 방향으로 자동 회전
		Movement->RotationRate = FRotator(0.0f, 540.0f, 0.0f);  // 회전 속도

		// 불필요한 물리 연산 비활성화
		Movement->bEnablePhysicsInteraction = false;  // 물리 객체와 상호작용 안함
		Movement->bUseFlatBaseForFloorChecks = true;  // 바닥 체크 간소화

	}

	// 컨트롤러 회전 사용 안함 (이동 방향으로만 회전)
	bUseControllerRotationYaw = false;

	// ========== CapsuleComponent 최적화 ==========
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (Capsule!=nullptr)
	{
		Capsule->OnComponentHit.AddDynamic(this,&ACrowd::OnCapsuleHit);

		// 콜리전 최적화
		Capsule->bMultiBodyOverlap = false;
		// Capsule Tick은 콜리전에 필요하므로 유지
	}

	CollectingTargetPoints();
	ATTTK_GameState* GameState = Cast<ATTTK_GameState>(GetWorld()->GetGameState());
}

void ACrowd::OnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	
	if (bIsRagdolled)
	{
		return;
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
	{
		return;
	}

	ACrowdAiController* AIController = Cast<ACrowdAiController>(GetController());
	if (!AIController || !AIController->StateTreeComp)
	{
		return;
	}

	// 플레이어와 부딪힌 경우
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		AMainPlayer* HitPlayer = Cast<AMainPlayer>(OtherActor);
		if (HitPlayer && HitPlayer->bIsSwing && HitPlayer->bIsHandfire)
		{
			// 횃불을 휘두르고 있는 상태에서만 레그돌 활성화
			bHitBySwingingTorch = true; // 횃불에 맞았음을 표시
			GetCharacterMovement()->StopMovementImmediately();
			FStateTreeEvent OuchEvent;
			OuchEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.Ouch");
			AIController->StateTreeComp->SendStateTreeEvent(OuchEvent.Tag);
		}
		else
		{
			// 횃불을 휘두르지 않은 상태에서는 일반 Ouch만
			bHitBySwingingTorch = false; // 횃불에 맞지 않았음을 표시
			GetCharacterMovement()->StopMovementImmediately();
			FStateTreeEvent OuchEvent;
			OuchEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.Ouch");
			AIController->StateTreeComp->SendStateTreeEvent(OuchEvent.Tag);
		}
	}
}



// Called every frame
void ACrowd::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACrowd::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

class ACrowdTargetPoint* ACrowd::GetTargetPoint(FString Destination)
{
	if (TargetPoints.Contains(Destination))
	{
		return TargetPoints[Destination];
	}
	return nullptr;
}


class ACrowdTargetPoint* ACrowd::GetTargetByEnum()
{
	if (!CrowdData)
	{
		return nullptr;
	}

	ECrowdType Type = CrowdData->GetCrowdType();

	FString TargetName;
	switch (Type)
	{
	case ECrowdType::Marketeer:
		TargetName = "sijang";
		break;

	case ECrowdType::Solider:
		TargetName = "순찰로";
		break;

	case ECrowdType::Kid:
		TargetName = "엄마 품";
		break;
	}


	ACrowdTargetPoint* Target = GetTargetPoint(TargetName);
	if (!Target)
	{
	}

	return Target;
}

void ACrowd::CollectingTargetPoints()
{
	TArray<class AActor*> crowdTargetPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ACrowdTargetPoint::StaticClass(),crowdTargetPoints);


	for (AActor* crowdTarget : crowdTargetPoints)
	{
		class ACrowdTargetPoint* CrowdTargetPoint = Cast<ACrowdTargetPoint>(crowdTarget);
		if (CrowdTargetPoint!=nullptr)
		{
			TargetPoints.Add(CrowdTargetPoint->targetName,CrowdTargetPoint);
		}
	}

}

void ACrowd::PlayGreeting()
{
	PlayAnimMontage(GreetingMontage);
}

void ACrowd::PlayOuch()
{
	bIsOuchAnimCompleted = false; 
	PlayAnimMontage(OuchAnimMontage);
}
void ACrowd::PlayCheck()
{
	//PlayAnimMontage()
}

bool ACrowd::CheckGoHomeTime()
{
	ATTTK_GameState* TimeState = Cast<ATTTK_GameState>(GetWorld()->GetGameState());
	if (TimeState != nullptr)
	{
		int32 currentHour;
		int32 currentMinute;
		TimeState->GetCurrentTime(currentHour,currentMinute);

		int32 goHomeHour = CrowdData->GetGoHomeTime().X;
		int32 goHomeMinute = CrowdData->GetGoHomeTime().Y;

		
		int32 currentTimeInMinutes = currentHour * 60 + currentMinute;
		int32 goHomeTimeInMinutes = goHomeHour * 60 + goHomeMinute;

		
		if (currentTimeInMinutes >= goHomeTimeInMinutes)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool ACrowd::CheckGoWorkTime()
{
	ATTTK_GameState* TimeState = Cast<ATTTK_GameState>(GetWorld()->GetGameState());
	if (TimeState != nullptr)
	{
		int32 currentHour;
		int32 currentMinute;
		TimeState->GetCurrentTime(currentHour,currentMinute);

		int32 goWorkHour = CrowdData->GetGoWorkTime().X;
		int32 goWorkMinute = CrowdData->GetGoWorkTime().Y;

		int32 currentTimeInMinutes = currentHour * 60 + currentMinute;
		int32 goWorkTimeInMinutes = goWorkHour * 60 + goWorkMinute;

		// >= 비교로 변경 (1초마다 체크해도 놓치지 않음)
		if (currentTimeInMinutes >= goWorkTimeInMinutes)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}
void ACrowd::CheckTime(FTimeOfDayData TimeData)
{

	ACrowdAiController* AiController = Cast<ACrowdAiController>(GetController());
	if (AiController == nullptr || AiController->StateTreeComp == nullptr)
	{
		return;
	}

	// StateTree가 시작되었는지 확인
	if (!AiController->StateTreeComp->IsRunning())
	{
		return;
	}

	if (CheckGoWorkTime())
	{
		FStateTreeEvent GoWorkEvent;
		GoWorkEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.GoWork");
		AiController->StateTreeComp->SendStateTreeEvent(GoWorkEvent);
		ATTTK_GameState* GameState = Cast<ATTTK_GameState>(GetWorld()->GetGameState());
		
		return;
	}

	
}

void ACrowd::SetCrowdCurrentState(FName NewState)
{
	currentState = NewState;
}



void ACrowd::MulitCast_Talk_Implementation(int32 index)
{
	if (!AudioComp)
	{
		return;
	}

	if (!talkSounds.IsValidIndex(index))
	{
		return;
	}

	
	if (AudioComp->IsPlaying())
	{
		AudioComp->Stop();
	}

	
	bIsTalking = true;
	bIsListening = false;

	
	AudioComp->OnAudioFinished.RemoveDynamic(this, &ACrowd::OnAudioFinishedCallback);
	AudioComp->OnAudioFinished.AddDynamic(this, &ACrowd::OnAudioFinishedCallback);

	
	AudioComp->SetSound(talkSounds[index]);
	AudioComp->Play();


	
	if (HasAuthority())
	{
		OnTalkStarted(this);
	}
}

void ACrowd::OnTalkStarted(ACrowd* lastTalker)
{
	if (CrowdData->GetCrowdType() == ECrowdType::Marketeer)
	{
		GoWorkTargetPoint -> SetCurrentTalkerCrowd(lastTalker);
	}
}

void ACrowd::OnAudioFinishedCallback()
{
	
	bIsTalking = false;
}

void ACrowd::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACrowd, bIsTalking);
	DOREPLIFETIME(ACrowd, bIsListening);
}

void ACrowd::EnableRagdoll()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		return;
	}

	// 레그돌 상태 플래그 설정 (영구적으로 유지)
	bIsRagdolled = true;

	// 캐릭터 이동 비활성화
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->DisableMovement();
		MovementComp->SetComponentTickEnabled(false);
	}

	// 캡슐 콜라이더 비활성화
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 레그돌 활성화
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetAllBodiesSimulatePhysics(true);

}

void ACrowd::DisableRagdoll()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		return;
	}

	// 레그돌 비활성화
	MeshComp->SetSimulatePhysics(false);
	MeshComp->SetAllBodiesSimulatePhysics(false);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 캡슐 콜라이더 재활성화
	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	// 위치 및 회전 초기화
	MeshComp->SetRelativeLocation(FVector(0.f, 0.f, -90.f)); // 기본 위치로 (필요에 따라 조정)
	MeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f)); // 기본 회전으로 (필요에 따라 조정)
	MeshComp->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);

	// 캐릭터 이동 재활성화
	if (UCharacterMovementComponent* MovementComp = GetCharacterMovement())
	{
		MovementComp->SetMovementMode(MOVE_Walking);
		MovementComp->SetComponentTickEnabled(true);
	}

}







