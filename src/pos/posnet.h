#ifndef POSNET_H
#define POSNET_H

#include <vector>
#include <thread>
#include <mutex>
#include <message.h>

#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#endif


class Posnet {
public:
    Posnet();
    ~Posnet();
    void connectToKiosk();
    void startNetworkServer(); // POS 네트워크 서버 실행 함수
    void handleClient(int clientSocket);
    void sendMessage(Message msg);
};

#endif // POSNET_H
