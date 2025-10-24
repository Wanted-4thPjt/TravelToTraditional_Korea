# Actor Overridable LifeCycle Functions

언리얼 엔진 Actor의 생명주기에서 **protected 가상함수 호출 순서**

## Actor 생명주기 호출 순서

### 1. 생성 단계 (Construction)

```cpp
// 1. 생성자
AMyActor::AMyActor()
{
    // 컴포넌트 생성, 기본값 설정
    // 에디터와 런타임 모두에서 호출
}

// 2. PostInitProperties (매우 초기 단계)
virtual void PostInitProperties() override
{
    Super::PostInitProperties();
    // 프로퍼티 초기화 직후
    // CDO(Class Default Object) 생성 시에도 호출됨
}

// 3. PostLoad (에디터에서 로드 시)
virtual void PostLoad() override
{
    Super::PostLoad();
    // 에디터에서 저장된 액터를 로드할 때
    // 런타임에는 호출 안 됨
}

// 4. PostActorCreated (런타임 생성 시)
virtual void PostActorCreated() override
{
    Super::PostActorCreated();
    // SpawnActor로 생성될 때만 호출
    // 레벨에 배치된 액터는 호출 안 됨
}

// 5. OnConstruction (Construction Script)
virtual void OnConstruction(const FTransform& Transform) override
{
    Super::OnConstruction(Transform);
    // 블루프린트 Construction Script와 동일
    // 에디터에서 액터 이동/수정 시마다 호출
    // 런타임에는 생성 시 한 번만 호출
}
```

### 2. 초기화 단계 (Initialization)

```cpp
// 6. PreInitializeComponents
virtual void PreInitializeComponents() override
{
    Super::PreInitializeComponents();
    // 컴포넌트 초기화 직전
}

// 7. InitializeComponent (각 컴포넌트마다)
// 컴포넌트의 InitializeComponent() 호출됨

// 8. PostInitializeComponents
virtual void PostInitializeComponents() override
{
    Super::PostInitializeComponents();
    // 모든 컴포넌트 초기화 완료 후
    // 컴포넌트 간 참조 설정하기 좋은 시점
}
```

### 3. 게임 시작 단계 (Begin Play)

```cpp
// 9. BeginPlay
virtual void BeginPlay() override
{
    Super::BeginPlay();
    // 게임이 시작되거나 액터가 스폰될 때
    // 가장 일반적으로 사용되는 초기화 함수
}
```

### 4. 실행 중 (Runtime)

```cpp
// 10. Tick (매 프레임)
virtual void Tick(float DeltaTime) override
{
    Super::Tick(DeltaTime);
    // 매 프레임마다 호출
    // PrimaryActorTick.bCanEverTick = true 필요
}

// 기타 이벤트 함수들
virtual void NotifyActorBeginOverlap(AActor* OtherActor) override
{
    Super::NotifyActorBeginOverlap(OtherActor);
}

virtual void NotifyHit(...) override
{
    Super::NotifyHit(...);
}

// 등등...
```

### 5. 종료 단계 (Destruction)

```cpp
// 11. EndPlay
virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override
{
    Super::EndPlay(EndPlayReason);
    // 게임 종료, 레벨 전환, 액터 파괴 시
    // 정리 작업(cleanup)에 사용
}

// 12. Destroyed (Deprecated)
virtual void Destroyed() override
{
    Super::Destroyed();
    // EndPlay 사용 권장
    // 레거시 코드와의 호환성을 위해 존재
}

// 13. BeginDestroy
virtual void BeginDestroy() override
{
    Super::BeginDestroy();
    // 가비지 컬렉션 직전
    // 거의 사용하지 않음
}
```

## 전체 호출 순서 요약

```
[생성]
1. Constructor
2. PostInitProperties
3. PostLoad (에디터) 또는 PostActorCreated (런타임 스폰)
4. OnConstruction

[초기화]
5. PreInitializeComponents
6. InitializeComponent (컴포넌트들)
7. PostInitializeComponents

[게임 시작]
8. BeginPlay

[실행 중]
9. Tick (반복)
10. 각종 이벤트 함수들

[종료]
11. EndPlay
12. Destroyed
13. BeginDestroy
```

## 실전 예제

