#include "mfa_window.h"
#include "qr_code_dialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QStackedWidget>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QPixmap>
#include <QRegularExpressionValidator>
#include <QGroupBox>

MFAWindow::MFAWindow(QWidget *parent)
    : QMainWindow(parent)
    , networkManager(std::make_unique<QNetworkAccessManager>(this))
    , qrDialog(nullptr)
{
    setupUI();
}

MFAWindow::~MFAWindow()
{
    if (qrDialog) {
        delete qrDialog;
    }
}

void MFAWindow::setupUI()
{
    // 중앙 위젯 설정
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // 스택 위젯 (페이지 전환용)
    stackedWidget = new QStackedWidget(this);
    
    // 로그인과 회원가입 페이지 추가
    stackedWidget->addWidget(createLoginWidget());
    stackedWidget->addWidget(createRegisterWidget());
    
    mainLayout->addWidget(stackedWidget);
    
    // 윈도우 설정
    setWindowTitle("MFA 인증 시스템");
    setMinimumSize(400, 350);
    resize(450, 400);
    
    // 초기 페이지는 로그인
    showLoginPage();
}

QWidget* MFAWindow::createLoginWidget()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 30, 30, 30);
    
    // 제목
    QLabel *titleLabel = new QLabel("MFA 로그인");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 10px;");
    layout->addWidget(titleLabel);
    
    // 입력 필드들을 그룹박스로 묶기
    QGroupBox *inputGroup = new QGroupBox("로그인 정보");
    QVBoxLayout *inputLayout = new QVBoxLayout(inputGroup);
    inputLayout->setSpacing(10);
    
    // 사용자 ID 입력
    QLabel *idLabel = new QLabel("사용자 ID:");
    loginIdEdit = new QLineEdit();
    loginIdEdit->setPlaceholderText("사용자 ID를 입력하세요");
    loginIdEdit->setMinimumHeight(30);
    inputLayout->addWidget(idLabel);
    inputLayout->addWidget(loginIdEdit);
    
    // OTP 코드 입력 (6자리 숫자만)
    QLabel *otpLabel = new QLabel("인증 코드 (6자리):");
    loginOtpEdit = new QLineEdit();
    loginOtpEdit->setPlaceholderText("123456");
    loginOtpEdit->setMaxLength(6);
    loginOtpEdit->setMinimumHeight(30);
    
    // 숫자만 입력 가능하도록 설정
    QRegularExpressionValidator *validator = new QRegularExpressionValidator(
        QRegularExpression("^[0-9]{0,6}$"), loginOtpEdit);
    loginOtpEdit->setValidator(validator);
    
    // OTP 입력 변경 시 버튼 활성화/비활성화
    connect(loginOtpEdit, &QLineEdit::textChanged, this, &MFAWindow::onOtpTextChanged);
    
    inputLayout->addWidget(otpLabel);
    inputLayout->addWidget(loginOtpEdit);
    
    layout->addWidget(inputGroup);
    
    layout->addSpacing(10);
    
    // 버튼들
    loginButton = new QPushButton("로그인");
    loginButton->setEnabled(false);  // 초기에는 비활성화
    loginButton->setMinimumHeight(40);
    loginButton->setStyleSheet("QPushButton { font-size: 16px; }");
    connect(loginButton, &QPushButton::clicked, this, &MFAWindow::onLoginClicked);
    layout->addWidget(loginButton);
    
    QPushButton *switchButton = new QPushButton("계정이 없으신가요? 회원가입");
    switchButton->setStyleSheet("QPushButton { border: none; color: #0066cc; text-decoration: underline; }");
    connect(switchButton, &QPushButton::clicked, this, &MFAWindow::showRegisterPage);
    layout->addWidget(switchButton);
    
    layout->addStretch();
    
    return widget;
}

QWidget* MFAWindow::createRegisterWidget()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setSpacing(15);
    layout->setContentsMargins(30, 30, 30, 30);
    
    // 제목
    QLabel *titleLabel = new QLabel("계정 등록");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; margin-bottom: 10px;");
    layout->addWidget(titleLabel);
    
    // 사용자 ID 입력
    QGroupBox *inputGroup = new QGroupBox("등록 정보");
    QVBoxLayout *inputLayout = new QVBoxLayout(inputGroup);
    
    QLabel *idLabel = new QLabel("사용자 ID:");
    registerIdEdit = new QLineEdit();
    registerIdEdit->setPlaceholderText("원하는 ID를 입력하세요");
    registerIdEdit->setMinimumHeight(30);
    inputLayout->addWidget(idLabel);
    inputLayout->addWidget(registerIdEdit);
    
    layout->addWidget(inputGroup);
    
    layout->addSpacing(10);
    
    // 등록 버튼
    registerButton = new QPushButton("등록하기");
    registerButton->setMinimumHeight(40);
    registerButton->setStyleSheet("QPushButton { font-size: 16px; }");
    connect(registerButton, &QPushButton::clicked, this, &MFAWindow::onRegisterClicked);
    layout->addWidget(registerButton);
    
    // 안내 메시지
    QLabel *infoLabel = new QLabel("등록 후 Google Authenticator QR 코드가 표시됩니다.");
    infoLabel->setAlignment(Qt::AlignCenter);
    infoLabel->setWordWrap(true);
    infoLabel->setStyleSheet("color: #666; margin-top: 10px;");
    layout->addWidget(infoLabel);
    
    // 로그인 페이지로 돌아가기
    QPushButton *switchButton = new QPushButton("이미 계정이 있으신가요? 로그인");
    switchButton->setStyleSheet("QPushButton { border: none; color: #0066cc; text-decoration: underline; }");
    connect(switchButton, &QPushButton::clicked, this, &MFAWindow::showLoginPage);
    layout->addWidget(switchButton);
    
    layout->addStretch();
    
    return widget;
}

