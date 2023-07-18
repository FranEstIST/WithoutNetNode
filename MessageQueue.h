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
        Message getMessage(unsigned long id);
        void removeMessage(unsigned long id);
        bool reachedLastMessage();
        bool isEmpty();
    private:
        std::map<unsigned long, Message> _messagesById;
        std::map<unsigned long, Message>::iterator _messageIterator;
        int _maxSize;
};

#endif