#ifndef MFA_WINDOW_H
#define MFA_WINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <memory>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QLabel;
class QStackedWidget;
class QPixmap;
QT_END_NAMESPACE

class QRCodeDialog;

class MFAWindow : public QMainWindow
{
    Q_OBJECT

public:
    MFAWindow(QWidget *parent = nullptr);
    ~MFAWindow();

private slots:
    // UI 전환
    void showLoginPage();
    void showRegisterPage();
    
    // 로그인 관련
    void onLoginClicked();
    void onLoginResponse();
    
    // 회원가입 관련
    void onRegisterClicked();
    void onRegisterResponse();
    
    // QR 코드 다운로드
    void onQRCodeDownloaded();
    
    // OTP 입력 검증
    void onOtpTextChanged(const QString &text);

private:
    // UI 생성 함수들
    void setupUI();
    QWidget* createLoginWidget();
    QWidget* createRegisterWidget();
    
    // 네트워크 요청
    void sendLoginRequest(const QString &userId, const QString &otpCode);
    void sendRegisterRequest(const QString &userId);
    void downloadQRCode(const QString &url, const QString &userId, const QString &secret);
    
    // 유틸리티
    void showMessage(const QString &title, const QString &message);
    
    // UI 컴포넌트
    QStackedWidget *stackedWidget;
    
    // 로그인 페이지
    QLineEdit *loginIdEdit;
    QLineEdit *loginOtpEdit;
    QPushButton *loginButton;
    
    // 회원가입 페이지
    QLineEdit *registerIdEdit;
    QPushButton *registerButton;
    
    // 네트워크
    std::unique_ptr<QNetworkAccessManager> networkManager;
    
    // QR 코드 다이얼로그
    QRCodeDialog *qrDialog;
    
    // 임시 저장용
    QString currentUserId;
    QString currentSecret;
    
    // 서버 URL
    static constexpr const char* SERVER_URL = "http://auth.kwon.pics:8443";
};

#endif // MFA_WINDOW_H