void MFAWindow::showLoginPage()
{
    stackedWidget->setCurrentIndex(0);
}

void MFAWindow::showRegisterPage()
{
    stackedWidget->setCurrentIndex(1);
}

void MFAWindow::onOtpTextChanged(const QString &text)
{
    // 6자리 숫자가 입력되었을 때만 로그인 버튼 활성화
    loginButton->setEnabled(text.length() == 6 && !loginIdEdit->text().isEmpty());
}

void MFAWindow::onLoginClicked()
{
    QString userId = loginIdEdit->text().trimmed();
    QString otpCode = loginOtpEdit->text();
    
    if (userId.isEmpty()) {
        showMessage("오류", "사용자 ID를 입력해주세요.");
        return;
    }
    
    if (otpCode.length() != 6) {
        showMessage("오류", "인증 코드는 6자리여야 합니다.");
        return;
    }
    
    sendLoginRequest(userId, otpCode);
}

void MFAWindow::onRegisterClicked()
{
    QString userId = registerIdEdit->text().trimmed();
    
    if (userId.isEmpty()) {
        showMessage("오류", "사용자 ID를 입력해주세요.");
        return;
    }
    
    sendRegisterRequest(userId);
}

void MFAWindow::sendLoginRequest(const QString &userId, const QString &otpCode)
{
    QNetworkRequest request(QUrl(QString("%1/api/authenticate").arg(SERVER_URL)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject json;
    json["user_id"] = userId;
    json["otp_code"] = otpCode;
    
    QJsonDocument doc(json);
    QNetworkReply *reply = networkManager->post(request, doc.toJson());
    
    connect(reply, &QNetworkReply::finished, this, &MFAWindow::onLoginResponse);
    
    loginButton->setEnabled(false);
    loginButton->setText("인증 중...");
}

void MFAWindow::sendRegisterRequest(const QString &userId)
{
    QNetworkRequest request(QUrl(QString("%1/api/register").arg(SERVER_URL)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    QJsonObject json;
    json["user_id"] = userId;
    
    QJsonDocument doc(json);
    QNetworkReply *reply = networkManager->post(request, doc.toJson());
    
    connect(reply, &QNetworkReply::finished, this, &MFAWindow::onRegisterResponse);
    
    registerButton->setEnabled(false);
    registerButton->setText("등록 중...");
}

void MFAWindow::onLoginResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    loginButton->setEnabled(true);
    loginButton->setText("로그인");
    
    if (reply->error() != QNetworkReply::NoError) {
        showMessage("로그인 실패", "서버 연결에 실패했습니다: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    QByteArray response = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonObject obj = doc.object();
    
    bool success = obj["success"].toBool();
    QString message = obj["message"].toString();
    
    if (success) {
        showMessage("로그인 성공", "인증이 완료되었습니다!");
        loginOtpEdit->clear();
    } else {
        showMessage("로그인 실패", message.isEmpty() ? "인증에 실패했습니다." : message);
    }
    
    reply->deleteLater();
}

void MFAWindow::onRegisterResponse()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    registerButton->setEnabled(true);
    registerButton->setText("등록하기");
    
    if (reply->error() != QNetworkReply::NoError) {
        showMessage("등록 실패", "서버 연결에 실패했습니다: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    QByteArray response = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(response);
    QJsonObject obj = doc.object();
    
    bool success = obj["success"].toBool();
    
    if (success) {
        QString userId = obj["user_id"].toString();
        QString secret = obj["secret"].toString();
        QString qrUrl = obj["qr_code_url"].toString();
        
        // QR 코드 다운로드
        if (!qrUrl.isEmpty() && !secret.isEmpty()) {
            downloadQRCode(qrUrl, userId, secret);
        } else {
            showMessage("등록 실패", "QR 코드 정보를 받아올 수 없습니다.");
        }
    } else {
        QString message = obj["message"].toString();
        showMessage("등록 실패", message.isEmpty() ? "등록에 실패했습니다." : message);
    }
    
    reply->deleteLater();
}

void MFAWindow::downloadQRCode(const QString &url, const QString &userId, const QString &secret)
{
    // 임시 저장
    currentUserId = userId;
    currentSecret = secret;
    
    QUrl qrUrl(url);
    QNetworkRequest request(qrUrl);
    QNetworkReply *reply = networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, this, &MFAWindow::onQRCodeDownloaded);
}

void MFAWindow::onQRCodeDownloaded()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    if (reply->error() != QNetworkReply::NoError) {
        showMessage("오류", "QR 코드를 다운로드할 수 없습니다: " + reply->errorString());
        reply->deleteLater();
        return;
    }
    
    QByteArray imageData = reply->readAll();
    QPixmap pixmap;
    
    if (pixmap.loadFromData(imageData)) {
        // QR 코드 다이얼로그 생성 및 표시
        if (qrDialog) {
            delete qrDialog;
        }
        
        qrDialog = new QRCodeDialog(currentUserId, currentSecret, this);
        qrDialog->setQRCodeImage(pixmap);
        
        if (qrDialog->exec() == QDialog::Accepted) {
            showMessage("등록 완료", "계정이 성공적으로 등록되었습니다.\n"
                                   "이제 Google Authenticator 앱의 6자리 코드로 로그인할 수 있습니다.");
        }
        
        delete qrDialog;
        qrDialog = nullptr;
    } else {
        showMessage("오류", "QR 코드 이미지를 표시할 수 없습니다.");
    }
    
    reply->deleteLater();
}

void MFAWindow::showMessage(const QString &title, const QString &message)
{
    QMessageBox::information(this, title, message);
}