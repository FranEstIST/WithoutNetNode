#ifndef WithoutNet_h
#define WithoutNet_h

#include "Arduino.h"
#include <ArduinoBLE.h>
#include "Message.h"
#include "MessageQueue.h"

typedef void (*IncomingMessageHandler)(Message msg);

int begin(int id, char *localName);
void runLoop();
void sendInt(int msg, int destId);
void sendChar(char* msg, char* destUuid);
void addMessageToQueue(byte* payload, short payloadLength, int destId);
void setMaxPendingMsgs(int size);
void getPendingMsgNum();
void setIncomingMessageHandler(IncomingMessageHandler handler);

static void onConnected(BLEDevice central);
static void onDisconnected(BLEDevice central);

static void moveToNextMsg(BLEDevice central, BLECharacteristic characteristic);
static void resetMessagePointer();

static void writeNextMessage();
void printByteArray(byte* byteArray, int size);

static void onIncomingMsgCharWritten(BLEDevice central, BLECharacteristic characteristic);
void dequeueMsg(char* msgUuid);

#endif