#ifndef WithoutNet_h
#define WithoutNet_h

#include "Arduino.h"
#include <ArduinoBLE.h>
#include "Message.h"

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
        static void moveToNextMsg(BLEDevice central, BLECharacteristic characteristic);
        static void resetMessagePointer(BLEDevice central);
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