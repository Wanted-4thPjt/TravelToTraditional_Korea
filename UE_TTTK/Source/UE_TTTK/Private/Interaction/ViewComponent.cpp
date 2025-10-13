#include "Interaction/ViewComponent.h"

#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "Chaos/Utilities.h"
#include "Interaction/InteractableComponent.h"

UViewComponent::UViewComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ownerEye = CreateDefaultSubobject<USceneComponent>(FName("Eye"));
}

void UViewComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UViewComponent, focusingActor);
}

void UViewComponent::BeginPlay()
{
	Super::BeginPlay();

	pawnOwner = Cast<APawn>(GetOwner());
	params.AddIgnoredActor(pawnOwner);

	if (IsValid(pawnOwner))
	{
		if (UCameraComponent* camera = pawnOwner->FindComponentByClass<UCameraComponent>())
		{
			ownerEye->AttachToComponent(camera, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Eye");
		}
		else
		{
			if (USkeletalMeshComponent* skm = pawnOwner->FindComponentByClass<USkeletalMeshComponent>();
				IsValid(skm) && skm->DoesSocketExist("Eye"))
			{
				ownerEye->AttachToComponent(skm, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Eye");
			}
			else if (UStaticMeshComponent* sm = pawnOwner->FindComponentByClass<UStaticMeshComponent>();
				IsValid(sm) && sm->DoesSocketExist("Eye"))
			{
				ownerEye->AttachToComponent(sm, FAttachmentTransformRules::SnapToTargetNotIncludingScale, "Eye");
			}
			else
			{
				ownerEye->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				ownerEye->SetRelativeLocationAndRotation(pawnOwner->GetPawnViewLocation(), pawnOwner->GetControlRotation());
			}
		}
		
		ownerEye->RegisterComponent();
	}
}

void UViewComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(traceTimer);
	Super::EndPlay(EndPlayReason);
}

void UViewComponent::EnableTrace_Implementation(bool bEnable)
{
	if (!IsValid(pawnOwner)) {return;}
	if (pawnOwner->GetLocalRole() != ENetRole::ROLE_AutonomousProxy) {return;}
	
	if (!bEnable)
	{
		GetWorld()->GetTimerManager().ClearTimer(traceTimer);
		return;
	}
	
	if (!traceTimer.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(traceTimer,
		   this, &UViewComponent::ShootLineTrace,
		   traceInterval, true
	   );
	}
}

void UViewComponent::ShootLineTrace()
{
	if (!IsValid(ownerEye)) {return;}

	FVector startPos = ownerEye->GetComponentLocation();
	FVector endPos = startPos + ownerEye->GetForwardVector() * traceDistance;

	FHitResult hitResult;
	if (GetWorld()->LineTraceSingleByChannel(
		hitResult, startPos, endPos,
		ECC_Visibility, params)
	) {
		OnViewSthByLineTrace.Broadcast(hitResult);
	}

}

bool UViewComponent::IsInViewAngle(const AActor* inTarget) const
{
	if (!IsValid(ownerEye)) {return false;}
	FVector directionVector = (inTarget->GetActorLocation() - GetOwner()->GetActorLocation()).GetSafeNormal();
	float dotProduct = ownerEye->GetForwardVector().Dot(directionVector);

	return dotProduct > FMath::Cos(halfViewAngle);
}

