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
    Particle = 1 << 4 
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
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/UE_TTTK.EEffectType"))
    uint8 effectType = 0;
    
    static bool IsEnableEffect(const uint8& bitmask, const EEffectType& bitflag)
    {return (bitmask & static_cast<uint8>(bitflag)) > 0;}

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Outline", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Outline", EditConditionHides))
    TObjectPtr<UPrimitiveComponent> outlineComponent;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Outline", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Outline", EditConditionHides))
    int32 outlineStencilValue = 252;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Outline", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Outline", EditConditionHides))
    FLinearColor outlineColor = FLinearColor::Green;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Widget", EditConditionHides))
    TSubclassOf<UUserWidget> interactionGuideWidgetClass;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Widget", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Widget", EditConditionHides))
    FVector widgetOffset = FVector(0.f, 0.f, 100.f);

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Sound", EditConditionHides))
    TObjectPtr<USoundBase> activatedSound;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Sound", EditConditionHides))
    TObjectPtr<USoundBase> interactedSound;

    //UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Niagara", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Niagara", EditConditionHides))
    //TObjectPtr<UNiagaraSystem> activatedNiagaraVFX;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Niagara", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Niagara", EditConditionHides))
    TObjectPtr<UNiagaraSystem> interactedNiagaraVFX;

    //UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particle", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Particle", EditConditionHides))
    //TObjectPtr<UParticleSystem> activatedParticleVFX;
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Particle", meta=(EditCondition="effectType & \"/Script/UE_TTTK.EEffectType::Particle", EditConditionHides))
    TObjectPtr<UParticleSystem> interactedParticleVFX;

    bool IsOutlineOn() const {return IsEnableEffect(effectType, EEffectType::Outline);}
    bool IsWidgetOn() const {return IsEnableEffect(effectType , EEffectType::Widget);}
    bool IsSoundOn() const {return IsEnableEffect(effectType, EEffectType::Sound);}
    bool IsNiagaraOn() const {return IsEnableEffect(effectType, EEffectType::Niagara);}
    bool IsParticleOn() const {return IsEnableEffect(effectType, EEffectType::Particle);}

    void EnableOutline(const bool& bEnabled, UPrimitiveComponent* inOutlineComponent, FLinearColor inOutlineColor = FLinearColor::Green);
    void EnableWidget(const bool& bEnabled, TSubclassOf<UUserWidget> inInteractionGuideWidgetClass, FVector inWidgetOffset = FVector(0.f, 0.f, 100.f));
    void EnableSound(const bool& bEnabled, USoundBase* inInteractedSound, USoundBase* inActivatedSound = nullptr);
    void EnableNiagara(const bool& bEnabled, UNiagaraSystem* inInteractedNiagaraVFX);
    void EnableParticle(const bool& bEnabled, UParticleSystem* inInteractedParticleVFX);
};
