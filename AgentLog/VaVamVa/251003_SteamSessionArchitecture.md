# TTTK 251003
## Steam 세션 시스템 아키텍처

# Start Log
- 20251003 00:00
- 작업자 : VaVamVa

### To Do

1. "Steam 세션 검색 결과 처리 로직 구현"
    - `OnFindSessionComplete()` Callback 구현
    - 검색된 세션 목록 UI 표시 로직 작성
    - 세션 검색 실패 처리 및 로그 추가

2. "Steam 세션 참가 로직 구현"
    - `JoinSession()` 호출 로직 구현
    - `OnJoinSessionComplete()` Callback 구현
    - `ClientTravel()`을 통한 서버 이동 처리

3. "Steam 네트워크 테스트"
    - Spacewar (AppID 480) 환경에서 실제 세션 생성/검색/참가 테스트
    - 로컬 및 인터넷 기반 멀티플레이 연결 검증

---

## Compact Log 1
- 20251003 23:00

### 요약 내용

**Steam 세션 시스템 아키텍처 재설계 및 구현**

#### 1. 아키텍처 설계 (책임 분리 원칙 적용)

**MainMenuSteam에서 Steam 로직 분리**:
- UI(MainMenuSteam)와 네트워크 로직(SteamSessionSubsystem) 분리
- `UGameInstanceSubsystem` 기반 설계 결정
- 레벨 전환에도 살아남는 생명주기 관리

**데이터 관리 시스템 구축**:
- Map 정보를 DataAsset으로 관리하기로 결정
- Dynamic Path 검색 문제 해결 (Asset Registry vs DataAsset 비교)
- `UDeveloperSettings` + `UPrimaryDataAsset` 조합 선택

---

#### 2. Data Layer 구축

**MapInfo.h (USTRUCT)**:
- Header-only 구조체 생성
- 필드: displayName, mapName, mapAsset, mapIcon, maxPlayers
- TSoftObjectPtr<UWorld>로 맵 경로 자동 관리

**PDA_MapList (UPrimaryDataAsset)**:
- `UPrimaryDataAsset` 상속으로 Asset Manager 통합
- 4개 헬퍼 함수 구현:
  - `GetMapInfoByName()` / `GetMapInfoByDisplayName()`
  - `GetMapObjectPathByName()` / `GetMapObjectPathByDisplayName()`
- `GetPrimaryAssetId()` 오버라이드 (고유 ID: "MapList", GetFName())

---

#### 3. Settings Layer 구축

**SteamSessionSettings (UDeveloperSettings)**:
- `Config=Game, DefaultConfig` 설정
- Project Settings UI에 노출 (`Edit > Project Settings > Steam Session`)
- MapListAsset 참조 (TSoftObjectPtr<UPDA_MapList>)
- 패키징 안전성 확보 (DefaultGame.ini 저장)

**모듈 의존성 추가**:
- `UE_TTTK.Build.cs`에 `DeveloperSettings` 모듈 추가 (LNK2019 에러 해결)

---

#### 4. UI Layer 구축

**SessionNodeData (UObject)**:
- ListView용 데이터 클래스
- 필드: hostName, mapIcon, mapName, currentPlayerCount, maxPlayerCount, ping, searchResultIndex

**SessionNode (UUserWidget + IUserObjectListEntry)**:
- ListView Entry Widget
- `NativeOnListItemObjectSet()` 구현
- SessionNodeData를 UI에 자동 바인딩
- BindWidget: hostName, mapIcon, mapName, playerCounter, hostPing

**CreatingSession (UUserWidget)**:
- 세션 생성 UI
- 맵 선택 드롭다운 (ComboBoxString)
- 최대 플레이어 수 조정 (SpinBox)
- Steam 닉네임 표시 (`IOnlineIdentityInterface::GetPlayerNickname()`)

**주요 기능**:
- PDA_MapList에서 맵 목록 로드 → 드롭다운 자동 채우기
- 맵 선택 시 해당 맵의 maxPlayers로 SpinBox 자동 제한
- SpinBox Delta 1.0f 설정 (1명씩 증감)
- 생성 버튼 → `SteamSessionSubsystem::CreateSession()` 호출

**코드 검증 및 버그 수정**:
- 🔴 치명적 버그 수정: `GetMapInfoByDisplayName()` 성공 조건 반전 (LINE 53)
- ⚠️ nullptr 체크 추가: `OnSelectionChanged()`
- ✅ 초기 맵 선택 로직 구현
- ✅ UpdateButton 제거 (불필요한 로직)

---

#### 5. Network Layer 구축

**SteamSessionSubsystem (UGameInstanceSubsystem)**:
- `Initialize()` / `Deinitialize()` 구현
- `CreateSession()` 구현:
  - FOnlineSessionSettings 설정
  - bUsesPresence, bAllowJoinViaPresence (친구 기반 세션)
  - Delegate 바인딩
