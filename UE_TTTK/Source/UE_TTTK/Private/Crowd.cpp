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
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// AudioComponent 생성 및 RootComponent에 부착
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	if (AudioComp)
	{
		AudioComp->SetupAttachment(RootComponent);
		AudioComp->bAutoActivate = false;// 자동 재생 비활성화
		AudioComp->bAllowSpatialization = true; //시장 아줌마 거리에 따라서 다르게 들리도록 
		
	}

	// 플래그 초기화
	bIsOuchAnimCompleted = false;
}

// Called when the game starts or when spawned
void ACrowd::BeginPlay()
{
	Super::BeginPlay();
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (Capsule!=nullptr)
	{
		Capsule->OnComponentHit.AddDynamic(this,&ACrowd::OnCapsuleHit);
	}
	CollectingTargetPoints();
	ATTTK_GameState* GameState = Cast<ATTTK_GameState>(GetWorld()->GetGameState());
	GetCharacterMovement()->MaxWalkSpeed = CrowdData->GetWalkSpeed();
	
	
	
}

void ACrowd::OnCapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// 이미 레그돌 상태인 경우 아무 것도 하지 않음
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
			UE_LOG(LogTemp, Warning, TEXT("[%s] 횃불에 맞아 레그돌 활성화!"), *GetName());
			bHitBySwingingTorch = true; // 횃불에 맞았음을 표시
			GetCharacterMovement()->StopMovementImmediately();
			FStateTreeEvent OuchEvent;
			OuchEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.Ouch");
			AIController->StateTreeComp->SendStateTreeEvent(OuchEvent.Tag);
		}
		else
		{
			// 횃불을 휘두르지 않은 상태에서는 일반 Ouch만
			UE_LOG(LogTemp, Warning, TEXT("[%s] 플레이어와 부딪힘 (횃불 휘두르기 아님)"), *GetName());
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
		UE_LOG(LogTemp, Error, TEXT("[%s] CrowdData가 nullptr!"), *GetName());
		return nullptr;
	}

	ECrowdType Type = CrowdData->GetCrowdType();
	UE_LOG(LogTemp, Warning, TEXT("[%s] CrowdType: %d"), *GetName(), (int32)Type);

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

	UE_LOG(LogTemp, Warning, TEXT("[%s] 찾는 타겟 이름: %s"), *GetName(), *TargetName);

	ACrowdTargetPoint* Target = GetTargetPoint(TargetName);
	if (!Target)
	{
		UE_LOG(LogTemp, Error, TEXT("[%s] '%s' 타겟을 찾을 수 없음!"), *GetName(), *TargetName);
	}

	return Target;
}

void ACrowd::CollectingTargetPoints()
{
	TArray<class AActor*> crowdTargetPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ACrowdTargetPoint::StaticClass(),crowdTargetPoints);

	UE_LOG(LogTemp, Warning, TEXT("[%s] TargetPoint 총 개수: %d"), *GetName(), crowdTargetPoints.Num());

	for (AActor* crowdTarget : crowdTargetPoints)
	{
		class ACrowdTargetPoint* CrowdTargetPoint = Cast<ACrowdTargetPoint>(crowdTarget);
		if (CrowdTargetPoint!=nullptr)
		{
			TargetPoints.Add(CrowdTargetPoint->targetName,CrowdTargetPoint);
			UE_LOG(LogTemp, Warning, TEXT("[%s] 추가: %s"), *GetName(), *CrowdTargetPoint->targetName);
		}
	}

}

void ACrowd::PlayGreeting()
{
	PlayAnimMontage(GreetingMontage);
}

void ACrowd::PlayOuch()
{
	UE_LOG(LogTemp,Warning,TEXT("Ouch재생"));
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
		if (currentHour == CrowdData->GetGoWorkTime().X && currentMinute == CrowdData->GetGoWorkTime().Y)
		{
			UE_LOG(LogTemp,Warning,TEXT("%d 시 %d 분"),currentHour,currentMinute);
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
	UE_LOG(LogTemp,Warning,TEXT("시간 체크 호출"));

	ACrowdAiController* AiController = Cast<ACrowdAiController>(GetController());
	if (AiController == nullptr || AiController->StateTreeComp == nullptr)
	{
		UE_LOG(LogTemp,Warning,TEXT("AIController 또는 StateTreeComp가 없음"));
		return;
	}

	// StateTree가 시작되었는지 확인
	if (!AiController->StateTreeComp->IsRunning())
	{
		UE_LOG(LogTemp,Warning,TEXT("StateTree가 아직 시작되지 않음"));
		return;
	}

	if (CheckGoWorkTime())
	{
		UE_LOG(LogTemp,Warning,TEXT("일하러 갈 시간"));
		FStateTreeEvent GoWorkEvent;
		GoWorkEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.GoWork");
		AiController->StateTreeComp->SendStateTreeEvent(GoWorkEvent);
		ATTTK_GameState* GameState = Cast<ATTTK_GameState>(GetWorld()->GetGameState());
		UE_LOG(LogTemp,Warning,TEXT("일하러 가는 이벤트 추가"));
		
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
		UE_LOG(LogDialogue, Error, TEXT("[Crowd::Talk] AudioComp가 nullptr!"));
		return;
	}

	if (!talkSounds.IsValidIndex(index))
	{
		UE_LOG(LogDialogue, Error, TEXT("[Crowd::Talk] 유효하지 않은 사운드 인덱스: %d"), index);
		return;
	}

	
	if (AudioComp->IsPlaying())
	{
		AudioComp->Stop();
	}

	
	bIsTalking = true;
	bIsListening = false;
	UE_LOG(LogDialogue, Display, TEXT("[Crowd::Talk] 상태 변경 - bIsTalking: true, bIsListening: false"));

	
	AudioComp->OnAudioFinished.RemoveDynamic(this, &ACrowd::OnAudioFinishedCallback);
	AudioComp->OnAudioFinished.AddDynamic(this, &ACrowd::OnAudioFinishedCallback);

	
	AudioComp->SetSound(talkSounds[index]);
	AudioComp->Play();

	UE_LOG(LogDialogue, Display, TEXT("[Crowd::Talk] 사운드 재생 시작 - Index: %d, Actor: %s"), index, *GetName());

	
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
	UE_LOG(LogDialogue, Display, TEXT("[Crowd::OnAudioFinishedCallback] 음성 종료 - bIsTalking: false, Actor: %s"), *GetName());
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
		UE_LOG(LogTemp, Error, TEXT("[Crowd::EnableRagdoll] Mesh가 nullptr!"));
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

	UE_LOG(LogTemp, Warning, TEXT("[Crowd::EnableRagdoll] 레그돌 활성화: %s"), *GetName());
}

void ACrowd::DisableRagdoll()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp)
	{
		UE_LOG(LogTemp, Error, TEXT("[Crowd::DisableRagdoll] Mesh가 nullptr!"));
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

	UE_LOG(LogTemp, Warning, TEXT("[Crowd::DisableRagdoll] 레그돌 비활성화: %s"), *GetName());
}







