QT += core gui widgets network

CONFIG += c++17

# Qt6를 사용하도록 명시
greaterThan(QT_MAJOR_VERSION, 5): QT += core gui widgets network

TARGET = MFAClient
TEMPLATE = app

SOURCES += \
    main.cpp \
    mfa_window.cpp

HEADERS += \
    mfa_window.h

# Windows에서 콘솔 창 숨기기
win32 {
    CONFIG += windows
}

# macOS 설정
macx {
    QMAKE_INFO_PLIST = Info.plist
}