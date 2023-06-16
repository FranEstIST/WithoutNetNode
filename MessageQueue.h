#include <map>
#include <iterator>
#include "Message.h"

class MessageQueue {
    public:
        void addMessage(Message message);
        void moveToStart();
        Message getNextMessage();
        void removeMessage(long id);
    private:
        std::map<long, Message> _messagesById;
        std::map<long, Message>::iterator _messageIterator;
};