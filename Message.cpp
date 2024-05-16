#include "Message.h"

#include <Arduino.h>
#include <stdio.h>
#include <string.h>

#include <bitset>
#include <cstddef>
#include <cstdlib>
#include <iostream>

Message::Message() : _length(0), _type(DATA), _timestamp(0) {
    _payload = (byte *)malloc(sizeof(byte));
}

Message::Message(byte *rawMessage) {
    byte *lengthPtr = rawMessage;
    byte *timestampPtr = lengthPtr + sizeof(short);
    byte *typePtr = timestampPtr + sizeof(unsigned long);
    byte *senderPtr = typePtr + 1;
    byte *receiverPtr = senderPtr + sizeof(int);
    byte *payloadPtr = receiverPtr + sizeof(int);

    memcpy(&_length, lengthPtr, sizeof(short));
    memcpy(&_timestamp, timestampPtr, sizeof(unsigned long));

    _type = MessageType::DATA;
    memcpy(&_type, typePtr, 1);

    memcpy(&_sender, senderPtr, sizeof(int));
    memcpy(&_receiver, receiverPtr, sizeof(int));

    size_t payloadSize = _length - (sizeof(_timestamp) + 1 + sizeof(_sender) +
                                    sizeof(_receiver));
    _payload = (byte *)malloc(payloadSize);
    memcpy(_payload, payloadPtr, payloadSize);
}

Message::Message(char *rawMessage) {
    char *lengthChar = strtok(rawMessage, "#");
    char *typeChar = strtok(NULL, "#");
    char *timestampChar = strtok(NULL, "#");
    char *senderChar = strtok(NULL, "#");
    char *receiverChar = strtok(NULL, "#");
    char *payloadChar = strtok(NULL, "#");

    std::string lengthString(lengthChar);
    _length = (short)std::stoi(lengthChar);
    _type = (MessageType)std::stoi(typeChar);
    _timestamp = std::stol(timestampChar);
    _sender = std::stoi(senderChar);
    _receiver = std::stoi(receiverChar);

    size_t payloadSize = _length - (sizeof(_timestamp) + 1 + sizeof(_sender) +
                                    sizeof(_receiver));
    _payload = (byte *)malloc(payloadSize);
    memcpy(_payload, payloadChar, payloadSize);
}

Message::Message(unsigned long timestamp, MessageType type, int sender,
                 int receiver, byte *payload, short payloadLength)
    : _timestamp(timestamp), _type(type), _sender(sender), _receiver(receiver) {
    _length = payloadLength + sizeof(_timestamp) + 1 + sizeof(_sender) +
              sizeof(_receiver);

    _payload = (byte *)malloc(payloadLength);
    memcpy(_payload, payload, payloadLength);
}

Message::Message(const Message &message)
    : _length(message.getLength()),
      _timestamp(message.getTimestamp()),
      _type(message.getType()),
      _sender(message.getSender()),
      _receiver(message.getReceiver()) {
    byte *payload = message.getPayload();

    size_t payloadSize = _length - (sizeof(_timestamp) + 1 + sizeof(_sender) +
                                    sizeof(_receiver));
    _payload = (byte *)malloc(payloadSize);
    memcpy(_payload, payload, payloadSize);
}

Message::~Message() { free(_payload); }

Message &Message::operator=(const Message &message) {
    _length = message.getLength();
    _timestamp = message.getTimestamp();
    _sender = message.getSender();
    _receiver = message.getReceiver();
    _payload = message.getPayload();

    return *this;
}

void Message::toByteArray(byte *destByteArray) {
    size_t payloadSize = _length - (sizeof(_timestamp) + 1 + sizeof(_sender) +
                                    sizeof(_receiver));

    byte *lengthPtr = destByteArray;
    byte *timestampPtr = lengthPtr + sizeof(short);
    byte *typePtr = timestampPtr + sizeof(unsigned long);
    byte *senderPtr = typePtr + 1;
    byte *receiverPtr = senderPtr + sizeof(int);
    byte *payloadPtr = receiverPtr + sizeof(int);
    byte *endPtr = payloadPtr + payloadSize;

    // The numerical values are stored and transmitted in the little endian
    // format, so the receiver is responsible for converting it to the big
    // endianess format

    memcpy(lengthPtr, &_length, sizeof(short));
    memcpy(timestampPtr, &_timestamp, sizeof(unsigned long));

    memcpy(typePtr, &_type, sizeof(MessageType));

    memcpy(senderPtr, &_sender, sizeof(int));
    memcpy(receiverPtr, &_receiver, sizeof(int));

    memcpy(payloadPtr, _payload, payloadSize);

    memcpy(endPtr, "", 1);
}

