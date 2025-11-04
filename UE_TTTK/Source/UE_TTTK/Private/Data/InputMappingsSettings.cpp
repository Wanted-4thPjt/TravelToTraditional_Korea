// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/InputMappingsSettings.h"

void UInputMappingsSettings::PostInitProperties()
{
	Super::PostInitProperties();
	prevMappings = inputMappings;
}

void UInputMappingsSettings::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FInputMappingData, inputMappingContext))
	{
		if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ValueSet)
		{
			TArray<FName> keys;
			prevMappings.GetKeys(keys);
			for (FName& key : keys)
			{
				FName newKey = key == NAME_None ? inputMappings[key].inputMappingContext->GetFName() : key;
				if (key != newKey)
				{
					FInputMappingData data = inputMappings.FindAndRemoveChecked(key);
					inputMappings.Add(newKey, data);
				}
				if (inputMappings[newKey].inputMappingContext != prevMappings[key].inputMappingContext)
				{
					inputMappings[newKey].inputActions.Empty();
					for (FEnhancedActionKeyMapping mapping : inputMappings[newKey].inputMappingContext->GetMappings())
					{
						inputMappings[newKey].inputActions.Add(TTuple<FName, UInputAction*>(mapping.Action.GetFName(), mapping.Action));
					}
				}
			}
			
		}
	}
	prevMappings = inputMappings;
}
