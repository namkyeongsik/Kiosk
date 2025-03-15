#include "message.h"

class KioskNet {
private:
    Message msg;
    int clientSocket;
    string SessionID;
public:
    KioskNet();
    ~KioskNet();
    void connectToPOS();
    void sendMessage(string msg);
    Message MessageParse(string receivedMessage);
};
