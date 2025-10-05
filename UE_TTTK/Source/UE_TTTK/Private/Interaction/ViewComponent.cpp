#include "Interaction/ViewComponent.h"

#include "Camera/CameraComponent.h"
#include "Interaction/InteractableComponent.h"

UViewComponent::UViewComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UViewComponent::BeginPlay()
{
	Super::BeginPlay();

	pawnOwner = Cast<APawn>(GetOwner());
	if (IsValid(pawnOwner))
	{
		ownerEye = pawnOwner->FindComponentByClass<UCameraComponent>();
		if (!ownerEye)
		{
			ownerEye = pawnOwner->FindComponentByClass<USkeletalMeshComponent>();
			if (!ownerEye)
			{
				ownerEye = pawnOwner->FindComponentByClass<UStaticMeshComponent>();
			}
		}
	}
	EnableTrace(true);
}

void UViewComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().ClearTimer(traceTimer);
	Super::EndPlay(EndPlayReason);
}

void UViewComponent::EnableTrace(const bool& bEnable)
{
	if (!IsValid(pawnOwner) || !pawnOwner->IsLocallyControlled()) {return;}
	
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
	FCollisionQueryParams params;
	params.AddIgnoredActor(pawnOwner);

	if (GetWorld()->LineTraceSingleByChannel(
		hitResult, startPos, endPos,
		ECC_Visibility, params)
	) {
		AActor* newTarget = hitResult.GetActor()->GetComponentByClass<UInteractableComponent>() ? hitResult.GetActor() : nullptr;
		if (interactableTarget != newTarget)
		{
			interactableTarget = newTarget;
			UpdateOutline(newTarget);
			onInteractableTargetChanged.Broadcast(newTarget);
		}
		return;
	}

	if (interactableTarget != nullptr)
	{
		interactableTarget = nullptr;
		UpdateOutline(nullptr);
		onInteractableTargetChanged.Broadcast(nullptr);
	}
}

bool UViewComponent::IsInViewAngle(AActor* inTarget, float& outDotProduct)
{
	return false;
}

void UViewComponent::UpdateOutline(AActor* newTarget)
{
	if (IsValid(outlinedTarget))
	{
		//if ()
		//{
		//	//if (UInteractableVisualComponent* visual = interactable)
		//}
	}
}

