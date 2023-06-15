#ifndef WithoutNet_h
#define WithoutNet_h

#include "Arduino.h"

typedef void (*IncomingMessageHandler)(char* msg);

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
        int begin();
        void send(char* msg, char* destUuid);
        void queueMsg(char* msg, char* destUuid);
        void setMaxPendingMsgs(int size);
        void getPendingMsgNum();
        void setIncomingMessageHandler(IncomingMessageHandler handler);
        void onIncomingMessageReceived(char* msg);
    private:
        void dequeueMsg(char* msgUuid);
};

class Node {
    public:
        Node();
        char* getUuid();
};

#endif