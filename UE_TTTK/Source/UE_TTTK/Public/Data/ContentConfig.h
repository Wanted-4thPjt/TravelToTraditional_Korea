#pragma once

#include "CoreMinimal.h"
#include "ContentConfig.generated.h"

enum class EMappingMode : uint8;

UENUM(BlueprintType, meta=(BitFlag, UseEnumValuesAsMaskInEidtor = "true"))
enum class EContentFlags : uint8
{
	Default = 0 UMETA(Hidden),
	PlayInSolo = 1 << 0,
	AllowMidGameQuit = 1 << 1,
	HasRound = 1 << 2,
	TeamBased = 1 << 3,
	RecordScore = 1 << 4,
	RecordTime = 1 << 5,
	RecordCustomData = 1 << 6,
	ChangeCameraView = 1 << 7,
};
ENUM_CLASS_FLAGS(EContentFlags);
FORCEINLINE uint8 operator| (const uint8& bitFlag, const EContentFlags& bitMask) {return bitFlag | static_cast<uint8>(bitMask);}
FORCEINLINE void operator|= (uint8& bitFlag, const EContentFlags& bitMask) {bitFlag = bitFlag | static_cast<uint8>(bitMask);}
FORCEINLINE uint8 operator& (const uint8& bitFlag, const EContentFlags& bitMask) {return bitFlag & static_cast<uint8>(bitMask);}
FORCEINLINE void operator&= (uint8& bitFlag, const EContentFlags& bitMask) {bitFlag = bitFlag & static_cast<uint8>(bitMask);}

USTRUCT(BlueprintType)
struct UE_TTTK_API FContentConfig
{
	GENERATED_BODY()

	bool IsContentTyped(const EContentFlags& bitflag) const
	{return (contentType & static_cast<uint8>(bitflag)) > 0;}                        
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Intro")
	FName contentName = FName();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Intro")
	FText Description = FText();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Intro")
	UTexture2D* thumbnailImage = nullptr;

	// contentInput은 제거됨 - ContentComponent가 InputMappingsSettings에서 로드
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DynamicSetting|InputMode", meta=(AllowedClasses=UInputMappingContext))
	// UInputMappingContext* contentInput = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DynamicSetting|SpawnLocation")
	TArray<FTransform> playerSpawnPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="DynamicSetting|Camera")
	TArray<FTransform> cameraTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GamePlay|Flag", meta = (Bitmask, BitmaskEnum = "/Script/UE_TTTK.EContentFlags"))
	uint8 contentType = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Time", meta=(ToolTip="@ TimeLimit < 1 : 제한 시간 없음."))
	float contentTimeLimit = -1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Time", meta=(ToolTip="@ TimeLimit < 1 : 제한 시간 없음.", EditCondition="contentType & \"/Script/UE_TTTK.EContentFlags::HasRound", EditConditionHides))
	float roundTimeLimit = -1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Round", meta=(ToolTip="플레이어당 라운드 수 (제기차기 등에서 사용)", EditCondition="contentType & \"/Script/UE_TTTK.EContentFlags::HasRound", EditConditionHides))
	int32 roundsPerPlayer = 1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GamePlay|Custom", meta=(EditCondition="contentType & \"/Script/UE_TTTK.EContentFlags::RecordCustomData", EditConditionHides))
	TArray<FName> recordedCustomKeys;
};