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
    
    #include <QApplication>
    #include "pos_ui.h"  // POS UI 헤더파일
    #include "posnet.h"  // 키오스크와 통신하는 네트워크 매니저
    
    #include <thread>
    
    int main(int argc, char *argv[]) {
        QApplication app(argc, argv);
    
        // // POS UI 실행
        // POS_UI mainWindow;
        // mainWindow.show();
    
        // 네트워크 스레드 시작 (키오스크 연결 담당)
        std::thread networkThread(startNetworkServer);
        networkThread.detach();  // 스레드 분리 실행
    
        return app.exec();  // Qt UI 실행
    }
