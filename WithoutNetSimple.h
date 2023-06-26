#ifndef WithoutNetSimple_h
#define WithoutNetSimple_h

#include "Arduino.h"
#include <ArduinoBLE.h>
#include "Message.h"
#include "MessageQueue.h"

typedef void (*IncomingMessageHandler)(Message msg);

int begin(char *uuid, char *localName);
void runLoop();
void send(char* msg, char* destUuid);
void setMaxPendingMsgs(int size);
void getPendingMsgNum();
void setIncomingMessageHandler(IncomingMessageHandler handler);

static void onConnected(BLEDevice central);
static void onDisconnected(BLEDevice central);

static void moveToNextMsg(BLEDevice central, BLECharacteristic characteristic);
static void resetMessagePointer();

static void onIncomingMsgCharWritten(BLEDevice central, BLECharacteristic characteristic);
void dequeueMsg(char* msgUuid);

#endif