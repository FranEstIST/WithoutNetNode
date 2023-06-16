#include "MessageQueue.h"

class MessageQueue {
    public:
        void addMessage(Message message) {
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
};