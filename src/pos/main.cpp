#include <iostream>
#include <thread>
#include "posnet.h"

// int main() {
//     cout << "POS system starting..." << std::endl;

//     Posnet pos;

//     cout << "POS system running..." << std::endl;
//     thread (pos.connectToKiosk()).detach();

//     //TODP : 포스 메인 UI

//     return 0;
    
#include <QApplication>
#include "posnet.h"  // 키오스크와 통신하는 네트워크 매니저    
#include <thread> 

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // POS 네트워크 객체 생성
    Posnet pos;

    // 네트워크 스레드 시작 (키오스크 연결 담당)
    std::thread t1([&pos]() { pos.startNetworkServer(); }); // ✅ 정상 실행

    t1.join();

    return app.exec();  // Qt UI 실행
}

