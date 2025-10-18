// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdAiController.h"

#include "Crowd.h"
#include "Components/StateTreeAIComponent.h"

ACrowdAiController::ACrowdAiController()
{
	StateTreeComp= CreateDefaultSubobject<UStateTreeAIComponent>("StateTreeAI");
}

void ACrowdAiController::OnPossess(APawn* InPawn)
{
	UE_LOG(LogTemp, Warning, TEXT("=== CrowdAiController::OnPossess 호출됨 ==="));
	UE_LOG(LogTemp, Warning, TEXT("Possessing Pawn: %s"), *InPawn->GetName());

	Super::OnPossess(InPawn);

	OwnerCrowd = Cast<ACrowd>(InPawn);
	UE_LOG(LogTemp, Warning, TEXT("OwnerCrowd Cast: %s"), OwnerCrowd ? TEXT("OK") : TEXT("FAILED"));

	if (StateTreeComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("StateTreeComp 존재함 - StartLogic 호출"));
		StateTreeComp->StartLogic();
		UE_LOG(LogTemp, Warning, TEXT("StartLogic 완료"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("StateTreeComp가 nullptr! StateTree가 실행되지 않습니다!"));
	}
}

void ACrowdAiController::WalkToPoint(AActor* TargetActor)
{
	MoveToActor(TargetActor);
}

void ACrowdAiController::WalkToLocation(FVector targetLocation)
{
	UE_LOG(LogTemp, Warning, TEXT("=== WalkToLocation 호출됨 ==="));
	UE_LOG(LogTemp, Warning, TEXT("목표 위치: %s"), *targetLocation.ToString());

	MoveToLocation(targetLocation);
}
void ACrowdAiController::NotifyArrived()
{
	
}


void ACrowdAiController::Talk(ACrowd* crowd)
{
	//crowd->Talk()
}


void ACrowdAiController::GoMarket()
{
}

void ACrowdAiController::GoHome()
{
}

void ACrowdAiController::Greeting()
{
	OwnerCrowd->PlayGreeting();
}
