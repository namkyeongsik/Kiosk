// #include "kiosknet.h"
// #include <iostream>
// #include <thread>
// #include <chrono>

// void testKiosk() {
//     KioskNet kioskNet;

//     // 1. 서버와 연결 시도
//     kioskNet.connectToPOS();

//     std::cout << "✅ 키오스크가 POS 서버에 연결됨!" << std::endl;

//     // 2. 메시지 전송 테스트
//     kioskNet.sendMessage("TEST_MESSAGE");
//     std::this_thread::sleep_for(std::chrono::seconds(2)); // 서버 응답 대기

//     std::cout << "✅ 키오스크 테스트 완료!" << std::endl;
// }

// int main() {
//     testKiosk();
//     return 0;
// }
