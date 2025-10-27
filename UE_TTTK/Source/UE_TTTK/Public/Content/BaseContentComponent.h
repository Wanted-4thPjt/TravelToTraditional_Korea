// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseContentComponent.generated.h"

class UBaseContentManager;
class UEnhancedInputComponent;
class UInputMappingContext;

/**
 * Content별 Input 처리 및 게임 로직을 담당하는 Base Component
 * 각 Content는 이를 상속받아 구체적 Input 처리 구현
 *
 * Client Authority 방식:
 * - Client에서 모든 게임 로직 실행 (타이밍 판정, 콤보 계산 등)
 * - Server에는 결과만 전송하여 검증 및 저장
 * - 즉각적인 피드백으로 타이밍 게임에 최적화
 */
UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_TTTK_API UBaseContentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBaseContentComponent();

	// ========== 생명주기 (BlueprintNativeEvent) ==========

	/** Content 진입 시 호출 - Manager로부터 필요한 정보 받기 */
	UFUNCTION(BlueprintNativeEvent, Category="ContentComponent")
	void ActivateContentInput(UBaseContentManager* manager);
	virtual void ActivateContentInput_Implementation(UBaseContentManager* manager);

	/** Content 퇴출 시 호출 - 상태 초기화 */
	UFUNCTION(BlueprintNativeEvent, Category="ContentComponent")
	void DeactivateContentInput();
	virtual void DeactivateContentInput_Implementation();

	/** Input 바인딩 설정 (자식 클래스에서 구현) */
	UFUNCTION(BlueprintNativeEvent, Category="ContentComponent")
	void SetupInputBindings(UEnhancedInputComponent* InputComponent);
	virtual void SetupInputBindings_Implementation(UEnhancedInputComponent* InputComponent);

	/** Input 바인딩 해제 (선택적, 필요시 자식에서 오버라이드) */
	UFUNCTION(BlueprintNativeEvent, Category="ContentComponent")
	void ClearInputBindings(UEnhancedInputComponent* InputComponent);
	virtual void ClearInputBindings_Implementation(UEnhancedInputComponent* InputComponent);

protected:
	// ========== 참조 ==========

	/** 현재 Content의 Manager */
	UPROPERTY(BlueprintReadOnly, Category="ContentComponent")
	UBaseContentManager* ownerContentManager = nullptr;

	/** 활성화 여부 */
	UPROPERTY(BlueprintReadOnly, Category="ContentComponent")
	bool bContentActive = false;

	/** Input Mapping Context (InputMappingsSettings에서 로드) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Input|Context")
	TObjectPtr<UInputMappingContext> inputMappingContext;

public:
	// ========== Getter ==========

	FORCEINLINE bool IsContentActive() const { return bContentActive; }
	FORCEINLINE UBaseContentManager* GetOwnerManager() const { return ownerContentManager; }
	FORCEINLINE UInputMappingContext* GetInputMappingContext() const { return inputMappingContext; }
};
