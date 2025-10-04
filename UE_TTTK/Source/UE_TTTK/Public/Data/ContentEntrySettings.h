#pragma once

#include "CoreMinimal.h"
#include "ContentEntrySettings.generated.h"


enum class EMappingMode : uint8;

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
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EMappingMode mappingMode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText contentName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText contentDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 minPlayers = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 maxPlayers = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EStartCondition startCondition;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAllowMidGameQuit = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsTeamBased = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float timeLimitSeconds = 0.0f; // 0 = no time limit

    // 컨텐츠 Manager 클래스
    //UPROPERTY(EditAnywhere, BlueprintReadWrite)
    //TSubclassOf<AContentManagerBase> ContentManagerClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FTransform> playerSpawnTransforms;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FName, FString> customSettings;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* thumbnailImage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor uiAccentColor = FLinearColor::White;
    
};