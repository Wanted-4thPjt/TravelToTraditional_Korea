// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CrowdData.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Character.h"
#include "Crowd.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FCheckTimeDelegate,ACrowd*,bool);
DECLARE_DELEGATE_TwoParams(FTalkStartDelegates,ACrowd*, int32 )
UCLASS()
class UE_TTTK_API ACrowd : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACrowd();
	UPROPERTY(EditDefaultsOnly, Category = "Crowd")
	class UCrowdData* CrowdData;
	UPROPERTY()
	TMap<FString,class ACrowdTargetPoint*> TargetPoints;
	UPROPERTY()
	FVector realTarget;
	UPROPERTY(EditDefaultsOnly, Category = "Ani")
	class UAnimMontage* GreetingMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Ani")
	class UAnimMontage* OuchAnimMontage;
	FTimerHandle CheckTimerHandle;
	UPROPERTY()
	class ACrowdTargetPoint* GoHomeTargetPoint;
	UPROPERTY()
	class ACrowdTargetPoint* GoWorkTargetPoint;
	UPROPERTY()
	class ACrowdTargetPoint* CurrentCrowdTargetPoint;
	UPROPERTY()
	FVector currentTargetLocation = FVector::ZeroVector;
	

	UPROPERTY(BlueprintReadOnly, Category = "Time")
	bool bShouldGoWork = false;

	UPROPERTY(BlueprintReadOnly, Category = "Time")
	bool bShouldGoHome = false;

private:
	bool bIsMoving;
	bool bIsOuchAnimCompleted; // Ouch 애니메이션 완료 플래그 (AnimNotify에서 설정)
	FName currentState;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnCapsuleHit(UPrimitiveComponent* HitComponent, 
					  AActor* OtherActor, 
					  UPrimitiveComponent* OtherComp, 
					  FVector NormalImpulse, 
					  const FHitResult& Hit);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Animation")
	bool bIsTalking=false;;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Animation")
	bool bIsListening=false;;
	class ACrowdTargetPoint* GetTargetPoint(FString Destination);
	class ACrowdTargetPoint* GetTargetByEnum();
	bool GetIsMoving(){return bIsMoving;}
	void SetIsMoving(bool MovingState){bIsMoving = MovingState;}
	void CollectingTargetPoints();
	void PlayGreeting();
	void PlayOuch();
	void PlayCheck();
	bool CheckGoHomeTime();
	bool CheckGoWorkTime();
	void SetIsTalking(bool talking){bIsTalking = talking;};
	void SetIsListening(bool listening){bIsListening = listening;};
	void SetGoHomeTargetPoint(class ACrowdTargetPoint* TargetPoint){GoHomeTargetPoint = TargetPoint;};
	void SetGoWorkTargetPoint(class ACrowdTargetPoint* TargetPoint){GoWorkTargetPoint = TargetPoint;};
	void SetCurrentTargetLocation(FVector loc){currentTargetLocation = loc;};
	void SetCurrentCrowdTargetPoint(class ACrowdTargetPoint* TargetPoint){CurrentCrowdTargetPoint = TargetPoint;};
	UAudioComponent* AudioComp;
	ACrowdTargetPoint* GetGoHomeTargetPoint(){return GoHomeTargetPoint;};
	ACrowdTargetPoint* GetGoWorkTargetPoint(){return GoWorkTargetPoint;};
	ACrowdTargetPoint* GetCurrentCrowdTargetPoint(){return CurrentCrowdTargetPoint;};
	FVector GetCurrentTargetLocation(){return currentTargetLocation;};
	
	UFUNCTION()
	void CheckTime(FTimeOfDayData TimeData);
	UFUNCTION()
	void SetCrowdCurrentState(FName NewState);

	bool bIsMarketeer = false;
	
	FName GetCrowdCurrentState(){return currentState;};

	
	bool GetOuchAnimCompleted() const { return bIsOuchAnimCompleted; }
	void SetOuchAnimCompleted(bool bCompleted) { bIsOuchAnimCompleted = bCompleted; }

	UFUNCTION(BlueprintCallable, Category = "Time")
	void ResetTimeFlags() { bShouldGoWork = false; bShouldGoHome = false; }

	UPROPERTY(EditAnywhere, Category="TalkSound")
	TArray<USoundBase*> talkSounds;
	UFUNCTION(BlueprintCallable)
	void Talk(int32 index);
	void OnTalkStarted(ACrowd* lastTalker);
	FTalkStartDelegates OnTalkStartedDelegate;

	// 오디오 종료 콜백
	UFUNCTION()
	void OnAudioFinishedCallback();
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

};