int Message::getByteArrayChunk(byte *destByteArray, int chunkIndex) {
    // The total chunk size is always <= 20, so the destByteArray to be 20 byte
    // long

    size_t payloadSize = _length - (sizeof(_timestamp) + 1 + sizeof(_sender) +
                                    sizeof(_receiver));
    int payloadOffset = chunkIndex * 18;

    byte *destPayloadPtr;
    short payloadChunkSize;

    byte *destLengthPtr = destByteArray;
    short chunkLength;

    if (chunkIndex == 0) {
        byte *destTimestampPtr = destLengthPtr + sizeof(short);
        byte *destTypePtr = destTimestampPtr + sizeof(unsigned long);
        byte *destSenderPtr = destTypePtr + 1;
        byte *destReceiverPtr = destSenderPtr + sizeof(int);

        // The numerical values are stored and transmitted in the little endian
        // format, so the receiver is responsible for converting them to the big
        // endian format

        memcpy(destTimestampPtr, &_timestamp, sizeof(unsigned long));
        memcpy(destTypePtr, &_type, sizeof(MessageType));
        memcpy(destSenderPtr, &_sender, sizeof(int));
        memcpy(destReceiverPtr, &_receiver, sizeof(int));

        payloadChunkSize = payloadSize < 5 ? payloadSize : 5;
        destPayloadPtr = destReceiverPtr + sizeof(int);
        chunkLength = sizeof(_timestamp) + 1 + sizeof(_sender) +
                      sizeof(_receiver) + payloadChunkSize;
    } else {
        payloadChunkSize = (payloadSize - payloadOffset) < 18
                               ? (payloadSize - payloadOffset)
                               : 18;
        destPayloadPtr = destLengthPtr + sizeof(short);
        chunkLength = payloadChunkSize;
    }

    if (payloadChunkSize > 0) {
        memcpy(destLengthPtr, &chunkLength, sizeof(short));
        memcpy(destPayloadPtr, _payload + payloadOffset, payloadChunkSize);
        // Return the total size of the message chunk
        return sizeof(short) + chunkLength;
    } else {
        // There are no more message chunks
        return 0;
    }
}

void Message::copyAndReverseEndianness(byte *dst, byte *src, size_t size) {
    int i = size - 1;

    for (int i = 0; i < size; i++) {
        dst[i] = src[size - 1 - i];
    }
}

void Message::toCharArray(char *destCharArray) {
    destCharArray[0] = '\0';

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
    size_t payloadSize = _length - (sizeof(_timestamp) + 1 + sizeof(_sender) +
                                    sizeof(_receiver));
    memcpy(&payloadInt, _payload,
           payloadSize < sizeof(int) ? payloadSize : sizeof(int));
    sprintf(payloadStr, "%d", payloadInt);

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

short Message::getLength() const { return _length; }

unsigned long Message::getTimestamp() const { return _timestamp; }

MessageType Message::getType() const { return _type; }

int Message::getSender() const { return _sender; }

int Message::getReceiver() const { return _receiver; }

byte *Message::getPayload() const { return _payload; }

short Message::getPayloadLength() const {
    return _length -
           (sizeof(_timestamp) + 1 + sizeof(_sender) + sizeof(_receiver));
}

int Message::getPayloadAsInt() const {
    int payloadInt = 0;
    size_t payloadSize = _length - (sizeof(_timestamp) + 1 + sizeof(_sender) +
                                    sizeof(_receiver));
    memcpy(&payloadInt, _payload,
           payloadSize < sizeof(int) ? payloadSize : sizeof(int));

    return payloadInt;
}

void Message::getPayloadAsCharArray(char *destCharArray,
                                    int destCharArraySize) const {
    size_t payloadSize = _length - (sizeof(_timestamp) + 1 + sizeof(_sender) +
                                    sizeof(_receiver));
    memcpy(destCharArray, _payload,
           payloadSize < destCharArraySize ? payloadSize : destCharArraySize);
}