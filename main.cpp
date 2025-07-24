#include <QApplication>
#include "mfa_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 애플리케이션 정보 설정
    app.setApplicationName("MFA Client");
    app.setOrganizationName("MyCompany");
    
    MFAWindow window;
    window.show();
    
    return app.exec();
}