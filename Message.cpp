#include "Message.h"
#include <stdio.h>
#include <cstdlib>
#include <string.h>
#include <iostream>

Message::Message()
: _id(0),
_type(DATA),
_timestamp(0) {
    std::cout << "Allocating ()...\n";

    // TODO: These mallocs shouldn't be necessary. 
    // These char arrays can be initizialized witht the necessary size.
    _sender = (char*) malloc(sizeof(char));
    _receiver = (char*) malloc(sizeof(char));
    _content = (char*) malloc(sizeof(char));
        
    memcpy(_sender, "", 1);
    memcpy(_receiver, "", 1);
    memcpy(_content, "", 1);
}

Message::Message(char* rawMessage) {
    // TODO: Split message into the different fields
    // 8 byte | 1 byte | 8 byte | 16 byte | 16 byte | 463 byte ?
    char* idChar = strtok(rawMessage, "#");
    char* typeChar = strtok(NULL, "#");
    char* timestampChar = strtok(NULL, "#");
    char* sender = strtok(NULL, "#");
    char* receiver = strtok(NULL, "#");
    char* content = strtok(NULL, "#");

    memcpy(&_id, idChar, sizeof(long));
    memcpy(&_type, typeChar, sizeof(MessageType));
    memcpy(&_timestamp, timestampChar, sizeof(unsigned long));

    std::cout << "Allocating (char* rawMessage)...\n";

    // TODO: These mallocs shouldn't be necessary. 
    // These char arrays can be initizialized witht the necessary size.
    _sender = (char*) malloc((strlen(sender) + 1) * sizeof(char));
    _receiver = (char*) malloc((strlen(receiver) + 1) * sizeof(char));
    _content = (char*) malloc((strlen(content) + 1) * sizeof(char));

    memcpy(_sender, sender, strlen(sender) + 1);
    memcpy(_receiver, receiver, strlen(receiver) + 1);
    memcpy(_content, content, strlen(content) + 1);
}

Message::Message(long id, MessageType type, unsigned long timestamp, char* sender, char* receiver, char* content) 
: _id(id),
_type(type),
_timestamp(timestamp) {

    std::cout << "Allocating (long id, MessageType type, unsigned long timestamp, char* sender, char* receiver, char* content)...\n";

    // TODO: These mallocs shouldn't be necessary. 
    // These char arrays can be initizialized witht the necessary size.
    _sender = (char*) malloc((strlen(sender) + 1) * sizeof(char));
    _receiver = (char*) malloc((strlen(receiver) + 1) * sizeof(char));
    _content = (char*) malloc((strlen(content) + 1) * sizeof(char));
        
    memcpy(_sender, sender, strlen(sender) + 1);
    memcpy(_receiver, receiver, strlen(receiver) + 1);
    memcpy(_content, content, strlen(content) + 1);
}

/*Message::Message(Message &message) {
    Message(message.getId(),
     message.getType(),
      message.getTimestamp(),
       message.getSender(),
        message.getReceiver(),
         message.getContent());
}*/

Message::Message(const Message &message)
: _id(message.getId()),
_type(message.getType()),
_timestamp(message.getTimestamp()) {
    char* sender = message.getSender();
    char* receiver = message.getReceiver();
    char* content = message.getContent();

    std::cout << "Allocating (const Message &message)...\n";

    // TODO: These mallocs shouldn't be necessary. 
    // These char arrays can be initizialized witht the necessary size.
    _sender = (char*) malloc((strlen(sender) + 1) * sizeof(char));
    _receiver = (char*) malloc((strlen(receiver) + 1) * sizeof(char));
    _content = (char*) malloc((strlen(content) + 1) * sizeof(char));
        
    memcpy(_sender, sender, strlen(sender) + 1);
    memcpy(_receiver, receiver, strlen(receiver) + 1);
    memcpy(_content, content, strlen(content) + 1);
}

Message::~Message() {
    std::cout << "Freeing...\n";
    free(_sender);
    free(_receiver);
    free(_content);
}

Message& Message::operator=(const Message& message) {
    _id = message.getId();
    _type = message.getType();
    _timestamp = message.getTimestamp();
    _sender = message.getSender();
    _receiver = message.getReceiver();
    _content = message.getContent();

    /*char* sender = message.getSender();
    char* receiver = message.getReceiver();
    char* content = message.getContent();

    std::cout << "Allocating... operator=(const Message& message)\n";

    // TODO: These mallocs shouldn't be necessary. 
    // These char arrays can be initizialized witht the necessary size.
    _sender = (char*) malloc((strlen(sender) + 1) * sizeof(char));
    _receiver = (char*) malloc((strlen(receiver) + 1) * sizeof(char));
    _content = (char*) malloc((strlen(content) + 1) * sizeof(char));
        
    memcpy(_sender, sender, strlen(sender) + 1);
    memcpy(_receiver, receiver, strlen(receiver) + 1);
    memcpy(_content, content, strlen(content) + 1);*/

    return *this;
}



void Message::toCharArray(char* destCharArray) {
    destCharArray[0] = '\0';

    //char idStr[12];
    sprintf(destCharArray, "%ld", _id);

    char typeStr[2];
    sprintf(typeStr, "%d", _type);

    char timestampStr[12];
    sprintf(timestampStr, "%lu", _timestamp);

    //strcat(destCharArray, idStr);
    strcat(destCharArray, "#");
    strcat(destCharArray, typeStr);
    strcat(destCharArray, "#");
    strcat(destCharArray, timestampStr);
    strcat(destCharArray, "#");
    strcat(destCharArray, _sender);
    strcat(destCharArray, "#");
    strcat(destCharArray, _receiver);
    strcat(destCharArray, "#");
    strcat(destCharArray, _content);
}

long Message::getId() const {
    return _id;
}

MessageType Message::getType() const {
    return _type;
}

unsigned long Message::getTimestamp() const {
    return _timestamp;
}

char* Message::getSender() const {
    return _sender;
}

char* Message::getReceiver() const {
    return _receiver;
}

char* Message::getContent() const {
    return _content;
}