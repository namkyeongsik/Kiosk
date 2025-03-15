#ifndef POSNET_H
#define POSNET_H

#include <vector>
#include <mutex>
#include "message.h"

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif


class Posnet {
private:
    Message msg;
    Message recvmsg;
    int serverSocket;
    map <string, int> clientSessionSocketMap;

public:
    Posnet();
    ~Posnet();
    string createSessionID();
    void startNetworkServer(); // POS 네트워크 서버 실행 함수
    Message MessageParse(string receivedMessage);
    void handleClient(string sessionID, int clientSocket);
    void sendMessage(string message);
};

#endif // POSNET_H
