#pragma once

#include "message.h"
#include <iostream>

Message::Message()
{
    MessageType = "";
    SessionID = "";
    CardNumber = "";
    PhoneNumber = "";
    UsePoint = "";
    WaitingNumber = "";
    PaymentType = "";
    IsTakeout = "";
    IsConnected = "";
    SavePoint = "";
    IsValid = "";
    Data = json();
}

Message::~Message() {
    // 동적으로 할당한 메모리 해제 필요 없음.
}

Message::Message(std::string messageType, std::string sessionID, json data)
{
    MessageType = messageType;
    SessionID = sessionID;
    Data = data;
}

std::string Message::createOrderMessage()
{
    this->MessageType = "ORDER";
    this->IsTakeout = "";
    this->Data = json(); // TODO: 주문 목록 데이터 추가

    std::string jsonmsg = Message::jsonToString(this->Data);
    return this->MessageType + "|" + this->SessionID + "|" + this->IsTakeout + "|" + jsonmsg;
}

std::string Message::createConnectionMessage()
{
    this->MessageType = "CONNECT";
    return this->MessageType;
}

std::string Message::createPointSaveMessage()
{
    this->MessageType = "POINT_SAVE";
    this->PhoneNumber = "";
    this->SavePoint = "";

    return this->MessageType + "|" + this->SessionID + "|" + this->PhoneNumber + "|" + this->SavePoint;
}

std::string Message::createPointUseMessage()
{
    this->MessageType = "POINT_USE";
    this->PhoneNumber = "";
    this->UsePoint = "";

    return this->MessageType + "|" + this->SessionID + "|" + this->PhoneNumber + "|" + this->UsePoint;
}

std::string Message::createPaymentMessage()
{
    this->MessageType = "PAYMENT";
    this->PaymentType = "";
    this->CardNumber = "";

    return this->MessageType + "|" + this->SessionID + "|" + this->PaymentType + "|" + this->CardNumber;
}

std::string Message::createEntryUpdateMessage()
{
    this->MessageType = "ENTRY_UPDATE";
    return this->MessageType + "|" + this->SessionID;
}

std::string Message::createOrderMessageResponse(std::string isValid)
{
    this->MessageType = "ORDER";
    this->IsValid = isValid;

    return this->MessageType + "|" + this->IsValid;
}

std::string Message::createConnectionMessageResponse(std::string sessionId)
{
    this->MessageType = "CONNECT";
    this->SessionID = sessionId;

    return this->MessageType + "|" + this->SessionID;
}

std::string Message::createPointSaveMessageResponse(std::string isValid)
{
    this->MessageType = "POINT_SAVE";
    this->IsValid = isValid;

    return this->MessageType + "|" + this->IsValid;
}

std::string Message::createPointUseMessageResponse(std::string isValid)
{
    this->MessageType = "POINT_USE";
    this->IsValid = isValid;

    return this->MessageType + "|" + this->IsValid;
}

std::string Message::createPaymentMessageResponse(std::string isValid, json orderList, std::string waitingNumber)
{
    this->MessageType = "PAYMENT";
    this->IsValid = isValid;
    this->Data = orderList;
    this->WaitingNumber = waitingNumber;

    std::string jsonmsg = Message::jsonToString(this->Data);
    return this->MessageType + "|" + this->IsValid + "|" + jsonmsg + "|" + this->WaitingNumber;
}

std::string Message::createEntryUpdateMessageResponse(json entryList)
{
    this->MessageType = "ENTRY_UPDATE";
    this->Data = entryList;

    std::string jsonmsg = Message::jsonToString(this->Data);
    return this->MessageType + "|" + jsonmsg;
}

// ✅ JSON 변환 함수 (static 멤버 함수)
std::string Message::jsonToString(json data)
{
    return data.dump();
}

json Message::stringToJson(std::string data)
{
    try {
        return json::parse(data);
    } catch (json::parse_error& e) {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return json{}; // 빈 JSON 객체 반환
    }
}