```cpp
UCLASS()
class MYGAME_API AMyActor : public AActor
{
    GENERATED_BODY()

public:
    AMyActor()
    {
        UE_LOG(LogTemp, Warning, TEXT("1. Constructor"));
        
        PrimaryActorTick.bCanEverTick = true;
        
        MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
        RootComponent = MeshComponent;
    }

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComponent;

    virtual void PostInitProperties() override
    {
        Super::PostInitProperties();
        UE_LOG(LogTemp, Warning, TEXT("2. PostInitProperties"));
    }

    virtual void PostLoad() override
    {
        Super::PostLoad();
        UE_LOG(LogTemp, Warning, TEXT("3. PostLoad (Editor)"));
    }

    virtual void PostActorCreated() override
    {
        Super::PostActorCreated();
        UE_LOG(LogTemp, Warning, TEXT("3. PostActorCreated (Runtime Spawn)"));
    }

    virtual void OnConstruction(const FTransform& Transform) override
    {
        Super::OnConstruction(Transform);
        UE_LOG(LogTemp, Warning, TEXT("4. OnConstruction"));
    }

    virtual void PreInitializeComponents() override
    {
        Super::PreInitializeComponents();
        UE_LOG(LogTemp, Warning, TEXT("5. PreInitializeComponents"));
    }

    virtual void PostInitializeComponents() override
    {
        Super::PostInitializeComponents();
        UE_LOG(LogTemp, Warning, TEXT("7. PostInitializeComponents"));
        // 여기서 컴포넌트 참조 안전하게 사용 가능
    }

    virtual void BeginPlay() override
    {
        Super::BeginPlay();
        UE_LOG(LogTemp, Warning, TEXT("8. BeginPlay"));
        // 가장 일반적인 초기화 위치
    }

    virtual void Tick(float DeltaTime) override
    {
        Super::Tick(DeltaTime);
        // 매 프레임
    }

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override
    {
        UE_LOG(LogTemp, Warning, TEXT("11. EndPlay"));
        
        // 정리 작업
        // 타이머 제거
        // 델리게이트 언바인드
        // 등등
        
        Super::EndPlay(EndPlayReason);
    }

    virtual void BeginDestroy() override
    {
        UE_LOG(LogTemp, Warning, TEXT("13. BeginDestroy"));
        Super::BeginDestroy();
    }
};
```

## 각 함수를 언제 사용하는가?

### Constructor
```cpp
AMyActor::AMyActor()
{
    // ✅ 컴포넌트 생성
    // ✅ 기본값 설정
    // ✅ 틱 설정
    // ❌ 다른 액터 참조 (아직 존재 안 함)
    // ❌ 월드 관련 작업 (GetWorld() 사용 불가)
}
```

### OnConstruction
```cpp
virtual void OnConstruction(const FTransform& Transform) override
{
    // ✅ 에디터에서 실시간 프리뷰
    // ✅ 동적 메시 생성
    // ✅ 머티리얼 설정
    // ❌ 게임플레이 로직 (에디터에서도 실행됨)
}
```

### PostInitializeComponents
```cpp
virtual void PostInitializeComponents() override
{
    Super::PostInitializeComponents();
    // ✅ 컴포넌트 간 연결
    // ✅ 컴포넌트 참조 설정
    // ✅ 컴포넌트 초기화 확인
}
```

### BeginPlay
```cpp
virtual void BeginPlay() override
{
    Super::BeginPlay();
    // ✅ 게임플레이 로직 초기화 (가장 일반적)
    // ✅ 타이머 시작
    // ✅ 델리게이트 바인딩
    // ✅ 다른 액터 찾기
    // ✅ UI 초기화
}
```

### EndPlay
```cpp
virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override
{
    // ✅ 타이머 제거
    // ✅ 델리게이트 언바인드
    // ✅ 리소스 정리
    // ✅ 네트워크 연결 종료
    
    Super::EndPlay(EndPlayReason);
}
```

## 특수 케이스

### 레벨에 배치된 액터
```
Constructor → PostLoad → OnConstruction → PreInitializeComponents 
→ PostInitializeComponents → BeginPlay
```

### SpawnActor로 생성된 액터
```
Constructor → PostActorCreated → OnConstruction → PreInitializeComponents 
→ PostInitializeComponents → BeginPlay
```

### 에디터에서 수정
```
OnConstruction (매번 호출됨 - 이동, 회전, 스케일 변경 시)
```

## 실용 팁

```cpp
UCLASS()
class MYGAME_API AWeapon : public AActor
{
    GENERATED_BODY()

public:
    AWeapon()
    {
        // 컴포넌트 생성만
        MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
        RootComponent = MeshComponent;
    }

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComponent;

    virtual void PostInitializeComponents() override
    {
        Super::PostInitializeComponents();
        // 컴포넌트가 모두 준비됨 - 안전하게 사용 가능
        if (MeshComponent)
        {
            MeshComponent->SetSimulatePhysics(true);
        }
    }

    virtual void BeginPlay() override
    {
        Super::BeginPlay();
        // 게임플레이 로직
        // 다른 액터 찾기
        ACharacter* Player = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
        if (Player)
        {
            // 플레이어와 상호작용 설정
        }

        // 타이머 시작
        GetWorldTimerManager().SetTimer(TimerHandle, this, &AWeapon::DoSomething, 1.0f, true);
    }

    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override
    {
        // 타이머 정리
        GetWorldTimerManager().ClearTimer(TimerHandle);
        
        Super::EndPlay(EndPlayReason);
    }

private:
    FTimerHandle TimerHandle;
    
    void DoSomething()
    {
        // 타이머 콜백
    }
};
```

