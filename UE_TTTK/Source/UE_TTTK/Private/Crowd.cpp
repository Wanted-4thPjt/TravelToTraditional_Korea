// Fill out your copyright notice in the Description page of Project Settings.


#include "Crowd.h"

#include "CrowdAiController.h"
#include "CrowdTargetPoint.h"
#include "TTTK_GameState.h"
#include "AssetTypeActions/AssetDefinition_SoundBase.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StateTreeAIComponent.h"
#include "Components/StateTreeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACrowd::ACrowd()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimInstance->IsAnyMontagePlaying())
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("===플레이어랑 부딫혓다===="));
	if (OtherActor->ActorHasTag("Player"))
	{
		ACrowdAiController* AIController = Cast<ACrowdAiController>(GetController());
		if (AIController!=nullptr)
		{
			GetCharacterMovement()->StopMovementImmediately();
			FStateTreeEvent OuchEvent;
			OuchEvent.Tag = FGameplayTag::RequestGameplayTag("Crowd.Event.Ouch");
			AIController->StateTreeComp->SendStateTreeEvent(OuchEvent.Tag);
			UE_LOG(LogTemp, Warning, TEXT("===event전송===="));
		}
		
		UE_LOG(LogTemp,Warning,TEXT("PlayerBeginOverlap"));
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
	bIsOuchAnimCompleted = false; // 플래그 리셋
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
		if (currentHour == CrowdData->GetGoHomeTime().X && currentMinute == CrowdData->GetGoHomeTime().Y)
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

void ACrowd::Talk(int32 index)
{
	AudioComp = UGameplayStatics::SpawnSoundAtLocation(
	GetWorld(),
	talkSounds[index],
	GetActorLocation());
	OnTalkStarted(this);
	
	
}

void ACrowd::OnTalkStarted(ACrowd* lastTalker)
{
	if (CrowdData->GetCrowdType() == ECrowdType::Marketeer)
	{
		GoWorkTargetPoint -> SetCurrentTalkerCrowd(lastTalker);
	}
}







