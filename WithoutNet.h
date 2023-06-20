#ifndef WithoutNet_h
#define WithoutNet_h

#include "Arduino.h"
#include <ArduinoBLE.h>
#include "Message.h"
#include "MessageQueue.h"

typedef void (*IncomingMessageHandler)(Message msg);

/*class WithoutNet {
    public:
        void begin();
        void addSubscribedNode(char* uuid);
        void startSending(char* msg);
        void stopSending();
        void stopSending(char* msg);

};*/

/**
 * New proposed implementation: To send a message, all a node has to do is call a func called
 * send, which takes as params the msg and the dest node's uuid. This func simply adds a new message
 * with header containing the ts, source and dest, to the pending msg list (a bl characteristic)
 * 
 */

class WithoutNet {
    public:
        WithoutNet(char* uuid, char* localName);
        int begin();
        void send(char* msg, char* destUuid);
        void setMaxPendingMsgs(int size);
        void getPendingMsgNum();
        void setIncomingMessageHandler(IncomingMessageHandler handler);
    private:
        char _uuid[37];
        char _localName[33];

        BLEService _WNService;
        static BLEStringCharacteristic _outgoingMsgChar;
        static BLEStringCharacteristic _incomingMsgChar;

        static IncomingMessageHandler _incomingMessageHandler;

        long _messageCounter = 0;

        static MessageQueue _messageQueue;

        char* _separator = "#";

        static void onConnected(BLEDevice central);
        static void onDisconnected(BLEDevice central);

        static void moveToNextMsg(BLEDevice central, BLECharacteristic characteristic);
        static void resetMessagePointer();
        //char* generateMessageUuid();
        static void onIncomingMsgCharWritten(BLEDevice central, BLECharacteristic characteristic);
        void dequeueMsg(char* msgUuid);
};

class Node {
    public:
        Node();
        char* getUuid();
};

#endif