# TTTK 251005

# Start Log
- 20251005 09:00
- 작업자 : VaVamVa

### To Do

1. ContentNPC 구현
    - ContentEntryComponent를 소유하는 NPC Character 생성
    - NPC Mesh 및 기본 설정 (Collision, Replication)
    - ContentEntryComponent 초기화 및 Owner 설정

2. Player 외곽선 시스템 구현
    - Player LineTrace 시스템 (Tick 또는 Timer 기반)
    - Interactable 감지 (Interface 또는 Tag 기반)
    - CustomDepth + Post Process 외곽선 활성화/비활성화

3. Player Interact Input 설정
    - Enhanced Input에 Interact Action 추가
    - Input Mapping Context 설정
    - ContentEntryComponent::RequestJoinLobby() 호출 연결

4. BaseContentManager 설계
    - AActor 기반 Base Class 생성
    - Virtual 함수 정의 (StartContent, EndContent, OnPlayerQuit)
    - 상태 관리 (EContentState)
    - Participants 관리 및 결과 계산

---

## Compact Log 1
- 20251005 14:30

### 요약 내용

**ContentNPC 및 ContentEntryComponent 구조 재설계 완료**

#### 1. 설계 개선 - Component 캡슐화 강화
**기존 문제:**
- NPC가 Widget, InteractionSphere 개별 소유
- ContentEntry 설정값(outline, color)과 분리
- 중복된 책임 분산

**개선 결과:**
- ✅ ContentEntryComponent가 Widget, InteractionSphere 모두 소유
- ✅ Outline, Color, Widget 설정이 한 곳에 집중
- ✅ NPC는 ContentEntry만 생성하면 모든 기능 동작
- ✅ 재사용성 향상 (다른 Actor에도 부착 가능)

#### 2. ContentNPC 구현 완료
**파일:** `Public/NPC/ContentNPC.h`, `Private/NPC/ContentNPC.cpp`

**주요 구현:**
- Character 기반 NPC Actor
- ContentEntryComponent 소유 (단일 의존성)
- Interactable Interface 구현
- Network 설정 (Replicate, Movement 비활성)

**핵심 함수:**
```cpp
// Outline 제어
void SetOutlineEnabled(bool bEnabled);

// Interact Interface 구현
void Interact_Implementation(AMainPlayer* player);
```

#### 3. ContentEntryComponent 개선
**파일:** `Public/NPC/ContentEntryComponent.h`, `Private/NPC/ContentEntryComponent.cpp`

**추가된 기능:**
- ✅ `TObjectPtr<UWidgetComponent> interactWidget` 소유
- ✅ Constructor에서 Widget 생성 및 초기 설정
- ✅ BeginPlay에서 Owner에 Attachment
- ✅ NPC Mesh CustomDepth 초기화 (비활성 상태)

**Constructor 개선:**
```cpp
// 변수 초기화 순서 수정 (interactionRadius 먼저)
interactionRadius = 300.0f;
outlineStencilValue = 252;
outlineColor = FLinearColor::Green;

// 이후 Component 생성
interactionSphere = CreateDefaultSubobject<USphereComponent>(...);
interactWidget = CreateDefaultSubobject<UWidgetComponent>(...);
```

**BeginPlay 추가:**
```cpp
// InteractionSphere Attachment
interactionSphere->SetupAttachment(GetOwner()->GetRootComponent());
interactionSphere->RegisterComponent();

// Widget Attachment & 초기화
interactWidget->SetupAttachment(GetOwner()->GetRootComponent());
interactWidget->RegisterComponent();
interactWidget->SetVisibility(false);

// NPC Mesh CustomDepth 초기 설정
if (ACharacter* npc = Cast<ACharacter>(GetOwner()))
{
    npc->GetMesh()->SetRenderCustomDepth(false);
    npc->GetMesh()->SetCustomDepthStencilValue(0);
}
```

#### 4. Interactable Interface 생성
**파일:** `Public/Interfaces/Interactable.h`

