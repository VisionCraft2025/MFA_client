# MFA Qt6 클라이언트

Google Authenticator와 연동되는 MFA(Multi-Factor Authentication) 서버를 위한 Qt6 기반 데스크톱 클라이언트입니다.

## 기능

### 1. 로그인
- 사용자 ID 입력
- Google Authenticator에서 생성된 6자리 인증 코드 입력
- 숫자만 입력 가능 (자동 필터링)
- 6자리 입력 완료 시 자동으로 로그인 버튼 활성화

### 2. 계정 등록
- 원하는 사용자 ID 입력
- 서버에서 QR 코드 자동 다운로드 및 표시
- QR 코드는 메모리에만 저장 (보안)
- 시크릿 키 표시 (수동 입력용)
- 프로그램 종료 시 QR 코드 자동 삭제

## 빌드 요구사항

- C++17 이상
- Qt6 (Core, Widgets, Network 모듈)
- CMake 3.16 이상 (CMake 빌드 시)

## 빌드 방법

### 방법 1: CMake 사용

```bash
mkdir build
cd build
cmake ..
make  # Linux/macOS
# 또는
cmake --build .  # Windows
```

### 방법 2: Qt Creator 사용

1. Qt Creator에서 `MFAClient.pro` 파일 열기
2. 빌드 버튼 클릭 또는 Ctrl+B

### 방법 3: qmake 직접 사용

```bash
qmake MFAClient.pro
make  # Linux/macOS
# 또는
nmake  # Windows (Visual Studio)
# 또는
mingw32-make  # Windows (MinGW)
```

## 사용 방법

### 로그인
1. 프로그램 실행
2. 사용자 ID 입력
3. Google Authenticator 앱에서 6자리 코드 확인
4. 인증 코드 입력 (자동으로 숫자만 입력됨)
5. 로그인 버튼 클릭

### 계정 등록
1. "계정이 없으신가요? 회원가입" 클릭
2. 원하는 사용자 ID 입력
3. "등록하기" 버튼 클릭
4. 표시된 QR 코드를 Google Authenticator 앱으로 스캔
5. 또는 시크릿 키를 수동으로 입력

## 보안 특징

- QR 코드는 파일로 저장되지 않고 메모리에만 유지
- 프로그램 종료 시 QR 코드 자동 삭제
- 페이지 전환 시 QR 코드 자동 삭제
- HTTPS 통신 지원 (서버 설정에 따라)

## 프로젝트 구조

```
MFAClient/
├── main.cpp          # 프로그램 진입점
├── mfa_window.h      # 메인 윈도우 헤더
├── mfa_window.cpp    # 메인 윈도우 구현
├── CMakeLists.txt    # CMake 빌드 설정
├── MFAClient.pro     # Qt Creator 프로젝트 파일
└── README.md         # 이 파일
```

## 서버 설정

기본 서버 주소: `http://auth.kwon.pics:8443`

서버 주소를 변경하려면 `mfa_window.h`의 `SERVER_URL` 상수를 수정하세요:

```cpp
static constexpr const char* SERVER_URL = "http://your-server:port";
```

## 문제 해결

### Qt6를 찾을 수 없음
Qt6가 설치되어 있는지 확인하고, 환경 변수가 올바르게 설정되어 있는지 확인하세요:
```bash
export CMAKE_PREFIX_PATH=/path/to/Qt/6.x.x/gcc_64  # Linux
# 또는
set CMAKE_PREFIX_PATH=C:\Qt\6.x.x\msvc2019_64  # Windows
```

### 네트워크 연결 실패
- 서버가 실행 중인지 확인
- 방화벽 설정 확인
- 서버 주소가 올바른지 확인

### QR 코드가 표시되지 않음
- 네트워크 연결 상태 확인
- 서버의 QR 코드 생성 서비스 확인