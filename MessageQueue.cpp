#include "MessageQueue.h"

MessageQueue::MessageQueue(int maxSize = 50) {
    _maxSize = maxSize;
    _messagesById = std::map<long, Message>();
    moveToStart();
}

void MessageQueue::addMessage(Message message) {
    if(_messagesById.size() >= 50) {
        long oldestId = _messagesById.end()->first;
        _messagesById.erase(oldestId);
    }
    _messagesById.insert({message.getId(), message});
}

void MessageQueue::moveToStart() {
    _messageIterator = _messagesById.begin();
}

Message MessageQueue::getNextMessage() {
    _messageIterator->second;
}

void MessageQueue::removeMessage(long id) {
    _messagesById.erase(id);
}