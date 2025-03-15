#include "posnet.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <mutex>
#include <thread>

#ifdef _WIN32
    #include <winsock2.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif


Posnet::Posnet() {
    std::cout << "Posnet 객체 생성" << std::endl;
    clientSessionSocketMap = {};
}

Posnet::~Posnet() {
    std::cout << "Posnet 객체 소멸" << std::endl;
}

string Posnet::createSessionID() {
    return std::to_string(rand() % 1000) + "-" +
           std::to_string(rand() % 1000) + "-" +
           std::to_string(rand() % 1000);
}

Message Posnet::MessageParse(string receivedMessage)
{
    // 요청 파싱
    std::stringstream ss(receivedMessage);
    std::string token;
    std::vector<std::string> tokens;

    std::cout << "받은 메시지: " << receivedMessage << std::endl;  // 🔍 메시지 확인


    while (std::getline(ss, token, '|')) {
        tokens.push_back(token);
    }

    // 최소한 메시지 타입과 세션 ID가 있어야 함
    if (tokens.size() < 1) {
        std::cerr << "잘못된 메시지 형식!" << std::endl;
        return Message();
    }
    
    if(tokens[0] == "CONNECT")
    {
        return Message("CONNECT", "", json{});
    }
    else if(tokens[0] == "POINT_SAVE")
    {
        if(tokens.size() < 4)
        {
            std::cerr << "잘못된 메시지 형식!" << std::endl;
            return Message();
        }
        return Message("POINT_SAVE", tokens[1], json{{"PhoneNumber", tokens[2]}, {"SavePoint", tokens[3]}});
    }
    else if(tokens[0] == "POINT_USE")
    {
        if(tokens.size() < 4)
        {
            std::cerr << "잘못된 메시지 형식!" << std::endl;
            return Message();
        }
        return Message("POINT_USE", tokens[1], json{{"PhoneNumber", tokens[2]}, {"UsePoint", tokens[3]}});
    }
    else if(tokens[0] == "PAYMENT")
    {
        if(tokens.size() < 4)
        {
            std::cerr << "잘못된 메시지 형식!" << std::endl;
            return Message();
        }
        return Message("PAYMENT", tokens[1], json{{"PaymentType", tokens[2]}, {"CardNumber", tokens[3]}});
    }
    else if(tokens[0] == "ENTRY_UPDATE")
    {
        if(tokens.size() < 2)
        {
            std::cerr << "잘못된 메시지 형식!" << std::endl;
            return Message();
        }
        return Message("ENTRY_UPDATE", tokens[1], json{});
    }
    else
    {
        std::cerr << "알 수 없는 메시지 타입!" << std::endl;
        return Message();
    }

    return Message();
}

void Posnet::handleClient(string sessionID, int clientSocket) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cout << "키오스크 연결 종료" << std::endl;
            break;
        }

        std::string receivedMessage(buffer);
        std::cout << "키오스크 요청: " << receivedMessage << std::endl;

        // 요청 파싱
        Message recvmsg = this->MessageParse(receivedMessage);
        std::cout << "메시지 타입: " << recvmsg.getMessageType() << std::endl;
        // 요청 처리
        if(recvmsg.getMessageType() == "CONNECT")
        {  
            std::cout << "키오스크 연결 요청에 대한 응답" << std::endl;
            std::string response = recvmsg.createConnectionMessageResponse(sessionID);
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if(recvmsg.getMessageType() == "POINT_SAVE")
        {
            // 포인트 적립 요청에 대한 응답
            Message msg("POINT_SAVE", "", json{});
            std::cout << "포인트 적립 요청에 대한 응답" << std::endl;
            std::string response = msg.createPointSaveMessageResponse("true");
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if(recvmsg.getMessageType() == "POINT_USE")
        {
            // 포인트 사용 요청에 대한 응답
            Message msg("POINT_USE", "", json{});
            std::cout << "포인트 사용 요청에 대한 응답" << std::endl;
            std::string response = msg.createPointUseMessageResponse("true");
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if(recvmsg.getMessageType() == "PAYMENT")
        {
            // 결제 요청에 대한 응답
            Message msg("PAYMENT", "", json{});
            std::cout << "결제 요청에 대한 응답" << std::endl;
            std::string response = msg.createPaymentMessageResponse("true", json{}, "1");
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if(recvmsg.getMessageType() == "ENTRY_UPDATE")
        {
            // 입장 정보 업데이트 요청에 대한 응답
            Message msg("ENTRY_UPDATE", "", json{});
            std::cout << "입장 정보 업데이트 요청에 대한 응답" << std::endl;
            std::string response = msg.createEntryUpdateMessageResponse(json{});
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else
        {
            std::cerr << "알 수 없는 메시지 타입!" << std::endl;
        }

        // std::string messageType = tokens[0];
        
        //     // JSON 응답을 문자열로 변환하여 전송
        //     std::string response = responseJson.dump();
        //     send(clientSocket, response.c_str(), response.length(), 0);
        }
    
#ifdef _WIN32
    closesocket(clientSocket);
#else
    close(clientSocket);
#endif
}

void Posnet::startNetworkServer() {

    std::cout << "네트워크 서버 시작..." << std::endl;
    // 네트워크 서버 로직 추가
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
        string sessionID = createSessionID();
        this->clientSessionSocketMap.insert(std::pair<std::string, int>(sessionID, clientSocket));
        std::cout << "세션 ID: " << sessionID << std::endl;

        std::thread t2([this, sessionID, clientSocket]() { 
            this->handleClient(sessionID, clientSocket); 
        });
        
    }

#ifdef _WIN32
    WSACleanup();
#endif
}


