# TTTK 251013
## ContentManager Config 시스템

# Start Log
- 20251013 00:00
- 작업자 : VaVamVa

### To Do

**BaseContentManager 완성을 목표로 작업 진행**

1. **PDA_ContentConfig 클래스 설계 및 생성**
   - UPrimaryDataAsset 기반 클래스 생성
   - SpawnPoints, ContentName, Custom Settings 등 필드 정의
   - UPDA_MapList 구조 참고

2. **BaseContentManager contentConfig 연동**
   - InitializeConfig 주석 해제 및 수정
   - contentConfig 필드를 구체적인 타입으로 변경
   - Blueprint에서 설정 가능하도록 UPROPERTY 설정

3. **TeleportPlayersIntoContent 구현 완성**
   - contentConfig.spawnPoints 기반 텔레포트 로직 구현
   - 플레이어 수보다 SpawnPoint가 적을 경우 순환 배치

4. **OnPlayerAction 기본 구조 설계**
   - 콘텐츠별 플레이어 액션 처리 인터페이스 정의
   - Virtual 함수로 자식 클래스에서 오버라이드 가능하게

5. **ContentEntryComponent-BaseContentManager 데이터 흐름 검증**
   - settings (FContentEntrySettings) → contentConfig 변환 로직
   - StartContentInternal → InitializeContent → StartContent 호출 순서 확인

## Compact Log 1
- 20251013 (시각 미상)

### 요약 내용

**완료된 작업:**
1. **FContentConfig 구조 설계 및 구현**
   - USTRUCT 기반으로 ContentConfig 생성
   - contentName, description, playerSpawnPoints, timeLimit 등 기본 필드 구현
   - bRecordingScore, bRecordingTime 플래그 추가
   - customSettings, customRecordingData 확장 필드 추가
   - DataTable 확장성 고려 (contentRowName 추가)

2. **BaseContentManager와 연동**
   - contentConfig를 FContentConfig 타입으로 변경
   - contentTimeLimit 제거 및 Config로 이동
   - InitializeConfig, StartContent, EndContent에서 contentConfig 활용
   - HasAuthority() 체크로 Server 전용 로직 보호

3. **Step-by-Step 구현 방식 문서화**
   - CLAUDE.md에 협업 프로토콜 추가
   - 방향 제시 → 구현 → 검증 → 다음 단계 흐름 정립

**검증된 이슈:**
1. FContentParticipatingPlayerData의 EditCondition 참조 오류
2. InitializeContent에서 Config 플래그 활용 미흡
3. InitializeConfig의 Tag 기반 SpawnPoint 로직 주석 처리
4. customSettings 타입 확정 필요 (TArray vs TMap)
5. contentRowName의 활용 로직 미구현 (DataTable 연동)

**다음 작업 예정:**
- TeleportPlayersIntoContent 구현
- OnPlayerAction 구조 설계
- ContentEntryComponent 데이터 흐름 검증

### 확인 필요 사항 (다음 세션)

1. **EditCondition 이슈 해결 방법**
   - FContentParticipatingPlayerData의 EditCondition 제거 vs 유지

2. **customSettings 타입 결정**
   - `TArray<FName>` (플래그 리스트) vs `TMap<FName, FString>` (Key-Value)

3. **DataTable 구현 시점**
   - 지금 contentConfigTable 구현 vs 나중으로 연기

4. **runtimeSpawnPoints 분리**
   - 별도 Transient 배열로 Runtime SpawnPoint 관리 필요 여부

5. **InitializeConfig의 Tag 기반 SpawnPoint**
   - GetAllActorsWithTag 로직 활성화 및 테스트

### Commit 정보

"Feat: BaseContentManager Config 시스템 구현"

1. "FContentConfig 구조체 생성"
   - USTRUCT 기반 콘텐츠 설정 구조 정의
   - 기본 정보, 스폰 위치, 게임플레이 설정, 커스텀 필드 포함

2. "BaseContentManager contentConfig 연동"
   - contentTimeLimit 제거 및 Config로 이동
   - InitializeConfig, StartContent, EndContent에서 Config 활용
   - HasAuthority() 체크로 멀티플레이 안정성 강화

3. "Step-by-Step 구현 방식 문서화"
   - CLAUDE.md에 협업 프로토콜 추가

## Compact Log 2
- 20251014 오후

### 요약 내용

**완료된 작업:**

1. **InteractableComponent 멀티플레이 구조 구현**
   - Server RPC (`Server_TryInteract`) 추가
   - Multicast RPC (`Multicast_TryInteract`) 추가
   - 클라이언트 전용 Delegate (`onChangeState`) 유지
   - 멀티플레이용 Delegate (`onRequestInteraction`) 추가
   - Client → Server → Multicast → All Clients 흐름 구현

2. **InteractableFeedbackSettings 구조 개선**
   - `outlineComponent` (UPrimitiveComponent*) → `outlinedMeshComponent` (TWeakObjectPtr<UMeshComponent>) 변경
   - `UpdateAvailablePrimitiveComponents()` 함수 추가 (자동으로 MeshComponent 탐색)
   - Outline 초기화 로직 개선

3. **ViewComponent 수정**
   - `params` → `queryParams`, `responseParams`로 분리
   - LineTrace 시작 위치 조정 (ForwardVector * 10.f 오프셋 추가)
   - `EnableTrace` 타이머 시작 딜레이 추가 (2.f)
   - Debug Draw 추가 (Red Sphere at ImpactPoint)

4. **MainPlayer Interaction 로직 수정**
   - `OnViewInteractableActor` BlueprintNativeEvent → 일반 함수로 변경
   - Focused Actor 중복 체크 로직 추가
   - F키 입력 시 `TryInteract()` 직접 호출로 변경
   - `GetFocusedActor()` Getter 함수 추가

