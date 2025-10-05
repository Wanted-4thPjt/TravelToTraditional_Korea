#include "Data/InteractableFeedbackSettings.h"

void FInteractableFeedbackSettings::EnableOutline(const bool& bEnabled, UPrimitiveComponent* inOutlineComponent, FLinearColor inOutlineColor)
{
	if (!bEnabled)
	{
		effectType &= ~static_cast<uint8>(EEffectType::Outline);
		return;
	}
	effectType |= static_cast<uint8>(EEffectType::Outline);
	outlineComponent = inOutlineComponent;
	outlineColor = inOutlineColor;
}


void FInteractableFeedbackSettings::EnableWidget(const bool& bEnabled, TSubclassOf<UUserWidget> inInteractionGuideWidgetClass, FVector inWidgetOffset)
{
	if (!bEnabled)
	{
		effectType &= ~static_cast<uint8>(EEffectType::Widget);
		return;
	}
	effectType |= static_cast<uint8>(EEffectType::Widget);
	interactionGuideWidgetClass = inInteractionGuideWidgetClass;
	widgetOffset = inWidgetOffset;
}


void FInteractableFeedbackSettings::EnableSound(const bool& bEnabled, USoundBase* inInteractedSound, USoundBase* inActivatedSound)
{
	if (!bEnabled)
	{
		effectType &= ~static_cast<uint8>(EEffectType::Sound);
		return;
	}
	effectType |= static_cast<uint8>(EEffectType::Sound);
	interactedSound = inInteractedSound;
	activatedSound = inActivatedSound;
}

void FInteractableFeedbackSettings::EnableNiagara(const bool& bEnabled, UNiagaraSystem* inInteractedNiagaraVFX)
{
	if (!bEnabled)
	{
		effectType &= ~static_cast<uint8>(EEffectType::Niagara);
		return;
	}
	effectType |= static_cast<uint8>(EEffectType::Niagara);
	interactedNiagaraVFX = inInteractedNiagaraVFX;
}

void FInteractableFeedbackSettings::EnableParticle(const bool& bEnabled, UParticleSystem* inInteractedParticleVFX)
{
	if (!bEnabled)
	{
		effectType &= ~static_cast<uint8>(EEffectType::Particle);
		return;
	}
	effectType |= static_cast<uint8>(EEffectType::Particle);
	interactedParticleVFX = inInteractedParticleVFX;
}

