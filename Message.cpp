#include "Message.h"
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <iostream>
#include <bitset>
#include <cstddef>
#include <Arduino.h>

Message::Message()
: _length(0),
_type(DATA),
_timestamp(0) {
    std::cout << "Allocating ()...\n";
    _payload = (byte*) malloc(sizeof(byte));
}

Message::Message(byte* rawMessage) {
    // TODO: Check if this makes sense
    byte* lengthPtr = rawMessage;
    byte* timestampPtr = lengthPtr + sizeof(short);
    byte* typePtr = timestampPtr + sizeof(unsigned long);
    byte* senderPtr = typePtr + 1;
    byte* receiverPtr = senderPtr + sizeof(int);
    byte* payloadPtr = receiverPtr + sizeof(int);

    memcpy(&_length, lengthPtr, sizeof(short));
    memcpy(&_timestamp, timestampPtr, sizeof(unsigned long));

    memcpy(&_type, typePtr, 1);
    _type >> 8*(sizeof(MessageType) - 1);

    memcpy(&_sender, senderPtr, sizeof(int));
    memcpy(&_receiver, receiverPtr, sizeof(int));

    size_t payloadSize = _length - (sizeof(_timestamp) + 1 + sizeof(_sender) + sizeof(_receiver));
    _payload = (byte*) malloc(payloadSize);
    memcpy(_payload, payloadPtr, payloadSize);
}   

Message::Message(char* rawMessage) {
    // TODO: Split message into the different fields
    // 8 byte | 1 byte | 8 byte | 16 byte | 16 byte | 463 byte ?
    char* lengthChar = strtok(rawMessage, "#");
    char* typeChar = strtok(NULL, "#");
    char* timestampChar = strtok(NULL, "#");
    char* senderChar = strtok(NULL, "#");
    char* receiverChar = strtok(NULL, "#");
    char* payloadChar = strtok(NULL, "#");

    std::string lengthString(lengthChar);
    _length = (short) std::stoi(lengthChar);
    _type = (MessageType) std::stoi(typeChar);
    _timestamp = std::stol(timestampChar);
    _sender = std::stoi(senderChar);
    _receiver = std::stoi(receiverChar);

    // TODO: These memcpys are wrong
    /*memcpy(&_length, lengthChar, sizeof(short));
    memcpy(&_type, typeChar, sizeof(MessageType));
    memcpy(&_timestamp, timestampChar, sizeof(unsigned long));
    memcpy(&_sender, sender, strlen(sender) + 1);
    memcpy(&_receiver, receiver, strlen(receiver) + 1);*/

    std::cout << "Allocating (char* rawMessage)...\n";

    size_t payloadSize = _length - (sizeof(_timestamp) + 1 + sizeof(_sender) + sizeof(_receiver));
    _payload = (byte*) malloc(payloadSize);
    memcpy(_payload, payloadChar, payloadSize);
}

Message::Message(unsigned long timestamp, MessageType type, int sender, int receiver, byte* payload, short payloadLength) 
: _timestamp(timestamp),
_type(type),
_sender(sender),
_receiver(receiver) {

    std::cout << "Allocating (unsigned long timestamp, MessageType type, int sender, int receiver, byte* payload, short payloadLength)...\n";

    _length = payloadLength + sizeof(_timestamp) + 1 + sizeof(_sender) + sizeof(_receiver);

    Serial.print("Payload length: ");
    Serial.println(payloadLength);
    Serial.print("Timestamp length: ");
    Serial.println(sizeof(_timestamp));
    Serial.print("Sender length: ");
    Serial.println(sizeof(_sender));
    Serial.print("Receiver length: ");
    Serial.println(sizeof(_receiver));

    _payload = (byte*) malloc(payloadLength);
    memcpy(_payload, payload, payloadLength);
}

Message::Message(const Message &message)
:_length(message.getLength()),
_timestamp(message.getTimestamp()),
_type(message.getType()),
_sender(message.getSender()),
_receiver(message.getReceiver())
 {
    byte* payload = message.getPayload();

    std::cout << "Allocating (const Message &message)...\n";

    size_t payloadSize = _length - (sizeof(_timestamp) + 1 + sizeof(_sender) + sizeof(_receiver));
    _payload = (byte*) malloc(payloadSize);
    memcpy(_payload, payload, payloadSize);
}

