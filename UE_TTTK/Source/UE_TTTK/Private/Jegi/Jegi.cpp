// Fill out your copyright notice in the Description page of Project Settings.


#include "Jegi/Jegi.h"

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

	if (IsValid(meshComponent))
	{
		//meshComponent->OnComponentHit.AddDynamic(this, &AJegi::OnReachToGround);

		if (UStaticMesh* mesh = meshComponent->GetStaticMesh();
			IsValid(sphereCollider) && mesh)
		{
			
		}
	}
	

	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "Camera", tempCameraActor);
	//sphereCollider->OnComponentBeginOverlap.AddDynamic()
}

void AJegi::TempInteract(APawn* player)
{
	if (!player->GetController<APlayerController>()) {return;}
	originalCameraActor = player->GetController<APlayerController>()->GetViewTarget();
	player->GetController<APlayerController>()->SetViewTargetWithBlend(tempCameraActor[0], 1, VTBlend_EaseInOut);
	FTimerHandle timer;
	GetWorld()->GetTimerManager().SetTimer(timer, this, &AJegi::OnStart, 3.f, false);
}

void AJegi::OnStart()
{
	FVector pos = GetActorLocation() + FVector(0.f, 0.f, 500.f);
	SetActorLocation(pos);
}

void AJegi::OnEnd(APawn* player)
{
	if (!player->GetController<APlayerController>()) {return;}
	player->GetController<APlayerController>()->SetViewTargetWithBlend(originalCameraActor, 1, VTBlend_EaseInOut);
}

void AJegi::OnKick(EKickTiming inTiming)
{
	if (movementComponent->Velocity.Z > 0.f) {return;}
	NetMulticast_OnKicked(inTiming);
}

void AJegi::TempKick()
{
	if (movementComponent->Velocity.Z > 0.f) {return;}
	if (GetActorLocation().Z > static_cast<int32>(EKickTiming::End) * 10.f)
	{
		return NetMulticast_OnKicked(EKickTiming::Missed);
	}
	else if (GetActorLocation().Z > static_cast<int32>(EKickTiming::Perfect) * 10.f)
	{
		return NetMulticast_OnKicked(EKickTiming::Perfect);
	}
	else if (GetActorLocation().Z > static_cast<int32>(EKickTiming::Great) * 10.f)
	{
		return NetMulticast_OnKicked(EKickTiming::Great);
	}
	else if (GetActorLocation().Z > static_cast<int32>(EKickTiming::Good) * 10.f)
	{
		return NetMulticast_OnKicked(EKickTiming::Good);
	}
	NetMulticast_OnKicked(EKickTiming::Missed);
}

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
	movementComponent->bSimulationEnabled = false;
	movementComponent->ProjectileGravityScale = 0.f;
	movementComponent->MaxSpeed = 0.f;
	movementComponent->Bounciness = 0.f;
}

void AJegi::NetMulticast_OnKicked_Implementation(EKickTiming inTiming)
{
	FVector kickPower = movementComponent->Velocity;
	
	switch (inTiming)
	{
	case EKickTiming::End:
	case EKickTiming::Default:
		return;
	case EKickTiming::Missed:
		kickPower.X = FMath::RandRange(100.f, 200.f);
		kickPower.Y = FMath::RandRange(100.f, 200.f);
		kickPower.Z = FMath::RandRange(10.f, 20.f);
		break;
	case EKickTiming::Good:
		kickPower.Z = 200.f;
		break;
	case EKickTiming::Great:
		kickPower.Z = 300.f;
		break;
	case EKickTiming::Perfect:
		kickPower.Z = 400.f;
		break;
	}
	
	movementComponent->Velocity = kickPower;
}

