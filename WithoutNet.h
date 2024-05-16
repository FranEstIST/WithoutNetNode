#ifndef WithoutNet_h
#define WithoutNet_h

#include <ArduinoBLE.h>

#include "Arduino.h"
#include "Message.h"
#include "MessageQueue.h"

typedef void (*IncomingMessageHandler)(Message msg);

int begin(int id, char *localName, long timestampOffset = 0,
          bool verbose = false);
void runLoop();
void sendInt(int msg, int destId);
void sendCharArray(char *msg, int destId);
void addMessageToQueue(byte *payload, short payloadLength, int destId);
void setMaxPendingMsgs(int size);
void getPendingMsgNum();
void setIncomingMessageHandler(IncomingMessageHandler handler);

static void onConnected(BLEDevice central);
static void onDisconnected(BLEDevice central);

static void moveToNextMsg(BLEDevice central, BLECharacteristic characteristic);
static void resetMessagePointer();

static void writeNextChunk();
void printByteArray(byte *byteArray, int size);
void printByteArrayCompact(byte *byteArray, int size);

static void onIncomingMsgCharWritten(BLEDevice central,
                                     BLECharacteristic characteristic);
void dequeueMsg(char *msgUuid);

#endif