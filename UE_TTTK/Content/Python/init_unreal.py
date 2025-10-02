"""
Claude Code Auto Logger - UE Python Integration

UE Editor 시작 시 자동으로 Claude Code 로그 세션을 Git Bash 별도 창에서 시작합니다.
"""

import unreal
import subprocess
import os
import sys

# 프로젝트 경로 설정
PROJECT_ROOT = os.path.dirname(os.path.dirname(os.path.dirname(unreal.Paths.get_project_file_path())))
ASSISTANT_DIR = os.path.join(PROJECT_ROOT, "Assistant")
LOCK_FILE = os.path.join(ASSISTANT_DIR, ".claude-log-session.lock")
LOG_MD = os.path.join(ASSISTANT_DIR, "CLAUDE-Log.md")


def log_info(message):
    #"""콘솔에 INFO 로그 출력"""
    unreal.log(f"[Claude Logger] {message}")


def log_warning(message):
    #"""콘솔에 WARNING 로그 출력"""
    unreal.log_warning(f"[Claude Logger] {message}")


def log_error(message):
    #"""콘솔에 ERROR 로그 출력"""
    unreal.log_error(f"[Claude Logger] {message}")


def check_duplicate_session():
    #"""중복 세션 체크"""
    if not os.path.exists(LOCK_FILE):
        return True

    try:
        with open(LOCK_FILE, 'r') as f:
            pid = f.read().strip()

        # Windows에서 프로세스 존재 여부 확인
        result = subprocess.run(
            ['tasklist', '/FI', f'PID eq {pid}'],
            capture_output=True,
            text=True
        )

        if pid in result.stdout:
            log_warning(f"Claude Log 세션이 이미 실행 중입니다. (PID: {pid})")
            return False
        else:
            log_info("이전 세션이 종료되었습니다. Lock 파일 제거 중...")
            os.remove(LOCK_FILE)
            return True
    except Exception as e:
        log_error(f"Lock 파일 확인 중 오류: {e}")
        return True


def get_git_user_info():
    """Git 사용자 정보 가져오기"""
    try:
        name_result = subprocess.run(
            ['git', 'config', 'user.name'],
            capture_output=True,
            text=True,
            cwd=PROJECT_ROOT
        )
        email_result = subprocess.run(
            ['git', 'config', 'user.email'],
            capture_output=True,
            text=True,
            cwd=PROJECT_ROOT
        )

        name = name_result.stdout.strip() or "Unknown User"
        email = email_result.stdout.strip() or "unknown@email.com"

        return name, email
    except Exception as e:
        log_warning(f"Git 사용자 정보를 가져올 수 없습니다: {e}")
        return "Unknown User", "unknown@email.com"


def start_claude_session():
    """Claude Code 로그 세션 시작"""
    log_info("Claude Code Log 세션을 시작합니다...")

    # Git 사용자 정보
    git_user, git_email = get_git_user_info()

    # 현재 시간
    from datetime import datetime
    current_time = datetime.now().strftime("%Y%m%d %H:%M")

    log_info(f"작업자: {git_user} ({git_email})")
    log_info(f"현재 시간: {current_time}")

    # PowerShell로 직접 Claude 실행 (새 창)
    try:
        # PowerShell 명령어 구성
        ps_command = f'''
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host "  Claude Code Auto Logger" -ForegroundColor Cyan
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "작업자: {git_user} ({git_email})" -ForegroundColor Green
Write-Host "현재 시간: {current_time}" -ForegroundColor Green
Write-Host ""
Write-Host "CLAUDE-Log.md의 'Agent 시작' 규칙에 따라" -ForegroundColor Yellow
Write-Host "로그 세션을 시작해주세요." -ForegroundColor Yellow
Write-Host "==========================================" -ForegroundColor Cyan
Write-Host ""

# 프로세스 ID를 Lock 파일에 저장
$PID | Out-File -FilePath "{LOCK_FILE}" -Encoding utf8

# Claude 실행
Set-Location "{PROJECT_ROOT}"
claude

# 종료 시 Lock 파일 제거
Remove-Item -Path "{LOCK_FILE}" -ErrorAction SilentlyContinue
Write-Host ""
Write-Host "[INFO] Claude Code 세션이 종료되었습니다." -ForegroundColor Green
Read-Host "Press Enter to close"
'''

        # PowerShell 새 창에서 실행
        DETACHED_PROCESS = 0x00000008
        process = subprocess.Popen(
            ['powershell', '-NoExit', '-Command', ps_command],
            creationflags=subprocess.CREATE_NEW_CONSOLE | DETACHED_PROCESS,
            cwd=PROJECT_ROOT,
            close_fds=True
        )

        log_info(f"Claude Logger가 PowerShell 새 창에서 시작되었습니다. (PID: {process.pid})")
    except Exception as e:
        log_error(f"Claude 세션 시작 실패: {e}")
        import traceback
        log_error(traceback.format_exc())


def on_editor_startup():
    #"""UE Editor 시작 시 호출되는 함수"""
    #log_info("=== Claude Code Auto Logger ===")
    log_info(f"프로젝트 루트: {PROJECT_ROOT}")

    ## 중복 세션 체크
    #if not check_duplicate_session():
    #    log_info("Claude Logger 시작을 건너뜁니다.")
    #    return

    ## Claude 세션 시작
    #start_claude_session()


# UE Editor 시작 시 자동 실행
if __name__ == "__main__":
    on_editor_startup()
