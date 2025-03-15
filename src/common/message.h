#pragma once

#include <string>
#include <nlohmann/json.hpp>
using namespace std;
using namespace nlohmann;

class Message {

public:
    Message();
    ~Message();
    Message(string messageType, string sessionID, json data);
 
    void setCardNumber(string CardNumber){
        this->CardNumber = CardNumber;
    }
    void setPhoneNumber(string PhoneNumber){
        this->PhoneNumber = PhoneNumber;
    }
    void setUsePoint(string UsePoint){
        this->UsePoint = UsePoint;
    }
    void setWaitingNumber(string WaitingNumber){
        this->WaitingNumber = WaitingNumber;
    }
    void setPaymentType(string PaymentType){
        this->PaymentType = PaymentType;
    }
    void setIsTakeout(string IsTakeout){
        this->IsTakeout = IsTakeout;
    }
    void setIsConnected(string IsConnected){
        this->IsConnected = IsConnected;
    }
    void setSavePoint(string SavePoint){
        this->SavePoint = SavePoint;
    }
    
    string getMessageType(){
        return MessageType;
    };
    string getSessionID(){
        return SessionID;
    };
    json getData(){
        return Data;
    };
    string getPhoneNumber(){
        return PhoneNumber;
    }
    string getUsePoint(){
        return UsePoint;
    }
    string getWaitingNumber(){
        return WaitingNumber;
    }
    string getPaymentType(){
        return PaymentType;
    }
    string getIsTakeout(){
        return IsTakeout;
    }
    string getIsConnected(){
        return IsConnected;
    }
    string getSavePoint(){
        return SavePoint;
    }
    string getIsValid(){
        return IsValid;
    }
    string getCardNumber(){
        return CardNumber;
    }

    // Kiosk
    string createOrderMessage();
    string createConnectionMessage();
    string createPointSaveMessage();
    string createPointUseMessage();
    string createPaymentMessage(); 
    string createEntryUpdateMessage();
    
    // Pos
    string createOrderMessageResponse(string IsValid);
    string createConnectionMessageResponse(string SessionId);
    string createPointSaveMessageResponse(string IsValid);
    string createPointUseMessageResponse(string IsValid);
    string createPaymentMessageResponse(string IsValid, json OrderList, string WaitingNumber);
    string createEntryUpdateMessageResponse(json EntryList);
    

    static string jsonToString(json data);
    static json stringToJson(string data);

private:
    string MessageType;
    string SessionID;
    string CardNumber;
    string PhoneNumber;
    string UsePoint;
    string WaitingNumber;
    string PaymentType;
    string IsTakeout;
    string IsConnected;
    string SavePoint;
    string IsValid;

    json Data;
};