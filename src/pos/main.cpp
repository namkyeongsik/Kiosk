#include <iostream>
#include <QApplication>
#include "posnet.h"  // 키오스크와 통신하는 네트워크 매니저    
#include <thread>
#include "poswindow.h"  // 키오스크 UI

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // POS 네트워크 객체 생성
    Posnet pos;

    // 네트워크 스레드 시작 (키오스크 연결 담당)
    std::thread t1([&pos]() { pos.startNetworkServer(); }); // ✅ 정상 실행
    t1.detach();  // 스레드 분리 실행

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

    return app.exec();  // Qt UI 실행
}
