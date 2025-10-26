#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ContentNPC.generated.h"

enum class EInteractableState : uint8;
class AMainPlayer;
class UInteractableComponent;
class UContentEntryComponent;
class UWidgetComponent;

UCLASS()
class UE_TTTK_API AContentNPC : public AActor
{
	GENERATED_BODY()

public:
	AContentNPC();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UContentEntryComponent* GetContentEntryComponent() const {return contentEntry;}
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UInteractableComponent* GetInteractableComponent() const {return interactableComponent;}

private:
	UFUNCTION()
	void OnInteractablePlayerStateChanged(APlayerController* playerController);
	
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TObjectPtr<UContentEntryComponent> contentEntry;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TObjectPtr<UInteractableComponent> interactableComponent;
};
