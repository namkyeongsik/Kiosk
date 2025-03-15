#include "kiosknet.h"
#include "message.h"
#include <iostream>
#include <sstream>
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

#define BUFFER_SIZE 1024

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

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
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
    sendMessage(msg.createConnectionMessage());


    // while (true) {
    //     std::string message;
    //     std::cout << "키오스크 메시지 입력: ";
    //     std::getline(std::cin, message);

    //     if (message == "exit") break;

    //     send(clientSocket, message.c_str(), message.length(), 0);

    //     char buffer[1024];
    //     memset(buffer, 0, sizeof(buffer));
    //     recv(clientSocket, buffer, sizeof(buffer), 0);
    //     std::cout << "POS 응답: " << buffer << std::endl;
    // }

// #ifdef _WIN32
//     closesocket(clientSocket);
//     WSACleanup();
// #else
//     close(clientSocket);
// #endif
}

void KioskNet::sendMessage(string msg) {

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    if(clientSocket == -1) {
        std::cerr << "POS 서버와 연결되지 않았습니다." << std::endl;
        return;
    }

    ssize_t sendbyte = send(clientSocket, msg.c_str(), msg.length(), 0);

    if(sendbyte == -1) {
        std::cerr << "메시지 전송 실패" << std::endl;
        return;
    }
    else{
        std::cout << "메시지 전송 성공" << std::endl;
        recv(clientSocket, buffer, sizeof(buffer), 0);

        std::cout << "POS 응답: " << buffer << std::endl;
    }
}

Message KioskNet::MessageParse(string receivedMessage){
    
    std::vector<string> tokens;
    std::stringstream ss(receivedMessage);
    std::string token;

    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    if(tokens.size() < 1) {
        std::cerr << "잘못된 메시지 형식!" << std::endl;
        return Message();
    }

    if(tokens[0] == "CONNECT"){
        return Message("CONNECT", tokens[1], json{});
    }
    else if(tokens[0] == "POINT_SAVE"){
        if(tokens.size() < 4){
            std::cerr << "잘못된 메시지 형식!" << std::endl;
            return Message();
        }
        return Message("POINT_SAVE", tokens[1], json{});
    }
    else if(tokens[0] == "POINT_USE"){
        if(tokens.size() < 4){
            std::cerr << "잘못된 메시지 형식!" << std::endl;
            return Message();
        }
        return Message("POINT_USE", tokens[1], json{});
    }
    else if(tokens[0] == "ORDER"){
        return Message("ORDER", tokens[1], json{});
    }
    else if(tokens[0] == "PAYMENT"){
        if(tokens.size() < 5){
            std::cerr << "잘못된 메시지 형식!" << std::endl;
            return Message();
        }

        return Message("PAYMENT", tokens[1], Message::stringToJson(tokens[2]));
        }
    else if(tokens[0] == "ENTRY_UPDATE"){
        return Message("ENTRY_UPDATE", tokens[1], json{});
    }
    else{
        std::cerr << "알 수 없는 메시지 타입" << std::endl;
        return Message();
    }

}