// Fill out your copyright notice in the Description page of Project Settings.


#include "Carriage/CarriageMovementComponent.h"

#include "Carriage/CarriagePathActor.h"
#include "Carriage/CarriageStopPoint.h"
#include "Carriage/CarriageVehicle.h"


// Sets default values for this component's properties
UCarriageMovementComponent::UCarriageMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	CurrentDistance  = 0.0f;
	bisMoving = false;
	bisStoped = false;
	stopTimer=0.0f;
	CurrentStopPoint= nullptr;
	

	// ...
}


// Called when the game starts
void UCarriageMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCarriage = Cast<ACarriageVehicle>(GetOwner());
	if (!OwnerCarriage)
	{
		UE_LOG(LogTemp,Warning,TEXT("MovementComponent: OwnerCarriage가 null입니다!"));
		return;
	}
	CachedPathActor = OwnerCarriage->GetPathActor();
	UE_LOG(LogTemp, Warning, TEXT("MovementComponent BeginPlay - PathActor: %s"), CachedPathActor ? *CachedPathActor->GetName() : TEXT("NULL"));
}

void UCarriageMovementComponent::StartMovement()
{
	if (!OwnerCarriage || !CachedPathActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("CarriageMovementComponent: PathActor가 설정되지 않았습니 다!"));
		return;
	}
	if (!OwnerCarriage->HasAuthority())
	{
		return;
	}
	bisMoving = true;
	GetWorld()->GetTimerManager().SetTimer(MovementTimerHande,this,&UCarriageMovementComponent::UpateMovement,0.016f,true);
	UE_LOG(LogTemp, Warning, TEXT("마차 이동 시작! PathActor: %s"), CachedPathActor ? *CachedPathActor->GetName() : TEXT("NULL"));
	
}

void UCarriageMovementComponent::StopMovement()
{
	bisMoving = false;
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(MovementTimerHande);
	}
}

void UCarriageMovementComponent::UpateMovement()
{
	if (!bisMoving||!OwnerCarriage||!CachedPathActor)
	{
		return;
	}
	float DeltaTime = 0.016f;
	if (bisStoped)
	{
		UpdateStopTimer(DeltaTime);
		return;
	}
	CurrentDistance+= OwnerCarriage->Speed*DeltaTime;
	float PathLength = CachedPathActor->GetPathLength();
	if (CurrentDistance>=PathLength)
	{
		if (OwnerCarriage->bLooping)
		{
			CurrentDistance= 0.0f;

			for (ACarriageStopPoint* Stop : CachedPathActor->StopPoints)
			{
				if (Stop) Stop->ResetVisited();
			}
		}
		else
		{
			CurrentDistance = PathLength;
			OwnerCarriage->Speed = -OwnerCarriage->Speed;
		}
		
	}
	else if (CurrentDistance<0.0f)
	{
		CurrentDistance = 0.0f;
		OwnerCarriage->Speed = -OwnerCarriage->Speed;
	}
	FVector NewLocation = CachedPathActor->GetLocationAtDistance(CurrentDistance);
	FRotator NewRotation = CachedPathActor->GetRotationAtDistance(CurrentDistance);

	OwnerCarriage->SetActorLocationAndRotation(NewLocation, NewRotation);

	CheckStopPoints();
}

void UCarriageMovementComponent::CheckStopPoints()
{
	if (!CachedPathActor)return;
	ACarriageStopPoint* NearestStop = CachedPathActor->FindNearestStopPoint(CurrentDistance,100.f);
	if (NearestStop&& !NearestStop->bVisited)
	{
		BeginStop(NearestStop);
	}
}

void UCarriageMovementComponent::BeginStop(ACarriageStopPoint* StopPoint)
{
	if (!StopPoint) return;
	bisStoped = true;
	CurrentStopPoint = StopPoint;
	stopTimer= StopPoint->GetStopDuration();

	StopPoint->bVisited = true;

	StopPoint->OnCarriageArrived(OwnerCarriage);
	UE_LOG(LogTemp, Log, TEXT("정류장 [%s] 도착! %f초 정차"), *StopPoint->StopName, stopTimer);
}

void UCarriageMovementComponent::UpdateStopTimer(float DeltaTime)
{
	stopTimer -= DeltaTime;
	if (stopTimer <= 0.0f)
	{
		bisStoped = false;
		if (CurrentStopPoint)
		{
			CurrentStopPoint->OnCarriageDeparted(OwnerCarriage);
			CurrentStopPoint = nullptr;
			UE_LOG(LogTemp, Log, TEXT("정류장 [%s] 출발!"), *CurrentStopPoint->StopName);

			
		}
	}
}




