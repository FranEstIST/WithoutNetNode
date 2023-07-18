#include <cstdlib>

#ifndef MESSAGE_H
#define MESSAGE_H

typedef unsigned char byte;

enum MessageType {DATA = 0, ACK = 1};

class Message {
    public:
        Message();
        Message(char* rawMessage);
        Message(byte* rawMessage);
        Message(unsigned long timestamp, MessageType type, int sender, int receiver, byte* payload, short payloadLength);
        Message(const Message &message);
        ~Message();
        Message& operator=(const Message& message);

        void toByteArray(byte* destByteArray);
        void toCharArray(char* destCharArray);

        short getLength() const;
        unsigned long getTimestamp() const;
        MessageType getType() const;
        int getSender() const;
        int getReceiver() const;
        byte* getPayload() const;
        
    private:
        short _length;
        unsigned long _timestamp;
        MessageType _type;
        int _sender;
        int _receiver;
        byte* _payload;
};

#endif