# TTTK 251017
## 동적 컴포넌트 생성 및 World 컨텍스트

# Start Log
- 20251017 08:58
- 작업자 : VaVamVa

### To Do

1. "다음 작업 지시 대기" :
    - Interaction 시스템 멀티플레이 초기화 및 권한 문제 해결 완료
    - 새로운 기능 개발, 버그 수정, 또는 최적화 작업 준비 상태
    - 사용자의 다음 작업 요청을 기다림

---

## Compact Log 1
- 20251017 20:08

### 요약 내용

InteractableComponent의 동적 컴포넌트 생성 및 멀티플레이 안전성 개선 작업 완료

**1. NewObject World 컨텍스트 문제 해결**
- 문제: `NewObject<USphereComponent>()`로 생성 시 Outer 미지정으로 인한 "Ensure condition failed: MyOwnerWorld" 런타임 에러
- 해결: `NewObject<T>(owner, ...)` 형태로 Outer 명시
- 추가: `bWantsInitializeComponent = true` 플래그 설정으로 InitializeComponent 호출 활성화

**2. InitializeComponent 구현**
- InteractionSphere를 InitializeComponent에서 생성 및 등록
- BeginPlay에서는 Collision 설정만 수행 (역할 분리)
- 이벤트 바인딩을 RootComponent 유효성 검사 블록 내부로 이동 (안전성 향상)

**3. PostEditChangeProperty 구현**
- 에디터에서 interactionRadius 값 변경 시 실시간 반영
- `#if WITH_EDITOR` 매크로로 에디터 전용 코드 분리

**4. Widget World Space 설정**
- interactionGuideComponent를 World Space로 설정
- TranslucencySortPriority, SetAbsolute 등 렌더링 옵션 구성

**5. 안전성 검증 및 개선**
- InteractionComponent::Server_FinishInteraction에 nullptr 체크 추가
- InteractionComponent::Server_Interact에 nullptr 체크 추가
- InteractableComponent::InInteractableRange에 playerInRange nullptr 체크 추가
- 디버그 로그 정리 (프로덕션 준비)

**6. 코드 검증 및 아키텍처 분석**

**시스템 아키텍처 도식:**
```
┌─────────────────────────────────────────────────────────────┐
│ MainPlayer (AUE_TTTKCharacter)                              │
│ ├─ ViewComponent (UActorComponent)                          │
│ │   ├─ LineTrace 실행 (Timer 기반, 0.05s 간격)             │
│ │   ├─ OnViewSthByLineTrace 델리게이트                     │
│ │   └─ traceDistance, halfViewAngle 설정                    │
│ ├─ InteractionComponent (UActorComponent)                   │
│ │   ├─ [Replicated] focusingActor (현재 보는 Actor)        │
│ │   ├─ [Replicated] possessingInteractable (상호작용 중)   │
│ │   ├─ InteractKeyInput() - 로컬 입력 처리                 │
│ │   ├─ FocusInteractableActor() - Focus 전환               │
│ │   ├─ Server_Focus() [RPC]                                 │
│ │   ├─ Server_Interact() [RPC]                              │
│ │   └─ Server_FinishInteraction() [RPC]                     │
│ └─ GetFocusedActor() - InteractionComponent 조회            │
└─────────────────────────────────────────────────────────────┘
                              ↓ LineTrace Hit
┌─────────────────────────────────────────────────────────────┐
│ InteractableActor (Any AActor)                              │
│ └─ InteractableComponent (UActorComponent)                  │
│     ├─ [Dynamic] interactionSphere (USphereComponent)       │
│     │   ├─ InitializeComponent에서 NewObject 생성          │
│     │   ├─ Overlap 이벤트 → InRange/OutOfBound             │
│     │   └─ PostEditChangeProperty로 반지름 실시간 반영     │
│     ├─ [Dynamic] interactionGuideComponent (UWidgetComp)    │
│     │   ├─ BeginPlay에서 생성 (Widget 조건부)              │
│     │   ├─ World Space 배치                                 │
│     │   └─ Tick에서 카메라 방향 회전                       │
│     ├─ [State] clientState (EInteractableState)             │
│     │   └─ OutOfBound → InRange → Focused → Interacting    │
│     ├─ [Replicated] possessingPlayers (TArray<AMainPlayer>) │
│     ├─ feedbackSettings (Outline, Sound, VFX, Widget)       │
│     ├─ onChangeState 델리게이트                             │
│     ├─ onRequestInteraction 델리게이트                      │
│     ├─ TryActivateInteractable() - 로컬 Focus               │
│     ├─ TryInteract() - 로컬 피드백                          │
│     ├─ Multicast_TryInteract() [RPC]                        │
│     └─ Multicast_FinishInteracting() [RPC]                  │
└─────────────────────────────────────────────────────────────┘
```

