# TTTK 251019 
- 제기차기 게임 기본 시스템 구현

# Start Log
- 20251019 16:16
- 작업자 : VaVamVa

### To Do

1. "현재 프로젝트 상태 파악" :
   - VaVamVa 브랜치의 수정된 파일들 확인
   - 최근 인터랙션 시스템 및 멀티플레이 구현 현황 점검

2. "제기차기(Jegi) 게임 로직 완성" :
   - Server_OnKicked_Implementation 함수의 빈 switch 케이스들 구현
   - 킥 타이밍별 제기 물리 움직임 및 점수 시스템 적용

3. "멀티플레이 동기화 검증" :
   - 제기차기 게임의 클라이언트-서버 동기화 테스트
   - Replication 설정 점검 및 최적화

## Compact Log 1
- 20251020 04:37

### 요약 내용

제기차기(Jegi) 게임 구현을 진행했습니다:

1. **Jegi Actor 기본 구조 완성**
   - ProjectileMovementComponent를 이용한 물리 시뮬레이션 구현
   - SphereCollider(플레이어 감지)와 MeshComponent(땅 충돌) 역할 분리
   - 멀티플레이 Replication 설정 완료

2. **킥 타이밍 시스템 구현**
   - EKickTiming enum 기반 타이밍별 다른 힘 적용
   - NetMulticast_OnKicked으로 모든 클라이언트 동기화
   - 높이별 킥 타이밍 판정 로직 구현

3. **카메라 시스템 추가**
   - InteractableComponent를 통한 제기차기 시작
   - 임시 카메라로 전환하여 플레이 뷰 제공
   - 게임 종료시 원래 카메라로 복원

4. **충돌 감지 시스템 설계**
   - Mesh의 Bounds를 이용한 자동 Collider 크기 계산
   - Cylinder/Sphere 메시 대응 FindCylinderMeshRadius 함수
   - WorldStatic과의 Block 충돌로 땅 감지

5. **Content Manager 아키텍처 논의**
   - UBaseContentManager : UObject 기반 설계
   - HasAuthority + Multicast 방식의 서버 권한 관리
   - Timer 기반 게임 시간 관리 시스템

### 현재 해결 중인 이슈

- **Jegi가 바닥을 뚫고 지나가는 문제**: ProjectileMovementComponent의 CCD 설정 및 속도 조정 필요

### Commit 정보

"Feat: 제기차기 기본 시스템 구현 완료"

1. "Jegi Actor 물리 시뮬레이션 시스템"
   - ProjectileMovementComponent 기반 제기 움직임 구현
   - 멀티플레이 Replication 설정

2. "킥 타이밍 및 점수 시스템"
   - 높이 기반 킥 타이밍 판정 로직
   - 타이밍별 차등 힘 적용 시스템

3. "카메라 및 상호작용 시스템"
   - InteractableComponent 연동
   - 임시 카메라 전환 시스템

4. "Content Manager 아키텍처 설계"
   - UObject 기반 게임 규칙 관리 시스템
   - Timer 기반 게임 시간 관리

## End Log
- 20251020 04:38
- 작업자 : VaVamVa

### 오늘 한 일

1. "제기차기 Actor 기본 구조 완성"
   - ProjectileMovementComponent 기반 물리 시뮬레이션 시스템 구현
   - 멀티플레이 Replication 및 NetMulticast 함수 설정

2. "킥 타이밍 및 게임 로직 구현"
   - 높이 기반 킥 타이밍 판정 시스템
   - EKickTiming별 차등 힘 적용 로직

3. "상호작용 및 카메라 시스템"
   - InteractableComponent 연동한 게임 시작 시스템
   - 임시 카메라 전환 및 복원 기능

4. "Content Manager 아키텍처 설계"
   - UBaseContentManager 기반 게임 규칙 관리 시스템 설계
   - HasAuthority + Multicast 방식의 서버 권한 구조

5. "충돌 감지 시스템 설계"
   - Mesh Bounds 기반 자동 Collider 크기 계산
   - WorldStatic 충돌 감지 로직 (현재 디버깅 중)