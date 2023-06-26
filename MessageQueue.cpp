#include "MessageQueue.h"
#include "Arduino.h"

MessageQueue::MessageQueue(int maxSize = 50) {
    _maxSize = maxSize;
    _messagesById = std::map<long, Message>();
    moveToStart();
}

void MessageQueue::addMessage(Message message) {
    /*if(_messagesById.size() >= 50) {
        Serial.println("Message queue full");
        long oldestId = _messagesById.end()->first;
        _messagesById.erase(oldestId);
    }*/
    
    //Serial.println("Message queue not full");

    //_messagesById.insert(std::make_pair(message.getId(), message));

    //Serial.println(_messagesById. ? "Queue null" : "Queue not null");

    //try {
        //_messagesById.insert(std::make_pair(message.getId(), message));
    /*} catch (std::exception e) {
        Serial.println("Insert into the message queue raised an exception");
    }*/
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