**컴포넌트 생성 패턴:**
```
MainPlayer (Actor):
├─ CreateDefaultSubobject<ViewComponent>()        ✅ 생성자
└─ CreateDefaultSubobject<InteractionComponent>() ✅ 생성자

InteractableComponent (ActorComponent):
├─ NewObject<USphereComponent>(owner)             ✅ InitializeComponent
└─ NewObject<UWidgetComponent>(owner)             ✅ BeginPlay
```

**네트워크 로직 프로토콜:**

**[시나리오 1] Focus 전환**
```
[Client - Local Player]
1. ViewComponent::ShootLineTrace() (Timer)
2. → Hit Actor 발견
3. → OnViewSthByLineTrace.Broadcast(hitResult)
4. → MainPlayer::OnViewInteractableActor(hitResult)
5. → InteractionComponent::FocusInteractableActor(hitResult)
6.   ├─ 이전 focusingActor가 있으면:
7.   │   └─ InteractableComponent::TryDeactivateInteractable(pc) [로컬]
8.   │       └─ clientState = UnFocused
9.   ├─ Server_Focus(hitActor) [RPC → Server]
10.  └─ 새 hitActor가 있으면:
11.      └─ InteractableComponent::TryActivateInteractable(pc) [로컬]
12.          ├─ clientState = Focused
13.          ├─ Outline 활성화 (CustomDepth)
14.          ├─ Widget 표시
15.          └─ PlaySound (activatedSound)

[Server]
16. InteractionComponent::Server_Focus_Implementation(focusedActor)
17. └─ focusingActor = focusedActor (Replicated)

[All Clients]
18. focusingActor 복제 수신 (현재는 사용 안 함, 향후 확장용)
```

**[시나리오 2] 상호작용 시작 (F키 입력)**
```
[Client - Local Player]
1. MainPlayer::Tick() → F키 감지
2. → InteractionComponent::InteractKeyInput()
3.   └─ focusingActor가 있고, InteractableComponent 발견
4.       ├─ InteractableComponent::TryInteract(pc) [로컬 즉시]
5.       │   ├─ clientState = Interacting
6.       │   ├─ PlaySound (interactedSound)
7.       │   ├─ PlayEffect (Niagara/Particle)
8.       │   └─ onChangeState.Broadcast()
9.       └─ interactable->CanPossess() 체크 후:
10.          └─ Server_Interact(interactable) [RPC → Server]

[Server]
11. InteractionComponent::Server_Interact_Implementation(interactable)
12. ├─ possessingInteractable = interactable (Replicated)
13. └─ Multicast_TryInteract(owner) [Multicast → All Clients]

[All Clients]
14. InteractableComponent::Multicast_TryInteract_Implementation(player)
15. ├─ possessingPlayers.Add(player)
16. └─ onRequestInteraction.Broadcast(player)
17.     └─ [Blueprint/C++에서 구독] 실제 상호작용 로직 실행
```

**[시나리오 3] 상호작용 종료**
```
[Client - Local Player]
1. MainPlayer::Tick() → F키 감지 (상호작용 중)
2. → InteractionComponent::InteractKeyInput()
3.   └─ possessingInteractable이 있으면:
4.       ├─ InteractableComponent::FinishInteracting(pc, OutOfBound) [로컬]
5.       │   └─ clientState = OutOfBound
6.       └─ Server_FinishInteraction() [RPC → Server]

[Server]
7. InteractionComponent::Server_FinishInteraction_Implementation()
8. ├─ Multicast_FinishInteracting(owner) [Multicast → All Clients]
9. └─ possessingInteractable = nullptr (Replicated)

[All Clients]
10. InteractableComponent::Multicast_FinishInteracting_Implementation(player)
11. ├─ possessingPlayers.Remove(player)
12. └─ onRequestFinishInteraction.Broadcast(player)
13.     └─ [Blueprint/C++에서 구독] 정리 로직 실행
```

**[시나리오 4] Sphere Overlap (범위 진입/이탈)**
```
[All Clients - 각자 로컬 체크]
1. InteractionSphere::OnComponentBeginOverlap()
2. → Cast<AMainPlayer>(OtherActor)
3. → GetController<APlayerController>()
4. → IsLocalController() 체크 ✅
5.   └─ 로컬 컨트롤러만:
6.       └─ InteractableComponent::InInteractableRange(pc)
7.           ├─ playerInRange = pawn
8.           ├─ clientState = InRange
9.           ├─ 이미 Focus 중이면: TryActivateInteractable()
10.          └─ onChangeState.Broadcast()

// EndOverlap도 동일하게 로컬에서만 처리
```