## Register Vs. Initialize

`PostRegisterAllComponents`와 `PostInitializeComponents`의 차이

### 호출 순서

```
1. PreRegisterAllComponents
2. RegisterAllComponents (모든 컴포넌트 등록)
3. PostRegisterAllComponents ← 여기!
4. PreInitializeComponents
5. InitializeComponent (각 컴포넌트 초기화)
6. PostInitializeComponents ← 여기!
7. BeginPlay
```

### PostRegisterAllComponents

```cpp
virtual void PostRegisterAllComponents() override
{
    Super::PostRegisterAllComponents();
    
    // 컴포넌트들이 등록(Register)된 직후
    // 아직 초기화(Initialize)는 안 됨
    // 컴포넌트는 존재하지만 "활성화" 전 상태
}
```

#### 특징
- 컴포넌트가 **등록**만 됨 (존재는 하지만 초기화 안 됨)
- 컴포넌트의 **계층 구조(Attachment)**는 설정됨
- 컴포넌트의 `InitializeComponent()`는 **아직 호출 안 됨**
- 물리, 렌더링 등은 **아직 준비 안 됨**

#### 사용 시기
- 컴포넌트 존재 여부만 확인
- 컴포넌트 계층 구조 확인/수정
- 매우 초기 단계 설정

### PostInitializeComponents

```cpp
virtual void PostInitializeComponents() override
{
    Super::PostInitializeComponents();
    
    // 모든 컴포넌트가 초기화(Initialize) 완료됨
    // 컴포넌트들이 완전히 "활성화"된 상태
}
```

#### 특징
- 모든 컴포넌트의 `InitializeComponent()` **완료됨**
- 컴포넌트들이 **완전히 사용 가능한 상태**
- 물리, 렌더링, 콜리전 등 **모두 준비됨**
- 컴포넌트 간 **상호작용 가능**

#### 사용 시기
- 컴포넌트 간 참조 설정
- 컴포넌트 기능 사용 (물리, 콜리전 등)
- **가장 일반적으로 사용**

#### 비교 예제

```cpp
UCLASS()
class MYGAME_API AMyActor : public AActor
{
    GENERATED_BODY()

public:
    AMyActor()
    {
        UE_LOG(LogTemp, Warning, TEXT("Constructor"));
        
        MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
        RootComponent = MeshComponent;
        
        ChildComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Child"));
        ChildComponent->SetupAttachment(MeshComponent);
    }

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* MeshComponent;
    
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* ChildComponent;

    virtual void PostRegisterAllComponents() override
    {
        Super::PostRegisterAllComponents();
        UE_LOG(LogTemp, Warning, TEXT("PostRegisterAllComponents"));
        
        // ✅ 컴포넌트 존재 확인 가능
        if (MeshComponent)
        {
            UE_LOG(LogTemp, Log, TEXT("MeshComponent exists"));
        }
        
        // ✅ 부모-자식 관계 확인 가능
        if (ChildComponent->GetAttachParent() == MeshComponent)
        {
            UE_LOG(LogTemp, Log, TEXT("Attachment hierarchy is set"));
        }
        
        // ❌ 컴포넌트 기능은 아직 사용 불가
        // MeshComponent->SetSimulatePhysics(true); // 동작 안 할 수 있음
        
        // ❌ 컴포넌트 간 복잡한 상호작용 불가
    }

    virtual void PostInitializeComponents() override
    {
        Super::PostInitializeComponents();
        UE_LOG(LogTemp, Warning, TEXT("PostInitializeComponents"));
        
        // ✅ 컴포넌트 완전히 사용 가능
        if (MeshComponent)
        {
            MeshComponent->SetSimulatePhysics(true);
            MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }
        
        // ✅ 컴포넌트 간 상호작용
        if (ChildComponent)
        {
            FVector ParentLocation = MeshComponent->GetComponentLocation();
            ChildComponent->SetWorldLocation(ParentLocation + FVector(0, 0, 100));
        }
        
        // ✅ 물리, 렌더링 설정
        // ✅ 바인딩, 델리게이트 연결
    }

    virtual void BeginPlay() override
    {
        Super::BeginPlay();
        UE_LOG(LogTemp, Warning, TEXT("BeginPlay"));
        
        // 게임플레이 로직
    }
};
```

### 실제 사용 시나리오

#### PostRegisterAllComponents 사용

