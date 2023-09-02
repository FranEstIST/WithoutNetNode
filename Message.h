#ifndef MESSAGE_H
#define MESSAGE_H

#define LENGTH_SIZE sizeof(short)
#define TIMESTAMP_SIZE sizeof(unsigned long)
#define TYPE_SIZE 1
#define SENDER_SIZE sizeof(int)
#define RECEIVER_SIZE sizeof(int)

#include <cstdlib>

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
        static void copyAndReverseEndianness(byte* dst, byte* src, size_t size);
        void toCharArray(char* destCharArray);
        int getByteArrayChunk(byte* destByteArray, int chunkIndex);

        short getLength() const;
        unsigned long getTimestamp() const;
        MessageType getType() const;
        int getSender() const;
        int getReceiver() const;
        byte* getPayload() const;
        int getPayloadAsInt() const;
        
    private:
        short _length;
        unsigned long _timestamp;
        MessageType _type;
        int _sender;
        int _receiver;
        byte* _payload;
};

#endif