**핵심 설계 특징:**
1. **로컬 피드백 우선**: TryInteract()는 클라이언트에서 즉시 실행 (반응성)
2. **서버 권한**: 실제 상태 변경은 Server RPC를 통해 검증
3. **Multicast 동기화**: 모든 클라이언트에 상태 전파
4. **로컬 Overlap**: 각 클라이언트가 자신의 InRange 상태를 독립적으로 관리
5. **Replication**: focusingActor, possessingInteractable, possessingPlayers 복제

**네트워크 권한 분리:**
```
Client Authority:
- Focus 시각화 (Outline, Widget)
- 로컬 피드백 (Sound, VFX)
- clientState (로컬 UI용)
- Overlap 범위 체크

Server Authority:
- possessingInteractable 관리
- possessingPlayers 배열 관리
- 상호작용 허용/거부 결정

Replicated Data:
- focusingActor (향후 확장용)
- possessingInteractable
- possessingPlayers
```

### Commit 정보

**Fix: InteractableComponent 동적 컴포넌트 생성 및 멀티플레이 안정성 개선**

1. "NewObject Outer 명시 및 InitializeComponent 활성화"
    - NewObject에 owner 파라미터 전달하여 World 컨텍스트 문제 해결
    - bWantsInitializeComponent = true 설정
    - InitializeComponent에서 InteractionSphere 생성 및 등록
    - 이벤트 바인딩을 RootComponent 검증 블록 내부로 이동

2. "에디터 실시간 반영 기능 추가"
    - PostEditChangeProperty 구현으로 interactionRadius 변경 즉시 반영
    - #if WITH_EDITOR 매크로 적용

3. "Widget 렌더링 설정 구성"
    - World Space 위젯 설정
    - Absolute transform 및 렌더링 옵션 설정

4. "멀티플레이 안전성 체크 강화"
    - InteractionComponent RPC 함수들에 nullptr 검증 추가
    - InteractableComponent::InInteractableRange에 playerInRange 검증 추가

5. "코드 정리 및 프로덕션 준비"
    - 디버그 로그 제거 및 주석 처리
    - #endif 매크로 형식 통일

---

## End Log
- 20251017 20:30
- 작업자 : VaVamVa

### 오늘 한 일

1. **InteractableComponent 동적 컴포넌트 생성 문제 해결**
   - "Ensure condition failed: MyOwnerWorld" 런타임 에러 원인 분석 및 해결
   - NewObject에 Outer 명시하여 World 컨텍스트 문제 해결

2. **InitializeComponent 구현**
   - bWantsInitializeComponent = true 플래그 추가
   - InteractionSphere 생성을 InitializeComponent로 이동
   - BeginPlay와 역할 분리 (생성 vs 설정)

3. **에디터 실시간 반영 기능 추가**
   - PostEditChangeProperty 구현
   - interactionRadius 변경 시 즉시 Sphere 반지름 업데이트
   - #if WITH_EDITOR 매크로 적용

4. **멀티플레이 안전성 강화**
   - InteractionComponent::Server_FinishInteraction에 nullptr 체크 추가
   - InteractionComponent::Server_Interact에 nullptr 체크 추가
   - InteractableComponent::InInteractableRange에 playerInRange 검증 추가

5. **Widget World Space 설정**
   - interactionGuideComponent를 World Space로 변경
   - TranslucencySortPriority, SetAbsolute 등 렌더링 옵션 구성

6. **전체 시스템 아키텍처 분석 및 문서화**
   - MainPlayer, ViewComponent, InteractionComponent, InteractableComponent 계층 구조 도식화
   - 네트워크 프로토콜 4가지 시나리오 문서화 (Focus, Interact Start, Finish, Sphere Overlap)
   - Client/Server Authority 분리 구조 정리

7. **로그 파일 정리**
   - 모든 로그 파일을 YYMMDD_{Title}.md 형식으로 이름 변경
   - 각 로그 파일에 한글 부제 추가

### 미완료 작업

없음 - 모든 작업 완료

### 다음 세션 작업 예정

1. **다음 작업 지시 대기**
   - 새로운 기능 개발, 버그 수정, 또는 최적화 작업 준비 상태
   - Enhanced Input 시스템으로 입력 처리 마이그레이션 (예정)