- `sessionSearch` TSharedPtr 멤버 변수 (생명주기 관리)

**미완성 부분 (향후 작업)**:
- `OnCompleteCreateSession()`: ServerTravel 주석 처리됨
- `FindSession()`: 미구현
- `JoinSession()`: 미구현
- `OnCompleteFindSession()`: 미구현

---

#### 6. SessionsList Widget (기본 구조만)

**SessionsList (UUserWidget)**:
- ListView 컨테이너 (sessionsListContainer)
- Join/Cancel 버튼
- ⚠️ cpp 파일 거의 비어있음 (향후 구현 필요)

---

### 학습 내용

1. **UDeveloperSettings 패키징 주의사항**:
   - `Config=Game` 사용 (DefaultGame.ini)
   - TSoftObjectPtr 쿠킹 검증 필요
   - Asset Manager 설정 권장

2. **ListView 사용법**:
   - UObject 기반 데이터 클래스 필요
   - IUserObjectListEntry 인터페이스 구현
   - `NativeOnListItemObjectSet()` 자동 호출

3. **OnlineSubsystem 개념**:
   - `IOnlineSubsystem::Get()` Deprecation (대안 없음, 사용 불가피)
   - `IOnlineIdentityInterface::GetPlayerNickname()` - Steam 닉네임 가져오기
   - Login 상태 확인 필요

4. **TSoftObjectPtr vs ConstructorHelpers**:
   - TSoftObjectPtr: 유연, 에디터 설정 가능
   - ConstructorHelpers: 하드코딩, 재컴파일 필요

---

### 구현된 파일 목록

**Data**:
- `/Data/MapInfo.h`
- `/Data/PDA_MapList.h`, `.cpp`

**Network**:
- `/Network/SteamSessionSettings.h`, `.cpp`
- `/Network/SteamSessionSubsystem.h`, `.cpp` (일부 미구현)

**UI**:
- `/UI/SessionNodeData.h`
- `/UI/SessionNode.h`, `.cpp`
- `/UI/CreatingSession.h`, `.cpp`
- `/UI/SessionsList.h`, `.cpp` (미구현)

---

### 미완성 작업 (다음 세션 작업)

1. **SteamSessionSubsystem 완성**:
   - `OnCompleteCreateSession()`: PDA_MapList로 ServerTravel 구현
   - `FindSession()`: SEARCH_PRESENCE 기반 검색
   - `JoinSession()`: 선택된 세션 참가
   - `OnCompleteFindSession()`: SessionsList UI 업데이트

2. **SessionsList 구현**:
   - NativeConstruct() 구현
   - FindSession 결과를 ListView에 채우기
   - Join 버튼 클릭 → JoinSession() 호출
   - Cancel 버튼 클릭 → 위젯 닫기

3. **통합 테스트**:
   - CreatingSession → CreateSession → ServerTravel
   - FindSession → SessionsList → JoinSession → ClientTravel

---

### Commit 정보

feat: Implement Steam session architecture with DataAsset

1. "데이터 계층 구축"
    - MapInfo USTRUCT 및 PDA_MapList PrimaryDataAsset 생성
    - 4개 헬퍼 함수로 맵 정보 검색 기능 제공
2. "설정 계층 구축"
    - UDeveloperSettings 기반 SteamSessionSettings 생성
    - Project Settings UI에 노출
    - DeveloperSettings 모듈 의존성 추가
3. "UI 계층 구축"
    - SessionNodeData, SessionNode (ListView Entry)
    - CreatingSession (세션 생성 UI, 맵 선택/플레이어 수 조정)
    - Steam 닉네임 자동 표시
4. "네트워크 계층 기초"
    - SteamSessionSubsystem (GameInstanceSubsystem)
    - CreateSession 기본 구현
    - FindSession/JoinSession 스켈레톤 코드

---

## End Log
- 20251003 23:30
- 작업자 : VaVamVa

### 오늘 한 일

1. **Steam 세션 시스템 아키텍처 재설계**
   - UI/Network/Data 계층 분리 (책임 분리 원칙)
   - UGameInstanceSubsystem + UDeveloperSettings + UPrimaryDataAsset 구조 설계

2. **Data Layer 완성**
   - MapInfo USTRUCT, PDA_MapList PrimaryDataAsset
   - 동적 맵 경로 검색 시스템 구축

3. **UI Layer 구축**
   - SessionNodeData, SessionNode (ListView Entry)
   - CreatingSession (세션 생성 UI, 맵 선택/플레이어 수 자동 조정)
   - Steam 닉네임 표시, 코드 검증 및 버그 수정

4. **Network Layer 기초 구축**
   - SteamSessionSubsystem 생성
   - CreateSession 구현, FindSession/JoinSession 스켈레톤

5. **학습**
   - UDeveloperSettings 패키징, ListView 사용법
   - OnlineSubsystem API, TSoftObjectPtr 활용법

