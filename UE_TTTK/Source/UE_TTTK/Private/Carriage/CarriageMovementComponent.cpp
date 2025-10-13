// Fill out your copyright notice in the Description page of Project Settings.


#include "Carriage/CarriageMovementComponent.h"

#include "Carriage/CarriagePathActor.h"
#include "Carriage/CarriageStopPoint.h"
#include "Carriage/CarriageVehicle.h"
#include "Net/UnrealNetwork.h"


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

	// 컴포넌트 복제 활성화
	SetIsReplicatedByDefault(true);

}

void UCarriageMovementComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCarriageMovementComponent, CurrentDistance);
	DOREPLIFETIME(UCarriageMovementComponent, bisMoving);
	DOREPLIFETIME(UCarriageMovementComponent, bisStoped);
	DOREPLIFETIME(UCarriageMovementComponent, CurrentStopPoint);
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
	FVector SplineLocation = CachedPathActor->GetLocationAtDistance(CurrentDistance);
	FRotator SplineRotation = CachedPathActor->GetRotationAtDistance(CurrentDistance);

	// 마차 Mesh가 Y축 방향을 Forward로 사용하므로 90도 회전 오프셋 적용
	SplineRotation.Yaw += -90.0f;

	// 역주행(Speed < 0) 시 180도 반전
	if (OwnerCarriage->Speed < 0.0f)
	{
		SplineRotation.Yaw += 180.0f;
	}

	// 지면 추적으로 위치/회전 조정
	FVector AdjustedLocation;
	FRotator AdjustedRotation;
	TraceGround(SplineLocation, SplineRotation, AdjustedLocation, AdjustedRotation);

	OwnerCarriage->SetActorLocationAndRotation(AdjustedLocation, AdjustedRotation);

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
			UE_LOG(LogTemp, Log, TEXT("정류장 [%s] 출발!"), *CurrentStopPoint->StopName);
			CurrentStopPoint->OnCarriageDeparted(OwnerCarriage);
			CurrentStopPoint = nullptr;
		}
	}
}

void UCarriageMovementComponent::TraceGround(const FVector& BaseLocation, const FRotator& BaseRotation,
	FVector& OutAdjustedLocation, FRotator& OutAdjustedRotation)
{
	if (!GetWorld())
	{
		OutAdjustedLocation = BaseLocation;
		OutAdjustedRotation = BaseRotation;
		return;
	}

	// SceneComponent에서 로컬 위치 가져오기
	if (!OwnerCarriage || !OwnerCarriage->FrontTracePoint || !OwnerCarriage->RearTracePoint)
	{
		OutAdjustedLocation = BaseLocation;
		OutAdjustedRotation = BaseRotation;
		return;
	}

	FVector ForwardVector = BaseRotation.Vector();
	FVector RightVector = FRotator(0, BaseRotation.Yaw + 90.0f, 0).Vector();
	FVector UpVector = FVector(0, 0, 1);

	// SceneComponent의 로컬 위치를 월드 좌표로 변환
	FVector FrontLocalOffset = OwnerCarriage->FrontTracePoint->GetRelativeLocation();
	FVector RearLocalOffset = OwnerCarriage->RearTracePoint->GetRelativeLocation();

	// 앞쪽 Trace 위치
	FVector FrontLocalPos = (ForwardVector * FrontLocalOffset.X)
		+ (RightVector * FrontLocalOffset.Y)
		+ (UpVector * FrontLocalOffset.Z);
	FVector FrontTraceStart = BaseLocation + FrontLocalPos + FVector(0, 0, TraceStartOffset);
	FVector FrontTraceEnd = FrontTraceStart - FVector(0, 0, TraceDistance);

	// 뒤쪽 Trace 위치
	FVector RearLocalPos = (ForwardVector * RearLocalOffset.X)
		+ (RightVector * RearLocalOffset.Y)
		+ (UpVector * RearLocalOffset.Z);
	FVector RearTraceStart = BaseLocation + RearLocalPos + FVector(0, 0, TraceStartOffset);
	FVector RearTraceEnd = RearTraceStart - FVector(0, 0, TraceDistance);

	// LineTrace 실행
	FHitResult FrontHit;
	FHitResult RearHit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCarriage);

	bool bFrontHit = GetWorld()->LineTraceSingleByChannel(
		FrontHit, FrontTraceStart, FrontTraceEnd, ECC_Visibility, QueryParams
	);

	bool bRearHit = GetWorld()->LineTraceSingleByChannel(
		RearHit, RearTraceStart, RearTraceEnd, ECC_Visibility, QueryParams
	);

	// 디버그 드로우
	DrawDebugLine(
		GetWorld(),
		FrontTraceStart,
		FrontTraceEnd,
		bFrontHit ? FColor::Green : FColor::Red,
		false,
		-1.0f,
		0,
		2.0f
	);

	DrawDebugLine(
		GetWorld(),
		RearTraceStart,
		RearTraceEnd,
		bRearHit ? FColor::Green : FColor::Red,
		false,
		-1.0f,
		0,
		2.0f
	);

	if (bFrontHit)
	{
		DrawDebugSphere(
			GetWorld(),
			FrontHit.Location,
			15.0f,
			12,
			FColor::Cyan,
			false,
			-1.0f
		);
	}

	if (bRearHit)
	{
		DrawDebugSphere(
			GetWorld(),
			RearHit.Location,
			15.0f,
			12,
			FColor::Cyan,
			false,
			-1.0f
		);
	}

	// 지면 높이 계산
	float FrontGroundZ = bFrontHit ? FrontHit.Location.Z : BaseLocation.Z;
	float RearGroundZ = bRearHit ? RearHit.Location.Z : BaseLocation.Z;

	// 중앙 높이 계산 (앞뒤 평균)
	float CenterGroundZ = (FrontGroundZ + RearGroundZ) / 2.0f;

	// 위치 조정 (지면 + 오프셋)
	OutAdjustedLocation = BaseLocation;
	OutAdjustedLocation.Z = CenterGroundZ + GroundOffset;

	// Pitch 계산 (앞뒤 높이 차이)
	float HeightDifference = FrontGroundZ - RearGroundZ;
	float Distance = FrontLocalPos.Size2D() + RearLocalPos.Size2D();  // 앞뒤 Trace 간 거리
	float PitchRadians = FMath::Atan2(HeightDifference, Distance);
	float PitchDegrees = FMath::RadiansToDegrees(PitchRadians);

	// 회전 조정 (기존 Yaw 유지, Pitch 추가)
	OutAdjustedRotation = BaseRotation;
	OutAdjustedRotation.Pitch = PitchDegrees;

	// 디버그 로그 (테스트용)
	// UE_LOG(LogTemp, Log, TEXT("Ground Trace - Front: %.1f, Rear: %.1f, Pitch: %.2f"),
	// 	FrontGroundZ, RearGroundZ, PitchDegrees);
}




