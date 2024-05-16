#include "MessageQueue.h"

MessageQueue::MessageQueue(int maxSize = 50) {
    _maxSize = maxSize;
    _messagesById = std::map<unsigned long, Message>();
    moveToStart();
}

void MessageQueue::addMessage(Message message) {
    if (_messagesById.size() >= 50) {
        long oldestId = _messagesById.end()->first;
        _messagesById.erase(oldestId);
    }

    _messagesById.insert(std::make_pair(message.getTimestamp(), message));

    if (_messagesById.size() == 1) {
        moveToStart();
    }
}

void MessageQueue::moveToStart() { _messageIterator = _messagesById.begin(); }

Message MessageQueue::getNextMessage() {
    Message message = _messageIterator->second;
    removeMessage(_messageIterator->first);

    if (!reachedLastMessage()) {
        _messageIterator++;
    }

    return message;
}

void MessageQueue::removeMessage(unsigned long id) { _messagesById.erase(id); }

Message MessageQueue::getMessage(unsigned long id) { return _messagesById[id]; }

bool MessageQueue::reachedLastMessage() {
    return _messagesById.end()->second.getTimestamp() ==
           _messageIterator->second.getTimestamp();
}

bool MessageQueue::isEmpty() { return _messagesById.empty(); }