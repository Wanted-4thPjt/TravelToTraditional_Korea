// Fill out your copyright notice in the Description page of Project Settings.


#include "Content/Jegi/Jegi.h"

#include "MainPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/InteractableComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AJegi::AJegi()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates  = true;


	sphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Collider"));
	sphereCollider->SetCollisionResponseToAllChannels(ECR_Ignore);
	sphereCollider->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	sphereCollider->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	if (!sphereCollider->ComponentHasTag("Jegi"))
	{
		sphereCollider->ComponentTags.Add("Jegi");
	}
	SetRootComponent(sphereCollider);
		
	meshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	meshComponent->SetupAttachment(sphereCollider);
	meshComponent->SetCollisionObjectType(ECC_WorldDynamic);
	meshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	meshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	meshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	sphereCollider->SetSphereRadius(10.f);
	meshComponent->SetRelativeLocation(FVector(0.f, 0.f, 32.f));
	SetActorRelativeScale3D(FVector(1.f));
	
	movementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	if (movementComponent)
	{
		movementComponent->UpdatedComponent = meshComponent;
		movementComponent->SetIsReplicated(true);
		movementComponent->bShouldBounce = false;
		movementComponent->bSimulationEnabled = true;
		movementComponent->ProjectileGravityScale = 1.f;
		movementComponent->MaxSpeed = 600.f;
		movementComponent->Bounciness = 0.f;
		movementComponent->bSweepCollision = true;
	}
}

// Called when the game starts or when spawned
void AJegi::BeginPlay()
{
	Super::BeginPlay();

	// 순수 물리 Actor - 별도 초기화 없음
}

// ========== 제거됨 - Component로 이동 ==========
// void AJegi::TempInteract(APawn* player) { }
// void AJegi::OnStart() { }
// void AJegi::OnEnd(APawn* player) { }
// void AJegi::OnKick(EKickTiming inTiming) { }
// void AJegi::TempKick() { }

void AJegi::OnReachToGround(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_WorldStatic)
	{
		NetMulticast_OnReachToGround();
	}
}

float AJegi::FindCylinderMeshRadius(const FVector& meshBoxExtent)
{
	// Z축이 높이, X/Y가 반지름
	if (FMath::IsNearlyEqual(meshBoxExtent.X, meshBoxExtent.Y, 1.0f))
		return meshBoxExtent.X;

	// X축이 높이, Y/Z가 반지름
	if (FMath::IsNearlyEqual(meshBoxExtent.Y, meshBoxExtent.Z, 1.0f))
		return meshBoxExtent.Y; 

	// Y축이 높이, X/Z가 반지름
	if (FMath::IsNearlyEqual(meshBoxExtent.X, meshBoxExtent.Z, 1.0f))
		return meshBoxExtent.X; 
	
	// 불규칙한 경우
	return (meshBoxExtent.X + meshBoxExtent.Y + meshBoxExtent.Z) * 0.333f;

}

void AJegi::NetMulticast_OnReachToGround_Implementation()
{
	// 땅 충돌 시 물리 정지
	movementComponent->bSimulationEnabled = false;
	movementComponent->ProjectileGravityScale = 0.f;
	movementComponent->MaxSpeed = 0.f;
	movementComponent->Bounciness = 0.f;

	UE_LOG(LogTemp, Log, TEXT("AJegi: Reached ground - Physics stopped"));
}

// ========== 제거됨 - Component가 로컬로 물리 처리 ==========
// void AJegi::NetMulticast_OnKicked_Implementation(EKickTiming inTiming) { }

