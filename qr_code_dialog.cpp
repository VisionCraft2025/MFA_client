#include "qr_code_dialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QFont>
#include <QResizeEvent>

QRCodeDialog::QRCodeDialog(const QString &userId, const QString &secret, QWidget *parent)
    : QDialog(parent)
{
    setupUI(userId, secret);
}

QRCodeDialog::~QRCodeDialog()
{
}

void QRCodeDialog::setupUI(const QString &userId, const QString &secret)
{
    setWindowTitle("Google Authenticator QR 코드");
    setModal(true);
    setMinimumSize(400, 500);
    resize(450, 550);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(15);
    layout->setContentsMargins(20, 20, 20, 20);
    
    // 사용자 ID 표시
    QLabel *userLabel = new QLabel(QString("사용자: %1").arg(userId));
    userLabel->setAlignment(Qt::AlignCenter);
    QFont userFont = userLabel->font();
    userFont.setPointSize(12);
    userLabel->setFont(userFont);
    layout->addWidget(userLabel);
    
    // 시크릿 키 표시
    QLabel *secretTitleLabel = new QLabel("시크릿 키:");
    secretTitleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = secretTitleLabel->font();
    titleFont.setBold(true);
    secretTitleLabel->setFont(titleFont);
    layout->addWidget(secretTitleLabel);
    
    QLabel *secretLabel = new QLabel(secret);
    secretLabel->setAlignment(Qt::AlignCenter);
    secretLabel->setWordWrap(true);
    secretLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    secretLabel->setStyleSheet("QLabel { "
                              "font-family: monospace; "
                              "font-size: 11pt; "
                              "background-color: #f0f0f0; "
                              "padding: 10px; "
                              "border: 1px solid #ccc; "
                              "border-radius: 4px; "
                              "}");
    layout->addWidget(secretLabel);
    
    // QR 코드 표시 영역
    qrCodeLabel = new QLabel("QR 코드 로딩 중...");
    qrCodeLabel->setAlignment(Qt::AlignCenter);
    qrCodeLabel->setMinimumSize(300, 300);
    qrCodeLabel->setStyleSheet("QLabel { "
                              "border: 2px solid #333; "
                              "background-color: white; "
                              "padding: 20px; "
                              "}");
    layout->addWidget(qrCodeLabel, 1, Qt::AlignCenter);  // stretch factor 1로 설정
    
    // 경고 메시지
    QLabel *warningLabel = new QLabel("⚠️ 경고: 이 QR 코드와 시크릿 키를 절대 타인에게 노출하지 마세요!");
    warningLabel->setAlignment(Qt::AlignCenter);
    warningLabel->setWordWrap(true);
    warningLabel->setStyleSheet("QLabel { "
                               "color: #d32f2f; "
                               "font-weight: bold; "
                               "margin-top: 10px; "
                               "}");
    layout->addWidget(warningLabel);
    
    // 닫기 버튼
    QPushButton *closeButton = new QPushButton("확인");
    closeButton->setDefault(true);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    layout->addWidget(closeButton, 0, Qt::AlignCenter);
}

void QRCodeDialog::setQRCodeImage(const QPixmap &pixmap)
{
    if (!pixmap.isNull()) {
        originalQRCode = pixmap;
        
        // QR 코드에 여백을 추가하여 스캔이 잘 되도록 함
        int margin = 20;
        int size = qMin(pixmap.width(), pixmap.height());
        
        // 여백이 포함된 새 이미지 생성
        QPixmap paddedPixmap(size + 2 * margin, size + 2 * margin);
        paddedPixmap.fill(Qt::white);
        
        // 중앙에 QR 코드 그리기
        QPainter painter(&paddedPixmap);
        painter.drawPixmap(margin, margin, pixmap);
        painter.end();
        
        // 라벨 크기에 맞게 조정 (가로세로 비율 유지)
        int labelWidth = qrCodeLabel->width() - 40;  // 패딩 고려
        int labelHeight = qrCodeLabel->height() - 40;
        int targetSize = qMin(labelWidth, labelHeight);
        targetSize = qMax(targetSize, 250);  // 최소 250px
        
        QPixmap scaledPixmap = paddedPixmap.scaled(
            targetSize, targetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        qrCodeLabel->setPixmap(scaledPixmap);
    } else {
        qrCodeLabel->setText("QR 코드 표시 실패");
    }
}

void QRCodeDialog::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    
    // 창 크기가 변경되면 QR 코드 크기도 재조정
    if (!originalQRCode.isNull()) {
        setQRCodeImage(originalQRCode);
    }
}