// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerWidget.h"

#include "Components/TextBlock.h"
#include "UI/ChatWidget.h"

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}

float UPlayerWidget::GetCurrentFPS()
{
	if (!bShowFPS) {return -1.f;}
	if (GetOwningPlayer() != GetWorld()->GetFirstPlayerController()) {return -1.f;}
	if (GEngine && GEngine->GameViewport)
	{
		// FStatUnitData 구조체를 포함하는 UGameViewportClient에서 통계 데이터 가져오기
		const FStatUnitData* StatData = GEngine->GameViewport->GetStatUnitData();

		if (StatData)
		{
			// 가장 중요한 'Game Thread' 시간을 기반으로 FPS 계산
			// FStatUnitData::GameThreadTime은 밀리초(ms) 단위입니다.
			float GameThreadMs = StatData->GameThreadTime;

			// 프레임 시간(ms)이 0이 아닌 경우에만 계산
			if (GameThreadMs > 0.f)
			{
				// FPS = 1000.0f / 프레임 시간(ms)
				float CurrentFPS = 1000.0f / GameThreadMs;
				return CurrentFPS;
			}
		}
	}
	// 실패 시 또는 시간이 0일 경우 0.0f 반환
	return -1.f;
}
