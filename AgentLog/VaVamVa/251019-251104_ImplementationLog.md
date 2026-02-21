# TTTK 251019 ~ 251104
- VaVamVa 구현 내역 종합 로그 (회고)

# Start Log
- 20260209 (회고 작성일)
- 작업자 : VaVamVa
- 대상 기간 : 20251019 ~ 20251104
- 총 커밋 수 : 약 35개 (Merge 포함)

---

## 1. 제기차기 (Jegi) 게임 시스템 (251019 ~ 251027)

### 관련 커밋
| 날짜 | 커밋 | 내용 |
|------|------|------|
| 251020 05:05 | `9ba9719` | Feat: 제기차기 기본 시스템 구현 |
| 251027 14:36 | `83c0891` | gegi assets setting |
| 251027 12:19 | `c2e2ce4` | Content Entry, Content Manager, Content Component(Player) 구현 |

### 구현 내역
- **Jegi Actor** (`Jegi.h/.cpp`)
  - `ProjectileMovementComponent` 기반 물리 시뮬레이션
  - `SphereCollider` (플레이어 감지) + `MeshComponent` (땅 충돌) 역할 분리
  - 멀티플레이 Replication 설정 완료
  - `NetMulticast_OnKicked` 로 모든 클라이언트 동기화
  - 높이 기반 `EKickTiming` 판정 로직

- **JegiChagiContentComponent** (`JegiChagiContentComponent.h/.cpp`) - 신규
  - 578줄 규모의 제기차기 전용 콘텐츠 컴포넌트
  - 좌/우 킥 InputAction 바인딩 (`IA_Kick_L`, `IA_Kick_R`)
  - `IMC_JegiChagi` 입력 매핑 컨텍스트

- **JegiChagiContentManager** (`JegiChagiContentManager.h/.cpp`) - 신규
  - 330줄 규모의 제기차기 전용 콘텐츠 매니저
  - 서버 권한 기반 게임 규칙 관리

- **BaseContentComponent** (`BaseContentComponent.h/.cpp`) - 신규
  - 콘텐츠 컴포넌트 기반 클래스
  - 콘텐츠별 공통 인터페이스 정의

- **BaseContentManager** 확장 (`BaseContentManager.h/.cpp`)
  - 171 → 290줄로 확장 (Timer, 게임 시간 관리 등)
  - `HasAuthority` + `Multicast` 방식 서버 권한 구조

- **에셋**: Jegi 3D 모델, 스켈레탈 메시, 물리 에셋, 킥 애니메이션 몽타주 추가

---

## 2. Steam 네트워크 시스템 (251020 ~ 251022)

### 관련 커밋
| 날짜 | 커밋 | 내용 |
|------|------|------|
| 251020 09:18 | `f94fc2b` | while Steam Socket |
| 251020 14:24 | `a129f08` | Steam Socket 구현 완료 |
| 251020 15:48 | `3acca70` | debug steam socket |
| 251020 16:10 | `4181859` | Hotfix |
| 251020 16:15 | `103061a` | hotfix 2 |
| 251022 20:09 | `78dde9f` | Please finish steam!!!! |
| 251022 22:44 | `00c0ba0` | Steam 모듈 완료 / UI는 아쉽 |

### 구현 내역
- **SteamSessionSubsystem** (`SteamSessionSubsystem.h/.cpp`)
  - Steam Socket 기반 네트워크 연결 구현
  - 세션 생성/검색/참가/종료 전체 플로우 완성
  - `ServerTravel` 디버그 및 안정화
  - `TransitionLoading.umap` 전환 로딩 맵 추가
  - `DefaultEngine.ini` 네트워크 설정 추가

- **MainMenu UI** (`MainMenuSteam.h/.cpp`)
  - `WBP_CreatingSession` 세션 생성 위젯 개선
  - `WBP_SessionNode` 세션 노드 위젯 개선
  - `WBP_SessionsList` / `WBP_SessionsEntry` 세션 목록 UI

---

## 3. 채팅 시스템 (251023 ~ 251027)

### 관련 커밋
| 날짜 | 커밋 | 내용 |
|------|------|------|
| 251023 22:07 | `d2c3e96` | chat delegate |
| 251027 15:54 | `f5494f9` | chat |

### 구현 내역
- **ChatWidget** (`ChatWidget.h/.cpp`) - 신규
  - 인게임 채팅 위젯 (44줄+)
  - `WBP_Chat.uasset` Blueprint 연동

- **ChatLineWidget** (`ChatLineWidget.h/.cpp`) - 신규
  - 채팅 메시지 개별 라인 위젯
  - `WBP_ChatLine.uasset` Blueprint 연동

- **IMC_UI** 입력 매핑 추가
  - `IA_Chat` (채팅 열기)
  - `IA_InContentMenu` (인게임 메뉴)
  - `IA_Leaderboard` (리더보드)

