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
    _sender = (char*) malloc(sizeof(sender)/sizeof(char));
    _receiver = (char*) malloc(sizeof(receiver)/sizeof(char));
    _content = (char*) malloc(sizeof(content)/sizeof(char));

    memcpy(_sender, sender, sizeof(sender)/sizeof(char));
    memcpy(_receiver, receiver, sizeof(receiver)/sizeof(char));
    memcpy(_content, content, sizeof(content)/sizeof(char));
}

Message::Message(long id, MessageType type, unsigned long timestamp, char* sender, char* receiver, char* content) 
: _id(id),
_type(type),
_timestamp(timestamp) {
    // TODO: These mallocs shouldn't be necessary. 
    // These char arrays can be initizialized witht the necessary size.
    _sender = (char*) malloc(sizeof(sender)/sizeof(char));
    _receiver = (char*) malloc(sizeof(receiver)/sizeof(char));
    _content = (char*) malloc(sizeof(content)/sizeof(char));
        
    memcpy(_sender, sender, sizeof(sender)/sizeof(char));
    memcpy(_receiver, receiver, sizeof(receiver)/sizeof(char));
    memcpy(_content, content, sizeof(content)/sizeof(char));
}

Message::~Message() {
    free(_sender);
    free(_receiver);
    free(_content);
}

void Message::toCharArray(char* destCharArray) {
    char idStr[8];
    sprintf(idStr, "%d", _id);

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