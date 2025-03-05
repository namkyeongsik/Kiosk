#include "posnet.h"
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif

std::vector<std::thread> clientThreads;
std::mutex clientMutex;

void Posnet::handleClient(int clientSocket) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cout << "키오스크 연결 종료" << std::endl;
            break;
        }

        std::cout << "키오스크 요청: " << buffer << std::endl;

        // 키오스크에게 응답 전송
        std::string response = "POS 응답: " + std::string(buffer);
        send(clientSocket, response.c_str(), response.length(), 0);
    }

#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}

void Posnet::startNetworkServer() {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "POS 서버 소켓 생성 실패" << std::endl;
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8081);  // 키오스크와 통신하는 포트
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "바인딩 실패" << std::endl;
        return;
    }
    
    if (listen(serverSocket, 5) == -1) {
        std::cerr << "리슨 실패" << std::endl;
        return;
    }

    std::cout << "POS 서버 대기 중 (포트: 8081)..." << std::endl;

    while (true) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket == -1) {
            std::cerr << "키오스크 연결 실패" << std::endl;
            continue;
        }

        std::cout << "새 키오스크 연결됨!" << std::endl;

        std::lock_guard<std::mutex> lock(clientMutex);
        clientThreads.emplace_back(handleClient, clientSocket);
        clientThreads.back().detach();
    }

#ifdef _WIN32
    WSACleanup();
#endif
}