- **TTTK_PlayerState** 확장
  - 채팅 관련 플레이어 상태 데이터 추가

- **TTTK_GameState** 확장
  - 채팅 메시지 브로드캐스트 로직 추가

---

## 4. Dynamic Key Mapping 시스템 (251027)

### 관련 커밋
| 날짜 | 커밋 | 내용 |
|------|------|------|
| 251027 05:22 | `e0be9dd` | Dynamic Key Mapping, With Automatic DeveloperSettings |

### 구현 내역
- **InputMappingsSettings** (`InputMappingsSettings.h/.cpp`)
  - `DeveloperSettings` 기반 자동 키 매핑 설정
  - `DefaultTTTK.ini` 커스텀 설정 파일 추가
  - `IMC_Interaction` 입력 매핑 컨텍스트 추가
  - `IA_Interaction` InputAction 추가

---

## 5. Interaction 시스템 리팩토링 (251024 ~ 251027)

### 관련 커밋
| 날짜 | 커밋 | 내용 |
|------|------|------|
| 251024 18:06 | `3ddb33d` | Interaction 시스템 리팩토링 및 UI 개선 |
| 251027 14:49 | `ccbdab6` | input mode |
| 251027 15:30 | `0e97648` | 정류장 |
| 251027 19:45 | `5822c1d` | Refactor: Interaction 시스템 리팩토링 - Custom Outline & Event-driven 전환 |

### 구현 내역
- **InteractableComponent** 대폭 리팩토링 (`InteractableComponent.h/.cpp`)
  - Custom Outline 렌더링 시스템 추가
  - Event-driven 방식으로 전환 (Tick 의존도 제거)
  - `InteractableFeedbackSettings` 개선

- **HeritageObjectComponent** 경량화 (`HeritageObjectComponent.h/.cpp`)
  - 222줄 → 경량화 (공통 로직을 InteractableComponent로 이관)

- **InteractionComponent** 개선 (`InteractionComponent.h/.cpp`)
  - Input Mode 전환 로직 개선
  - 정류장(CarriageStopPoint) 연동

- **ContentEntryComponent** 리팩토링 (`ContentEntryComponent.h/.cpp`)
  - 181줄 → 리팩토링으로 구조 개선
  - `EntryInfoWidget` UI 추가 (`WBP_EntryInfo.uasset`)

- **PlayerWidgetComponent** 확장 (`PlayerWidgetComponent.h/.cpp`)
  - 81줄+ 추가 (채팅, 설정 등 위젯 관리)

- **Actor Virtual LifeCycle 문서** (`UE_ActorVirtualLifeCycle.md`)
  - 705줄 규모의 UE Actor 생명주기 학습 문서 작성

---

## 6. Settings UI 시스템 (251028 ~ 251030)

### 관련 커밋
| 날짜 | 커밋 | 내용 |
|------|------|------|
| 251028 23:02 | `d0c4d62` | Settings UI 기본 구조 (탭 전환 시스템) |
| 251030 23:13 | `d272ee8` | .UI |

### 구현 내역
- **SettingWidget** (`Settings/SettingWidget.h/.cpp`) - 신규
  - 탭 전환 기반 설정 UI 시스템
  - 4개 탭 위젯 관리

- **GraphicSettingWidget** (`Settings/GraphicSettingWidget.h/.cpp`) - 신규
  - 그래픽 설정 (해상도, 프레임레이트 등)
  - Toggle/Slider UI 컴포넌트

- **ControllerSettingWidget** (`Settings/ControllerSettingWidget.h/.cpp`) - 신규
  - 컨트롤러/키 설정

- **NetworkSettingWidget** (`Settings/NetworkSettingWidget.h/.cpp`) - 신규
  - 네트워크 관련 설정

- **InfoSettingWidget** (`Settings/InfoSettingWidget.h/.cpp`) - 신규
  - 정보/기타 설정

- **TTTKUserSettings** (`TTTKUserSettings.h/.cpp`)
  - `SettingsData.h` 구조체 기반 사용자 설정 데이터 관리

- **UI 에셋 다수 추가**
  - 설정 배경, 아이콘, Toggle/Slider 이미지 등

- **UI 폴더 구조 리팩토링**
  - `UI/` → `UI/Network/`, `UI/Settings/` 서브 디렉토리로 재구성
  - `CreatingSession`, `MainMenu`, `MainMenuSteam`, `SessionNode`, `SessionsEntry` → Network 폴더로 이동

---

## 7. 인게임 메뉴 및 MainMenu 시스템 (251030 ~ 251103)

### 관련 커밋
| 날짜 | 커밋 | 내용 |
|------|------|------|
| 251103 12:14 | `fc5c5e4` | beta!!!!!!!!!!!!!!!!  |
| 251103 13:33 | `67b5113` | 1. menuwidget |
| 251103 14:04 | `6073779` | hotfix. level |
| 251103 14:38 | `d25d281` | hotfix. ingame menu |

