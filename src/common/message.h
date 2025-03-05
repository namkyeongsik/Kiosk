#include <string>
#include <nlohmann/json.hpp>
using namespace std;
using namespace nlohmann;

class Message {

public:
    Message();
    ~Message();
    Message(string messageType, string sessionID, json data);
    string getMessageType();
    string getSessionID();
    json getData();

    Message createOrderMessage();
    Message createConnectionMessage();
    Message createPointSaveMessage();
    Message createPointUseMessage();
    Message createPaymentMessage(); 

    string jsonToString(json data);
    json stringToJson(string data);

private:
    string MessageType;
    string SessionID;
    json Data;
};