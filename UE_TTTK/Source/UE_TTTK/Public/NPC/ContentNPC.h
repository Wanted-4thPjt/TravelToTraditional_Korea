#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ContentNPC.generated.h"

class UContentEntryComponent;
class UWidgetComponent;

UCLASS()
class UE_TTTK_API AContentNPC : public ACharacter
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
	void SetOutlineEnabled(bool bEnabled);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	TObjectPtr<UContentEntryComponent> contentEntry;
	
};
