#include "message.h"

class KioskNet {
public:
    KioskNet();
    ~KioskNet();
    void connectToPOS();
    void sendMessage(Message msg);
};
