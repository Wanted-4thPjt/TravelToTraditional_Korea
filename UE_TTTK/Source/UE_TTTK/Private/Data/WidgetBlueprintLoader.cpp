// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/WidgetBlueprintLoader.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/UserWidgetBlueprint.h"

void UWidgetBlueprintLoader::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(UWidgetBlueprintLoader, WBP_ClassContainer))
	{
		if (PropertyChangedEvent.ChangeType == EPropertyChangeType::ValueSet)
		{
			UE_LOG(LogTemp, Warning, TEXT("Property Name From Change Event : %s"), *PropertyChangedEvent.GetMemberPropertyName().ToString());
			
		}
	}
}
