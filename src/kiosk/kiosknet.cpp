#include "kiosknet.h"
#include <iostream>
#include <cstring>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>

#endif

KioskNet::KioskNet() {
    std::cout << "KioskNet 객체 생성" << std::endl;
}

KioskNet::~KioskNet() {
    std::cout << "KioskNet 객체 소멸" << std::endl;
}


void KioskNet::connectToPOS() {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        std::cerr << "소켓 생성 실패" << std::endl;
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8081);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // POS 서버

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "POS 연결 실패" << std::endl;
        return;
    }

    std::cout << "POS 연결됨!" << std::endl;

    while (true) {
        std::string message;
        std::cout << "키오스크 메시지 입력: ";
        std::getline(std::cin, message);

        if (message == "exit") break;

        send(clientSocket, message.c_str(), message.length(), 0);

        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        recv(clientSocket, buffer, sizeof(buffer), 0);
        std::cout << "POS 응답: " << buffer << std::endl;
    }

#ifdef _WIN32
    closesocket(clientSocket);
    WSACleanup();
#else
    close(clientSocket);
#endif
}
