#ifndef MESSAGE_H
#define MESSAGE_H

enum MessageType {DATA = 0, ACK = 1};

class Message {
    public:
        Message(char* rawMessage);
        Message(long id, MessageType type, unsigned long timestamp, char* sender, char* receiver, char* content);
        ~Message();
        void toCharArray(char* destCharArray);

        long getId();
        MessageType getType();
        char* getSender();
        char* getReceiver();
        char* getContent();
        
    private:
        long _id;
        MessageType _type;
        long _timestamp;
        char* _sender;
        char* _receiver;
        char* _content;
};

#endif