**ContentNPC 적용:**
- `AContentNPC : public ACharacter, public IInteractable`
- `Interact_Implementation()` → `contentEntry->RequestJoinLobby(player)` 호출

#### 5. UE C++ 개념 학습

**CreateDefaultSubobject 동작 원리:**
- 동기(Synchronous) 함수 - 반환 즉시 완전 초기화된 객체
- CDO(Class Default Object)에서 기본값 복사
- Constructor에서는 절대 실패하지 않음 (nullptr 반환 안함)
- Runtime NewObject와 다름 (NewObject는 실패 가능)

**Constructor vs BeginPlay:**
- Constructor: Component 생성, 기본값 설정
  - Owner는 아직 nullptr 가능
  - Attachment 불가 (Owner의 RootComponent 미생성)
- BeginPlay: Attachment, Delegate 바인딩, Owner 참조 작업
  - Owner 완전히 초기화됨
  - Component 간 연결 작업 수행


#### 6. 외곽선 시스템 설계 논의

**설계 방향:**
- Player의 화면 중앙 LineTrace로 NPC 감지
- Hit된 NPC에 Outline 활성화
- ViewComponent(ActorComponent) 생성 예정

**IsLocallyControlled 이슈 해결:**
- ❌ `AActor::IsLocallyControlled()` 없음
- ✅ `APawn::IsLocallyControlled()` 사용
- ViewComponent BeginPlay에서 Owner를 Pawn으로 캐스팅 필요

**Delegate 설계 개선:**
- Delegate: Target 변경 알림만 (OnInteractTargetChanged)
- Outline 제어: ViewComponent가 직접 수행
- 1:1 관계 유지 (동시에 여러 NPC Outline 활성화 방지)

#### 7. 다음 작업 (보류)
- ViewComponent 구현 (Player에서 진행 예정)
- LineTrace 로직
- Enhanced Input Interact Action 연결
- BaseContentManager 설계

### 파일 변경 사항

**생성된 파일:**
- `Public/Interfaces/Interactable.h`

**수정된 파일:**
- `Public/NPC/ContentNPC.h`
  - Interactable Interface 구현
  - Widget 관련 변수 제거 (ContentEntry로 이동)
- `Private/NPC/ContentNPC.cpp`
  - Widget Include 제거
  - BeginPlay 간소화
  - Interact_Implementation 추가
- `Public/NPC/ContentEntryComponent.h`
  - Widget 변수 추가 (Outline Category)
  - Getter 추가 (GetOutlineDepthStencilValue)
- `Private/NPC/ContentEntryComponent.cpp`
  - Constructor 변수 초기화 순서 수정
  - Widget 생성 코드 추가
  - BeginPlay에 Widget & Mesh 초기화 추가

### Commit 정보
refactor: Redesign ContentNPC with enhanced component encapsulation

1. "ContentEntryComponent 캡슐화 강화"
   - Widget, InteractionSphere를 Component 내부로 통합
   - Outline 설정 및 시각 요소 통합 관리

2. "ContentNPC 구조 간소화"
   - ContentEntry 단일 의존성으로 변경
   - Interactable Interface 구현

3. "UE C++ 개념 학습 및 적용"
   - CreateDefaultSubobject 동작 원리 이해
   - Constructor/BeginPlay 역할 명확화
   - 변수 초기화 순서 수정

---

## Compact Log 2
- 20251006 16:30

### 요약 내용

**Interactable 시스템 전면 재설계: Interface → Component 기반 전환**

#### 1. 구조 재설계 완료

**이전 구조:**
- IInteractable Interface 기반
- InteractableVisualComponent (Outline + Widget)
- ViewComponent가 Interface Cast하여 제어

**최종 구조:**
- **UInteractableComponent** (ActorComponent)
  - 상태 관리 (Default/OutOfBound/Inactive/Active/Interacting)
  - 피드백 자동 처리 (Outline/Widget/Sound/VFX)
  - Delegate 제공 (Actor가 로직 구현)

**전환 이유:**
- ✅ Blueprint 친화성
- ✅ 런타임 추가/제거 가능
- ✅ Interface 다중 상속 문제 회피
- ✅ Component 재사용성 향상

