#include "message.h"

Message::Message()
{

}

Message::~Message()
{
    delete this;
}

Message::Message(string messageType, string sessionID, json data)
{
    MessageType = messageType;
    SessionID = sessionID;
    Data = data;
}

Message Message::createOrderMessage()
{
    return Message();
}

Message Message::createConnectionMessage()
{
    return Message();
}

Message Message::createPointSaveMessage()
{
    return Message();
}

Message Message::createPointUseMessage()
{
    return Message();
}

Message Message::createPaymentMessage()
{
    return Message();
}

string Message::jsonToString(json data)
{
    return string();
}

json Message::stringToJson(string data)
{
    return json();
}
