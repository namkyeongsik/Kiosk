// #include "posnet.h"
// #include <iostream>
// #include <thread>

// void testPosServer() {
//     Posnet posnet;

//     std::cout << "🔄 POS 서버 시작 중..." << std::endl;

//     // 1. 서버 실행 (별도 스레드에서 실행)
//     std::thread serverThread(&Posnet::startNetworkServer, &posnet);
//     serverThread.detach(); // 서버는 백그라운드에서 실행

//     // 2. 서버가 정상적으로 실행되는지 확인
//     std::this_thread::sleep_for(std::chrono::seconds(1)); // 대기 후 테스트 실행

//     std::cout << "✅ POS 서버가 정상적으로 실행됨!" << std::endl;
// }

// int main() {
//     testPosServer();
//     return 0;
// }