---

#### 2. InteractableComponent 구현

**파일:**
- `Public/Interaction/InteractableComponent.h`
- `Private/Interaction/InteractableComponent.cpp`

**상태 관리:**
```cpp
enum class EInteractableState : uint8
{
    Default UMETA(Hidden),  // 초기 상태
    OutOfBound,             // 범위 밖
    Inactive,               // 조건 불만족
    Active,                 // 상호작용 가능
    Interacting             // 진행 중
};
```

**핵심 함수:**
```cpp
// Actor 제어
void OutOfBound();                              // 범위 밖 전환
void ActivateInteractable();                    // Active 전환
void DeactivateInteractable();                  // Inactive 전환
bool TryInteract(AMainPlayer* player);          // Interact 실행
void FinishInteracting(const EInteractableState& newState);  // 종료

// 자동 피드백
void UpdateVisuals();     // Outline + Widget 자동 업데이트
void PlaySound(...);      // Sound 재생
void PlayEffect(...);     // Niagara/Particle 재생
```

**Delegate:**
```cpp
FOnInteractRequest onInteract;    // Interact 실행 시
FOnChangeState onChangeState;     // 상태 변경 시
```

---

#### 3. InteractableFeedbackSettings 구현

**파일:**
- `Public/Data/InteractableFeedbackSettings.h`
- `Private/Data/InteractableFeedbackSettings.cpp`

**BitFlag 시스템:**
```cpp
enum class EEffectType : uint8
{
    None = 0 << 0,
    Outline = 1 << 0,   // 0b00001
    Widget = 1 << 1,    // 0b00010
    Sound = 1 << 2,     // 0b00100
    Niagara = 1 << 3,   // 0b01000
    Particle = 1 << 4   // 0b10000
};
ENUM_CLASS_FLAGS(EEffectType)
```

**설정 구조:**
```cpp
struct FInteractableFeedbackSettings
{
    uint8 effectType;  // Bitmask

    // Outline
    UPrimitiveComponent* outlineComponent;
    int32 outlineStencilValue = 252;
    FLinearColor outlineColor;

    // Widget
    TSubclassOf<UUserWidget> interactionGuideWidgetClass;
    FVector widgetOffset;

    // Sound
    USoundBase* activatedSound;
    USoundBase* interactedSound;

    // VFX
    UNiagaraSystem* interactedNiagaraVFX;
    UParticleSystem* interactedParticleVFX;
};
```

**Enable 함수:**
```cpp
void EnableOutline(const bool& bEnabled,
                   UPrimitiveComponent* inOutlineComponent,
                   FLinearColor inOutlineColor = FLinearColor::Green);

void EnableWidget(const bool& bEnabled,
                  TSubclassOf<UUserWidget> inInteractionGuideWidgetClass,
                  FVector inWidgetOffset = FVector(0, 0, 100));

void EnableSound(const bool& bEnabled,
                 USoundBase* inInteractedSound,
                 USoundBase* inActivatedSound = nullptr);

void EnableNiagara(const bool& bEnabled,
                   UNiagaraSystem* inInteractedNiagaraVFX);

void EnableParticle(const bool& bEnabled,
                    UParticleSystem* inInteractedParticleVFX);
```

**비트 체크 로직:**
```cpp
static bool IsEnableEffect(const uint8& bitmask, const EEffectType& bitflag)
{
    return (bitmask & static_cast<uint8>(bitflag)) > 0;
}

bool IsOutlineOn() const {return IsEnableEffect(effectType, EEffectType::Outline);}
bool IsWidgetOn() const {return IsEnableEffect(effectType, EEffectType::Widget);}
// ...
```

---

#### 4. ViewComponent 개선

**Owner Eye 방식 (GetControlRotation):**
```cpp
FVector startPos;
FVector direction;

// Camera 우선
if (IsValid(ownerEye) && ownerEye->IsA<UCameraComponent>())
{
    startPos = ownerEye->GetComponentLocation();
    direction = ownerEye->GetForwardVector();
}
// 없으면 ControlRotation
else
{
    startPos = pawnOwner->GetPawnViewLocation();
    direction = pawnOwner->GetControlRotation().Vector();
}
```

