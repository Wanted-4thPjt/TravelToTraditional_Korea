#pragma once

#include "CoreMinimal.h"
#include "PlayerSubSystem.h"
#include "ContentEntrySettings.generated.h"


class UBaseContentManager;


UENUM(BlueprintType)
enum class EStartCondition : uint8
{
    Auto,      // 인원 차면 자동 시작
    Manual,    // Host가 시작 버튼 눌러야 시작
    Ready      // 모든 플레이어가 Ready 해야 시작
};

USTRUCT(BlueprintType)
struct UE_TTTK_API FContentEntrySettings
{
    GENERATED_BODY()
    
public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    FName contentName = "";

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TSubclassOf<UBaseContentManager> contentManagerClass;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    float maxWaitSeconds = 0.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 minPlayers = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 maxPlayers = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EStartCondition startCondition = EStartCondition::Manual;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor uiAccentColor = FLinearColor::White;
    
};