```cpp
virtual void PostRegisterAllComponents() override
{
    Super::PostRegisterAllComponents();
    
    // 시나리오 1: 동적으로 컴포넌트 추가
    if (!bExtraComponentCreated)
    {
        UStaticMeshComponent* ExtraComponent = NewObject<UStaticMeshComponent>(this, TEXT("ExtraMesh"));
        ExtraComponent->RegisterComponent();
        ExtraComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
        bExtraComponentCreated = true;
    }
    
    // 시나리오 2: 컴포넌트 계층 구조 검증
    TArray<USceneComponent*> Children;
    RootComponent->GetChildrenComponents(false, Children);
    UE_LOG(LogTemp, Log, TEXT("Component has %d children"), Children.Num());
}
```

#### PostInitializeComponents 사용 (더 일반적)

```cpp
virtual void PostInitializeComponents() override
{
    Super::PostInitializeComponents();
    
    // 시나리오 1: 컴포넌트 간 참조 설정
    if (MeshComponent && HealthComponent)
    {
        // MeshComponent의 이벤트를 HealthComponent와 연결
        MeshComponent->OnComponentHit.AddDynamic(this, &AMyActor::OnMeshHit);
    }
    
    // 시나리오 2: 물리 설정
    if (MeshComponent)
    {
        MeshComponent->SetSimulatePhysics(true);
        MeshComponent->SetMassOverrideInKg(NAME_None, 100.0f);
    }
    
    // 시나리오 3: 머티리얼 동적 인스턴스 생성
    if (MeshComponent)
    {
        DynamicMaterial = MeshComponent->CreateDynamicMaterialInstance(0);
        if (DynamicMaterial)
        {
            DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FLinearColor::Red);
        }
    }
    
    // 시나리오 4: 다른 컴포넌트 찾기
    UChildActorComponent* ChildActorComp = FindComponentByClass<UChildActorComponent>();
    if (ChildActorComp && ChildActorComp->GetChildActor())
    {
        // 자식 액터와 상호작용
    }
}
```

### 캐릭터 클래스 예제

```cpp
UCLASS()
class MYGAME_API AMyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AMyCharacter()
    {
        // 컴포넌트 생성
        WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon"));
        WeaponMesh->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));
        
        HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
    }

protected:
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* WeaponMesh;
    
    UPROPERTY(VisibleAnywhere)
    UHealthComponent* HealthComponent;

    virtual void PostRegisterAllComponents() override
    {
        Super::PostRegisterAllComponents();
        
        // 소켓 존재 여부만 확인
        if (GetMesh() && GetMesh()->DoesSocketExist(TEXT("WeaponSocket")))
        {
            UE_LOG(LogTemp, Log, TEXT("WeaponSocket exists"));
        }
        
        // 이 시점에는 메시가 완전히 로드 안 됐을 수 있음
    }

    virtual void PostInitializeComponents() override
    {
        Super::PostInitializeComponents();
        
        // ✅ 이제 안전하게 소켓에 부착 가능
        if (WeaponMesh && GetMesh())
        {
            WeaponMesh->AttachToComponent(
                GetMesh(), 
                FAttachmentTransformRules::SnapToTargetNotIncludingScale,
                TEXT("WeaponSocket")
            );
        }
        
        // ✅ 컴포넌트 간 연결
        if (HealthComponent)
        {
            HealthComponent->OnHealthChanged.AddDynamic(this, &AMyCharacter::HandleHealthChanged);
        }
        
        // ✅ 애니메이션 인스턴스 가져오기
        if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
        {
            // 애니메이션 설정
        }
    }

    UFUNCTION()
    void HandleHealthChanged(float NewHealth)
    {
        // 체력 변경 처리
    }
};
```

### 요약 비교표

| 항목 | PostRegisterAllComponents | PostInitializeComponents |
|------|---------------------------|--------------------------|
| **호출 시점** | 컴포넌트 등록 직후 | 컴포넌트 초기화 완료 후 |
| **컴포넌트 존재** | ✅ 존재함 | ✅ 존재함 |
| **컴포넌트 초기화** | ❌ 아직 안 됨 | ✅ 완료됨 |
| **물리/렌더링** | ❌ 준비 안 됨 | ✅ 준비됨 |
| **컴포넌트 기능 사용** | ⚠️ 제한적 | ✅ 완전히 가능 |
| **사용 빈도** | 🔵 드물게 | 🟢 자주 사용 |
| **주 용도** | 계층 구조 검증, 동적 추가 | 컴포넌트 간 연결, 설정 |

### 결론

**일반적으로 `PostInitializeComponents`를 사용하세요!**

- ✅ **PostInitializeComponents**: 컴포넌트가 완전히 준비됨 - **대부분의 경우 이것 사용**
- ⚠️ **PostRegisterAllComponents**: 컴포넌트가 등록만 됨 - **특수한 경우에만 사용**

**90% 이상의 상황에서는 `PostInitializeComponents`면 충분합니다!**