**LineTrace 개선:**
- Hit 실패 시 Target nullptr 초기화
- Target 변경 시에만 Delegate 브로드캐스팅
- InteractableComponent 체크 방식

**UpdateOutline 구조:**
```cpp
void UpdateOutline(AActor* newTarget)
{
    // 이전 Target → InteractableComponent Deactivate
    // 새 Target → InteractableComponent Activate
}
```

---

#### 5. 파일 구조 정리

**이동:**
- `NPC/ContentEntryComponent` → `Interaction/ContentEntryComponent`

**생성:**
- `Interaction/InteractableComponent.h/cpp`
- `Data/InteractableFeedbackSettings.h/cpp`

**삭제 예정:**
- `Interfaces/Interactable.h` (Component 방식으로 완전 전환)

---

#### 6. 설계 개념 Q&A

**Q1: Owner Eye를 SceneComponent로 둔 이유와 개선 방안**

**질문:**
- Player, Non-Playable Pawn 모두 사용 가능하게 하려고 SceneComponent로 설정
- 더 좋은 방법이 있는지?

**답변:**
- **Option 1 (선택):** GetControlRotation() 사용
  - Camera 유무 무관, Player/AI 지원, 정확한 시선 추적
- **Option 2:** Camera 필수화 (단순하지만 유연성 감소)
- **Option 3:** Blueprint 지정 (최대 유연성)

**최종 결정:** Option 1 (GetControlRotation)

---

**Q2: Outline/Widget 구조 분리 필요성**

**질문:**
1. Outline은 NPC뿐만 아니라 Actor(Prop)에도 표시 가능
2. ContentEntryComponent의 interactWidget을 분리해야 함
3. 분리 방법 고민

**답변:**
- **방안 A (선택):** InteractableVisualComponent
  - SRP, 재사용성, 확장성 우수
- **방안 B:** Interface에서 직접 제공
  - 중복 코드 발생
- **방안 C:** WidgetComponent 상속
  - Outline/Widget 항상 결합

**최종 결정:** 방안 A → Component 기반 완전 전환

---

**Q3: Interactable 확장 설계**

**질문:**
1. InteractableComponent로 넓은 이름 재정의
2. 상태에 따라 Outline/Widget 등 표시 여부 결정
3. 각 Actor에서 Interact Response 정의

**답변:**
- **핵심 원칙:** Component = 상태 관리 + 피드백, Actor = 로직 구현
- **상태:** Default/OutOfBound/Inactive/Active/Interacting
- **피드백:** BitFlag로 Outline/Widget/Sound/VFX 조합

**사용 예시:**
- ContentNPC: LookingAt → Active
- 제기차기: Timing + FootPosition → Active
- Door: Overlap → Active

---

**Q4: LookingAt vs 다양한 Interact 조건**

**질문:**
- 제기차기는 높이/발 위치가 기준
- Outline이 표현 안될 수 있는 경우

**답변:**
- **핵심:** LookingAt ≠ Interact 조건
- **각 Actor가 Active 조건 결정**

```cpp
// ContentNPC
void OnPlayerLookingAtChanged(bool bIsLookingAt)
{
    interactableComponent->SetActive(bIsLookingAt);
}

// 제기차기
void CheckKickCondition()
{
    bool bCanKick = bTimingGood && bFootInRange && bHeightCorrect;
    interactableComponent->SetActive(bCanKick);
}
```

**피드백 선택:**
- Use Outline: true/false
- Use Widget: true/false
- Widget Always Visible: true/false

---

**Q5: InteractableVisualComponent vs Interactable Interface 통합**

**질문:**
- 한 파일에 선언하는 것은?

**답변:**
- **방안 A:** 한 파일 통합
  - Include 간소화, 응집도 증가
  - Interface 재사용성 감소, SRP 위반, 순환 의존성 위험
