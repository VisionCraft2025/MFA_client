#ifndef QR_CODE_DIALOG_H
#define QR_CODE_DIALOG_H

#include <QDialog>
#include <memory>

QT_BEGIN_NAMESPACE
class QLabel;
class QPixmap;
QT_END_NAMESPACE

class QRCodeDialog : public QDialog
{
    Q_OBJECT

public:
    QRCodeDialog(const QString &userId, const QString &secret, QWidget *parent = nullptr);
    ~QRCodeDialog();
    
    void setQRCodeImage(const QPixmap &pixmap);

private:
    void setupUI(const QString &userId, const QString &secret);
    
    QLabel *qrCodeLabel;
};

#endif // QR_CODE_DIALOG_H