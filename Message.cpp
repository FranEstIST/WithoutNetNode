#include "Message.h"
#include <stdio.h>
#include <cstdlib>
#include <string.h>

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
    free(_sender);
    free(_receiver);
    free(_content);
}

void Message::toCharArray(char* destCharArray) {
    destCharArray[0] = '\0';

    char idStr[8];
    sprintf(destCharArray, "%ld", _id);

    char typeStr[1];
    sprintf(typeStr, "%d", _type);

    char timestampStr[1];
    sprintf(timestampStr, "%d", _timestamp);

    strcat(destCharArray, idStr);
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

long Message::getId() {
    return _id;
}

MessageType Message::getType() {
    return _type;
}

char* Message::getSender() {
    return _sender;
}

char* Message::getReceiver() {
    return _receiver;
}

char* Message::getContent() {
    return _content;
}