- **방안 B (권장):** 분리 유지
  - 명확한 역할, 확장성, 언리얼 관례
- **방안 C:** Namespace 활용

**최종 결정:** Interface 완전 제거 → Component만 사용

---

#### 7. 최종 구조

```
Player (ViewComponent)
    ↓ LineTrace
    ↓ FindComponentByClass
InteractableComponent
    ↓ SetActive/Deactivate
    ↓ UpdateVisuals (자동)
    ↓ Delegate Broadcast
Actor (ContentNPC/Door/제기차기)
    ↓ OnInteract 구현
    ↓ 실제 로직 수행
```

**핵심:**
- ViewComponent: Target 감지
- InteractableComponent: 상태 + 피드백
- Actor: 조건 판단 + 로직 구현

---

### 파일 변경 사항

**생성된 파일:**
- `Public/Interaction/InteractableComponent.h`
- `Private/Interaction/InteractableComponent.cpp`
- `Public/Data/InteractableFeedbackSettings.h`
- `Private/Data/InteractableFeedbackSettings.cpp`

**수정된 파일:**
- `Public/Components/ViewComponent.h`
  - UpdateOutline() 함수 추가
  - outlinedTarget 변수 추가
- `Private/Components/ViewComponent.cpp`
  - LineTrace Hit 실패 시 nullptr 초기화
  - Target 변경 시에만 Delegate 브로드캐스팅
  - GetControlRotation() 방식 적용 (예정)
- `Public/NPC/ContentNPC.h`
  - Interactable Interface 제거 (예정)
- `Private/NPC/ContentNPC.cpp`
  - Include 경로 수정

**이동된 파일:**
- `NPC/ContentEntryComponent` → `Interaction/ContentEntryComponent`

**삭제 예정:**
- `Public/Interfaces/Interactable.h`

---

### Commit 정보

feat: Implement InteractableComponent system with BitFlag feedback

1. "Interactable 시스템 재설계"
   - Interface 방식 → Component 기반 전환
   - 상태 관리 시스템 구현 (Default/OutOfBound/Inactive/Active/Interacting)
   - Actor가 조건 결정, Component가 상태/피드백 관리

2. "InteractableComponent 구현"
   - 상태 제어 함수 (Activate/Deactivate/TryInteract/FinishInteracting)
   - Delegate 제공 (onInteract, onChangeState)
   - 자동 피드백 처리 (UpdateVisuals, PlaySound, PlayEffect)

3. "BitFlag 피드백 시스템 구현"
   - EEffectType BitFlag Enum (Outline/Widget/Sound/Niagara/Particle)
   - FInteractableFeedbackSettings 구조체
   - Enable 함수로 플래그 + 설정값 동시 적용

4. "ViewComponent Owner Eye 개선"
   - GetControlRotation() 방식 설계
   - Camera 유무 무관하게 정확한 시선 추적
   - Player/AI Pawn 모두 지원

5. "파일 구조 정리"
   - ContentEntryComponent 이동 (NPC → Interaction)
   - Include 경로 수정

---

## End Log
- 20251006 17:00
- 작업자 : VaVamVa

### 오늘 한 일

1. **ContentNPC 및 ContentEntryComponent 구조 재설계**
   - Component 캡슐화 강화 (Widget, InteractionSphere 통합)
   - Interactable Interface 구현
   - UE C++ 개념 학습 (CreateDefaultSubobject, Constructor vs BeginPlay)

2. **Interactable 시스템 전면 재설계**
   - Interface 기반 → Component 기반 아키텍처 전환
   - InteractableComponent 구현 (상태 관리 + 자동 피드백)
   - BitFlag 피드백 시스템 구현 (Outline/Widget/Sound/VFX)
   - ViewComponent 개선 (GetControlRotation 방식)

3. **설계 개념 정립**
   - Actor가 활성화 조건 결정, Component가 상태/피드백 관리
   - LookingAt ≠ Interact 조건 (각 Actor마다 다른 조건)
   - Blueprint 친화적 구조 설계
