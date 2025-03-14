// #include <iostream>
// #include <thread>
// #include "posnet.h"
// using namespace std;

// int main() {
//     cout << "POS system starting..." << std::endl;

//     Posnet pos;

//     cout << "POS system running..." << std::endl;
//     thread (pos.connectToKiosk()).detach();

//     //TODP : 포스 메인 UI

//     return 0;

//#include "pos_ui.h"  // POS UI 헤더파일
#include "posnet.h"  // 키오스크와 통신하는 네트워크 매니저
#include "poswindow.h"
#include <QApplication>
#include <QPalette>
#include <thread>

int main(int argc, char *argv[])
{
    std::thread networkThread(startNetworkServer);
    networkThread.detach();  // 스레드 분리 실행



    QApplication app(argc, argv);

    QPalette palette;
    palette.setColor(QPalette::Window, Qt::white);
    palette.setColor(QPalette::WindowText, Qt::black);
    app.setPalette(palette);

    app.setStyleSheet(R"(
        QPushButton {
            background-color: #036B3F;
            color: white;
            font-size: 18px;
            border-radius: 8px;
            padding: 10px;
        }
        QPushButton:hover {
            background-color: #028C50;
        }
        QLabel {
            font-size: 18px;
            color: black;
        }
        QDialog {
            background-color: white;
            border-radius: 10px;
            padding: 15px;
        }
    )");
    POSWindow posWindow;
    posWindow.show();
    return app.exec();
}
