#include "Data/InteractableFeedbackSettings.h"

void FInteractableFeedbackSettings::EnableOutline(const bool& bEnabled, UMeshComponent* inOutlineComponent, FLinearColor inOutlineColor)
{
	if (!bEnabled)
	{
		effectType &= ~static_cast<uint8>(EEffectType::Outline);
		return;
	}
	effectType |= static_cast<uint8>(EEffectType::Outline);
	ownerMeshComponent = inOutlineComponent;
	outlineColor = inOutlineColor;
}


void FInteractableFeedbackSettings::EnableWidget(const bool& bEnabled, const TSubclassOf<UUserWidget>& inInteractionGuideWidgetClass, FName inWidgetSocketName)
{
	if (!bEnabled)
	{
		effectType &= ~static_cast<uint8>(EEffectType::Widget);
		return;
	}
	effectType |= static_cast<uint8>(EEffectType::Widget);
	interactionGuideWidgetClass = inInteractionGuideWidgetClass;
	widgetSocketName = inWidgetSocketName;
}


void FInteractableFeedbackSettings::EnableSound(const bool& bEnabled, USoundBase* inInteractedSound)
{
	if (!bEnabled)
	{
		effectType &= ~static_cast<uint8>(EEffectType::Sound);
		return;
	}
	effectType |= static_cast<uint8>(EEffectType::Sound);
	interactedSound = inInteractedSound;
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

void FInteractableFeedbackSettings::EnableNetwork(const bool& bEnabled)
{
	if (!bEnabled)
	{
		effectType &= ~static_cast<uint8>(EEffectType::Network);
		return;
	}
	effectType |= static_cast<uint8>(EEffectType::Network);
}

