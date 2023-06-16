#include "Message.h"

class Message {
    public:
        Message(char* rawMessage) {
            // TODO: Split message into the different fields
            // 8 byte

        }

        Message(long id, MessageType type, long timestamp, char* sender, char* receiver, char* content) 
        : _id(id),
        _type(type),
        _timestamp(timestamp),
        _sender(sender),
        _receiver(receiver),
        _content(content) {}

        void toCharArray(char* destCharArray) {
            
        }

    private:
        long _id;
        MessageType _type;
        long _timestamp;
        char* _sender;
        char* _receiver;
        char* _content;
};