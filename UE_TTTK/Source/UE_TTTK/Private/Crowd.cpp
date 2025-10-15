// Fill out your copyright notice in the Description page of Project Settings.


#include "Crowd.h"

#include "CrowdTargetPoint.h"
#include "Components/StateTreeComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACrowd::ACrowd()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	StateTreeComp = CreateDefaultSubobject<UStateTreeComponent>(TEXT("StateTreeComponet"));
	
}

// Called when the game starts or when spawned
void ACrowd::BeginPlay()
{
	Super::BeginPlay();
	CollectingTargetPoints();
	if (StateTreeComp != nullptr)
	{

		UE_LOG(LogTemp,Warning,TEXT("시작시작시작시작"));
		StateTreeComp->StartLogic();
	}
	UE_LOG(LogTemp,Warning,TEXT("hihi"));
	
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

