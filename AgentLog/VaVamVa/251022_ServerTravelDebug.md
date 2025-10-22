# TTTK 251022 - ServerTravel 에러 디버깅

# Start Log
- 20251022 12:37
- 작업자 : VaVamVa

## 작업 목표
Steam P2P ServerTravel 시 리슨 소켓 중복 생성 에러 해결

### To Do

1. **ServerTravel 에러 원인 파악**
   - "Already have a listen socket on P2P vport 17777" 에러 분석
   - NetGameInstance를 통한 세션 생성 플로우 확인
   - NewMap1 → NewMap ServerTravel 테스트 중

2. **환경 설정 확인**
   - DefaultEngine.ini NetDriver 설정 비교
   - 다른 프로젝트와 차이점 분석
   - 에디터 Play 설정 검증

---

## Compact Log 1
- 20251022 12:37

### 요약 내용

**ServerTravel 리슨 소켓 중복 생성 에러 해결**

1. **문제 증상**
   - NetGameInstance를 통한 세션 생성 후 ServerTravel 시 에러 발생
   - `LogSteamSocketsAPI: Error: SteamSockets API: Error Cannot create listen socket. Already have a listen socket on P2P vport 17777`
   - NewMap1 → NewMap 테스트 중 재현
   - 세션 생성은 한 번만 발생, ServerTravel이 실패

2. **진단 과정**
   - NetGameInstance.cpp 코드 확인
   - DefaultEngine.ini NetDriver 설정 분석 (SteamSocketsNetDriver 사용 확인)
   - 다른 프로젝트와 비교 (동일 설정임에도 다른 프로젝트는 정상 동작)
   - 세션 생성 플로우 검토 (CreateSession → OnCreateSessionComplete → ServerTravel(?listen))

3. **원인 파악**
   - **에디터 Play 설정이 "Play As Listen Server"로 되어 있었음**
   - Standalone 실행 시 NewMap1이 이미 리슨 서버로 시작됨
   - P2P vport 17777에 소켓이 이미 생성된 상태
   - 이 상태에서 CreateSession → ServerTravel(?listen) 호출 시 새로운 소켓 생성 시도
   - Steam P2P 소켓은 한 프로세스당 하나만 가능, 중복 생성 실패

4. **해결 방법**
   - 에디터 Play 설정을 "Standalone" 또는 "Play As Client"로 변경
   - Standalone 실행 → 일반 클라이언트로 시작 → 소켓 없음
   - CreateSession → ServerTravel(?listen) → 7777 포트에 소켓 생성 성공
   - Output Log 확인:
     ```
     LogNet: SteamSocketsNetDriver_0 started listening on 7777
     LogWorld: Bringing World /Game/VaVamVa/NewMap.NewMap up for play
     LogLoad: Took 0.208990 seconds to LoadMap(/Game/VaVamVa/NewMap)
     ```

5. **핵심 교훈**
   - Steam P2P 소켓은 한 프로세스당 하나만 허용
   - 세션 생성 테스트 시: Standalone(서버 아님) 시작 → CreateSession으로 서버 만들기
   - 맵 전환 테스트 시: Listen Server 시작 → CreateSession 없이 바로 ServerTravel
   - Listen Server + CreateSession = 소켓 중복 충돌

### Commit 정보

**Fix: ServerTravel 리슨 소켓 중복 생성 에러 해결**

1. **문제 해결**
   - 에디터 Play 설정이 Listen Server로 되어 있어 발생한 문제 파악
   - Steam P2P 소켓 중복 생성 방지

2. **테스트 환경 개선**
   - 세션 생성 테스트는 Standalone 모드 사용
   - 올바른 ServerTravel 플로우 확인

🤖 Generated with [Claude Code](https://claude.com/claude-code)

Co-Authored-By: Claude <noreply@anthropic.com>

---
