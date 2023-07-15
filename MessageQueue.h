#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <map>
#include <iterator>
#include "Message.h"

class MessageQueue {
    public:
        MessageQueue(int maxSize);
        void addMessage(Message message);
        void moveToStart();
        Message getNextMessage();
        Message getMessage(long id);
        void removeMessage(long id);
        bool reachedLastMessage();
        bool isEmpty();
    private:
        std::map<long, Message> _messagesById;
        std::map<long, Message>::iterator _messageIterator;
        int _maxSize;
};

#endif