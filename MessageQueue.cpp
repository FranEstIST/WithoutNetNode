#include "MessageQueue.h"

class MessageQueue {
    public:
        MessageQueue(int maxSize = 50) {
            _maxSize = maxSize;
            _messagesById = std::map<long, Message>();
            moveToStart();
        }

        void addMessage(Message message) {
            if(_messagesById.size() >= 50) {
                long oldestId = _messagesById.end()->first;
                _messagesById.erase(oldestId);
            }
            _messagesById.insert({message.getId(), message});
        }
        void moveToStart() {
            _messageIterator = _messagesById.begin();
        }
        Message getNextMessage() {
            _messageIterator->second;
        }
        void removeMessage(long id) {
            _messagesById.erase(id);
        }
    private:
        std::map<long, Message> _messagesById;
        std::map<long, Message>::iterator _messageIterator;
        int _maxSize;
};