#pragma once

#include "CoreMinimal.h"
#include "InteractableFeedbackSettings.generated.h"

class UPrimitiveComponent;
class UUserWidget;
class USoundBase;
class UNiagaraSystem;
class UParticleSystem;

UENUM(BlueprintType, meta=(BitFlags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EEffectType : uint8
{
    None = 0 << 0 UMETA(Hidden),
    Outline = 1 << 0, 
    Widget = 1 << 1, 
    Sound = 1 << 2, 
    Niagara = 1 << 3, 
    Particle = 1 << 4,
    Network = 1 << 7
};

/*
 * https://dev.epicgames.com/documentation/ko-kr/unreal-engine/unreal-engine-uproperties
 * https://dev.epicgames.com/documentation/ko-kr/unreal-engine/edit-conditions-for-properties-in-the-details-panel-in-unreal-engine
 * https://forums.unrealengine.com/t/bitmask-enum-value-in-editcondition-freezes-the-editor/1311664
*/
USTRUCT(BlueprintType)
struct UE_TTTK_API FInteractableFeedbackSettings
{
    GENERATED_BODY()

protected:
    static bool IsEnableEffect(const uint8& bitmask, const EEffectType& bitflag)
    {return (bitmask & static_cast<uint8>(bitflag)) > 0;}

public:
    FInteractableFeedbackSettings();
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/UE_TTTK.EEffectType"))
    uint8 effectType = 0;
    
    UPROPERTY(/*VisibleAnywhere, BlueprintReadOnly, Category = "Outline", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Outline", EditConditionHides)*/)
    TWeakObjectPtr<UMeshComponent> ownerMeshComponent;

    // CustomDepth Stencil 방식 (사용 안 함 - 커스텀 머티리얼 방식으로 대체)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Outline", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Outline", EditConditionHides))
    int32 outlineStencilValue = 252;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Outline", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Outline", EditConditionHides))
    FLinearColor outlineColor = FLinearColor::Green;

    // 커스텀 머티리얼 아웃라인 (Heritage에서 사용)
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Outline", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Outline", EditConditionHides))
    bool bUseOverlayMaterial = true;  // true: Overlay, false: Replace

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Outline", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Outline", EditConditionHides))
    TObjectPtr<UMaterialInterface> customOutlineMaterial;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Widget", EditConditionHides))
    TSubclassOf<UUserWidget> interactionGuideWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Widget", EditConditionHides))
    FName widgetSocketName = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Sound", EditConditionHides))
    TObjectPtr<USoundBase> interactedSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Niagara", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Niagara", EditConditionHides))
    TObjectPtr<UNiagaraSystem> interactedNiagaraVFX;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particle", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Particle", EditConditionHides))
    TObjectPtr<UParticleSystem> interactedParticleVFX;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Network", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Network", EditConditionHides))
    bool bInteractionEffectsInNetwork = false;

    bool IsOutlineOn() const {return IsEnableEffect(effectType, EEffectType::Outline);}
    bool IsWidgetOn() const {return IsEnableEffect(effectType , EEffectType::Widget);}
    bool IsSoundOn() const {return IsEnableEffect(effectType, EEffectType::Sound);}
    bool IsNiagaraOn() const {return IsEnableEffect(effectType, EEffectType::Niagara);}
    bool IsParticleOn() const {return IsEnableEffect(effectType, EEffectType::Particle);}
    bool IsNetworkOn() const {return IsEnableEffect(effectType, EEffectType::Network);}

    void EnableOutline(const bool& bEnabled, UMeshComponent* inOutlineComponent = nullptr, FLinearColor inOutlineColor = FLinearColor::Green);
    void EnableWidget(const bool& bEnabled, const TSubclassOf<UUserWidget>& inInteractionGuideWidgetClass, FName inWidgetSocketName = NAME_None);
    void EnableSound(const bool& bEnabled, USoundBase* inInteractedSound);
    void EnableNiagara(const bool& bEnabled, UNiagaraSystem* inInteractedNiagaraVFX);
    void EnableParticle(const bool& bEnabled, UParticleSystem* inInteractedParticleVFX);
    void EnableNetwork(const bool& bEnabled);    
};
