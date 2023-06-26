#ifndef MESSAGE_H
#define MESSAGE_H

enum MessageType {DATA = 0, ACK = 1};

class Message {
    public:
        Message();
        Message(char* rawMessage);
        Message(long id, MessageType type, unsigned long timestamp, char* sender, char* receiver, char* content);
        Message(const Message &message);
        ~Message();
        Message& operator=(const Message& message);

        void toCharArray(char* destCharArray);

        long getId() const;
        MessageType getType() const;
        unsigned long getTimestamp() const;
        char* getSender() const;
        char* getReceiver() const;
        char* getContent() const;
        
    private:
        long _id;
        MessageType _type;
        unsigned long _timestamp;
        char* _sender;
        char* _receiver;
        char* _content;
};

#endif