### 구현 내역
- **InContentMenuWidget** (`InContentMenuWidget.h/.cpp`) - 신규
  - 인게임 메뉴 위젯 (`WBP_Menu.uasset`)
  - 게임 내 일시정지/설정/나가기 기능

- **PlayerWidget** (`PlayerWidget.h/.cpp`) - 신규
  - 인게임 플레이어 HUD 위젯 (`WBP_Player.uasset`)

- **LoadingWidget** (`LoadingWidget.h/.cpp`) - 신규
  - 로딩 화면 위젯 (`WBP_Loading.uasset`)

- **MainMenu 시스템 리팩토링**
  - `MainMenuController` / `MainMenuGameMode` / `MainMenuPawn` 신규 생성
  - MainMenu 전용 GameFramework 분리

- **SteamSessionSubsystem** 대폭 확장 (242줄+)
  - SessionNode UI 개선 (75줄 → 리팩토링)
  - SessionsEntry 개선

- **TTTKGameMode** 확장
  - 게임 모드 로직 추가 (24줄+)

---

## 8. 유틸리티 및 기타 (251028 ~ 251104)

### 관련 커밋
| 날짜 | 커밋 | 내용 |
|------|------|------|
| 251028 18:07 | `c11c99a` | 유물 해결 |
| 251104 16:26 | `d8b93ee` | Base64 Converter |
| 251104 17:44 | `07d69a9` | debug for bind widget |

### 구현 내역
- **Base64Converter** (`Base64Converter.h/.cpp`) - 신규
  - Base64 인코딩/디코딩 유틸리티 클래스

- **WidgetBlueprintLoader** (`WidgetBlueprintLoader.h/.cpp`) - 신규
  - 위젯 블루프린트 동적 로딩 유틸리티

- **유물 시스템 버그 수정**
  - Heritage 관련 이슈 해결

- **PlayerWidgetComponent 바인딩 디버그**
  - 위젯 바인딩 관련 버그 수정

---

## 구현 통계 요약

### 신규 생성 파일 (C++ 기준)
| 카테고리 | 파일 | 비고 |
|----------|------|------|
| Content | `BaseContentComponent.h/.cpp` | 콘텐츠 기반 클래스 |
| Content | `JegiChagiContentComponent.h/.cpp` | 제기차기 컴포넌트 |
| Content | `JegiChagiContentManager.h/.cpp` | 제기차기 매니저 |
| UI | `ChatWidget.h/.cpp` | 채팅 위젯 |
| UI | `ChatLineWidget.h/.cpp` | 채팅 라인 위젯 |
| UI | `PlayerWidget.h/.cpp` | 플레이어 HUD |
| UI | `InContentMenuWidget.h/.cpp` | 인게임 메뉴 |
| UI | `LoadingWidget.h/.cpp` | 로딩 화면 |
| UI/Settings | `SettingWidget.h/.cpp` | 설정 메인 |
| UI/Settings | `GraphicSettingWidget.h/.cpp` | 그래픽 설정 |
| UI/Settings | `ControllerSettingWidget.h/.cpp` | 컨트롤러 설정 |
| UI/Settings | `NetworkSettingWidget.h/.cpp` | 네트워크 설정 |
| UI/Settings | `InfoSettingWidget.h/.cpp` | 정보 설정 |
| MainMenu | `MainMenuController.h/.cpp` | 메인메뉴 컨트롤러 |
| MainMenu | `MainMenuGameMode.h/.cpp` | 메인메뉴 게임모드 |
| MainMenu | `MainMenuPawn.h/.cpp` | 메인메뉴 폰 |
| Data | `SettingsData.h` | 설정 데이터 구조체 |
| Utility | `Base64Converter.h/.cpp` | Base64 유틸리티 |
| Utility | `WidgetBlueprintLoader.h/.cpp` | 위젯 로더 |

### 주요 리팩토링 파일
- `InteractableComponent` - Custom Outline + Event-driven 전환
- `HeritageObjectComponent` - 경량화
- `ContentEntryComponent` - 구조 개선
- `SteamSessionSubsystem` - 대폭 확장
- `BaseContentManager` - Timer/게임 시간 관리 추가
- `Jegi` - Content 폴더로 이동 + 리팩토링
- UI 폴더 구조 - `Network/`, `Settings/` 서브 디렉토리 분리

### 주요 아키텍처 결정
1. **Component 기반 콘텐츠 시스템**: `BaseContentComponent` → 각 게임별 Component로 확장
2. **Event-driven Interaction**: Tick 의존도 제거, 이벤트 기반 상호작용
3. **탭 기반 Settings UI**: 4개 탭(Graphic/Controller/Network/Info)으로 분리
4. **MainMenu 전용 GameFramework**: 메인 메뉴만의 Controller/GameMode/Pawn 분리
5. **DeveloperSettings 기반 Dynamic Key Mapping**: `DefaultTTTK.ini`로 커스텀 설정
