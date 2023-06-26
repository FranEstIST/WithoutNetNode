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

    _messagesById.insert(std::make_pair(message.getId(), message));

    if(_messagesById.size() == 1) {
        // TODO: Does it make sense to have the iterator move to the start here?
        moveToStart();
    }
}

void MessageQueue::moveToStart() {
    _messageIterator = _messagesById.begin();
}

Message MessageQueue::getNextMessage() {
    Message message = _messageIterator->second;
    if(!reachedLastMessage()) {
        _messageIterator++;
    }
    return message;
}

void MessageQueue::removeMessage(long id) {
    _messagesById.erase(id);
}

Message MessageQueue::getMessage(long id) {
    return _messagesById[id];
}

bool MessageQueue::reachedLastMessage() {
    return _messagesById.end()->second.getId() == _messageIterator->second.getId();
}