5. **BaseContentManager 수정**
   - `StartContent`에서 `HasAuthority()` 체크 추가
   - `contentTimeLimit` → `contentConfig.timeLimit` 변경
   - Timer 중복 실행 방지 로직 추가

**검증된 이슈:**

1. **멀티플레이 Authority 문제**
   - Host는 작동하지만 Client에서 `SetActorLocation`이 작동하지 않음
   - 원인: Client는 PlayerController/Pawn 위치 변경 Authority가 없음
   - 해결 방향: Server RPC에서 위치 변경 또는 확장 가능한 구조 설계 필요

2. **확장성 고려사항**
   - Interface 패턴 (Actor별 다른 반응)
   - BlueprintNativeEvent 패턴 (Component 확장)
   - Server 전용 Delegate 패턴 (Blueprint 자유도)

**다음 작업 예정:**
- InteractableComponent 확장 가능한 구조 설계
- Server RPC에서의 Interaction 처리 로직 구현
- Blueprint에서 멀티플레이 안전하게 처리하는 패턴 확립

### Git Diff 요약

**주요 변경 파일:**
1. `InteractableComponent.h/.cpp` - 멀티플레이 RPC 구조 추가
2. `InteractableFeedbackSettings.h/.cpp` - Outline 시스템 개선
3. `ViewComponent.h/.cpp` - LineTrace 로직 개선
4. `MainPlayer.h/.cpp` - Interaction 흐름 수정
5. `BaseContentManager.h/.cpp` - Authority 체크 추가

**주요 변경사항:**
```cpp
// InteractableComponent - 멀티플레이 Delegate 추가
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRequestInteraction, APawn*, player);

// Server → Multicast RPC 체인
void Server_TryInteract_Implementation(APawn* player)
{
    Multicast_TryInteract(player);
}

void Multicast_TryInteract_Implementation(APawn* player)
{
    onRequestInteraction.Broadcast(player);
}

// InteractableFeedbackSettings - 타입 변경
TWeakObjectPtr<UMeshComponent> outlinedMeshComponent;  // 기존: TObjectPtr<UPrimitiveComponent>

// ViewComponent - 파라미터 분리
FCollisionQueryParams queryParams;
FCollisionResponseParams responseParams;

// MainPlayer - Focused Actor 관리 개선
void OnViewInteractableActor(const FHitResult& hitResult)
{
    if (focusedActor)
    {
        if (hitResult.GetActor() == focusedActor) {return;}
        focusedActor->FindComponentByClass<UInteractableComponent>()->TryDeactivateInteractable(...);
        focusedActor = nullptr;
    }
    // ... 새로운 Actor Focus
}
```

### 확인 필요 사항 (다음 세션)

1. **멀티플레이 확장성 패턴 선택**
   - 옵션 A: Interface 패턴 (IInteractableResponse)
   - 옵션 B: BlueprintNativeEvent (ProcessInteractionOnServer)
   - 옵션 C: Server 전용 Delegate (onServerInteract)
   - 추천: 옵션 B + C 조합

2. **Server RPC 검증 로직 추가**
   - 거리 검증 (치트 방지)
   - Pawn 유효성 검증
   - Authority 확인

3. **Blueprint 사용 패턴 정립**
   - `onChangeState`: 클라이언트 전용 (UI, VFX)
   - `onRequestInteraction`: 멀티플레이용 (게임 로직)
   - Server에서 처리해야 할 것과 Client에서 처리할 것 명확히 구분

### Commit 정보 (예정)

**Commit 1: "Feat: InteractableComponent 멀티플레이 RPC 구조 추가"**
- Server_TryInteract, Multicast_TryInteract RPC 추가
- onRequestInteraction Delegate 추가 (멀티플레이용)
- Client → Server → Multicast 흐름 구현

**Commit 2: "Refactor: InteractableFeedbackSettings Outline 시스템 개선"**
- outlineComponent → outlinedMeshComponent (TWeakObjectPtr<UMeshComponent>) 변경
- UpdateAvailablePrimitiveComponents 자동 탐색 기능 추가
- Outline 초기화 로직 개선

**Commit 3: "Fix: ViewComponent LineTrace 및 MainPlayer Interaction 로직 수정"**
- ViewComponent: queryParams, responseParams 분리
- ViewComponent: LineTrace 시작 위치 오프셋 추가
- MainPlayer: OnViewInteractableActor 중복 체크 개선
- MainPlayer: TryInteract 직접 호출로 변경

**Commit 4: "Fix: BaseContentManager Authority 체크 추가"**
- StartContent에 HasAuthority() 체크 추가
- contentTimeLimit → contentConfig.timeLimit 변경
- Timer 중복 실행 방지

## End Log
- 20251014 오후
- 작업자 : VaVamVa

### 오늘 한 일

1. "InteractableComponent 멀티플레이 RPC 구조 설계 및 구현"
2. "Interaction 시스템 멀티플레이 이슈 진단 및 해결 방향 논의"
3. "ViewComponent, MainPlayer, BaseContentManager 버그 수정"
4. "InteractableFeedbackSettings Outline 시스템 리팩토링"

### 미완료 작업

1. **InteractableComponent 확장 가능한 구조 설계**
   - Server RPC에서 직접 처리 vs Blueprint 확장성
   - Interface, BlueprintNativeEvent, Delegate 패턴 중 선택 필요

2. **멀티플레이 Authority 문제 해결**
   - Client에서 SetActorLocation 작동하지 않는 문제
   - Server에서 위치 변경 후 자동 복제 구조 구현 필요