Message::~Message() {
    // std::cout << "Freeing...\n";
    free(_payload);
}

Message& Message::operator=(const Message& message) {
    _length = message.getLength();
    _timestamp = message.getTimestamp();
    _sender = message.getSender();
    _receiver = message.getReceiver();
    _payload = message.getPayload();

    /*char* sender = message.getSender();
    char* receiver = message.getReceiver();
    char* content = message.getContent();

    std::cout << "Allocating... operator=(const Message& message)\n";

    // TODO: These mallocs shouldn't be necessary. 
    // These char arrays can be initizialized witht the necessary size.
    _sender = (char*) malloc((strlen(sender) + 1) * sizeof(char));
    _receiver = (char*) malloc((strlen(receiver) + 1) * sizeof(char));
    _content = (char*) malloc((strlen(content) + 1) * sizeof(char));
        
    memcpy(_sender, sender, strlen(sender) + 1);
    memcpy(_receiver, receiver, strlen(receiver) + 1);
    memcpy(_content, content, strlen(content) + 1);*/

    return *this;
}

void Message::toByteArray(byte* destByteArray) {
    size_t payloadSize = _length - (sizeof(_timestamp) + 1 + sizeof(_sender) + sizeof(_receiver));

    // TODO: Check if this makes sense
    byte* lengthPtr = destByteArray;
    byte* timestampPtr = lengthPtr + sizeof(short);
    byte* typePtr = timestampPtr + sizeof(unsigned long);
    byte* senderPtr = typePtr + 1;
    byte* receiverPtr = senderPtr + sizeof(int);
    byte* payloadPtr = receiverPtr + sizeof(int);
    byte* endPtr = payloadPtr + payloadSize;

    memcpy(lengthPtr, &_length, sizeof(short));
    memcpy(timestampPtr, &_timestamp, sizeof(unsigned long));

    memcpy(typePtr, &_type, sizeof(MessageType));
    *(typePtr) << 8*(sizeof(MessageType) - 1);

    memcpy(senderPtr, &_sender, sizeof(int));
    memcpy(receiverPtr, &_receiver, sizeof(int));

    _payload = (byte*) malloc(payloadSize);
    memcpy(payloadPtr, &_payload, payloadSize);

    memcpy(endPtr, "", 1);
}

void Message::toCharArray(char* destCharArray) {
    destCharArray[0] = '\0';

    //char idStr[12];
    sprintf(destCharArray, "%d", _length);

    char typeStr[2];
    sprintf(typeStr, "%d", _type);

    char timestampStr[12];
    sprintf(timestampStr, "%lu", _timestamp);

    char senderStr[12];
    sprintf(senderStr, "%d", _sender);

    char receiverStr[12];
    sprintf(receiverStr, "%d", _receiver);

    char payloadStr[512];
    int payloadInt = 0;
    size_t payloadSize = _length - (sizeof(_timestamp) + 1 + sizeof(_sender) + sizeof(_receiver));
    memcpy(&payloadInt, _payload, payloadSize < sizeof(int) ? payloadSize : sizeof(int));
    sprintf(payloadStr, "%d", payloadInt);

    //strcat(destCharArray, idStr);
    strcat(destCharArray, "#");
    strcat(destCharArray, typeStr);
    strcat(destCharArray, "#");
    strcat(destCharArray, timestampStr);
    strcat(destCharArray, "#");
    strcat(destCharArray, senderStr);
    strcat(destCharArray, "#");
    strcat(destCharArray, receiverStr);
    strcat(destCharArray, "#");
    strcat(destCharArray, payloadStr);
}

short Message::getLength() const {
    return _length;
}

unsigned long Message::getTimestamp() const {
    return _timestamp;
}

MessageType Message::getType() const {
    return _type;
}

int Message::getSender() const {
    return _sender;
}

int Message::getReceiver() const {
    return _receiver;
}

byte* Message::getPayload() const {
    return _payload;
}