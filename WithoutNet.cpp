#include "WithoutNet.h"
#include "UUIDs.h"
#include "MessageQueue.h"
#include <C:\Users\Francisco\Documents\Arduino\libraries\UUID\UUID.h>
#include <list>

BLEStringCharacteristic WithoutNet::_outgoingMsgChar(OUTGOING_MSG_CHAR_UUID, BLERead, 120);
BLEStringCharacteristic WithoutNet::_incomingMsgChar(INCOMING_MSG_CHAR_UUID, BLERead | BLEWrite, 120);
IncomingMessageHandler WithoutNet::_incomingMessageHandler;
MessageQueue WithoutNet::_messageQueue(50);

WithoutNet::WithoutNet(char *uuid, char *localName, BLELocalDevice &BLE)
    : _WNService(WNSERVICE_UUID),
    _BLE(BLE)
{
    /*BLE.address().toCharArray(uuid, 48);*/
    // TODO: Check if the uuid and local name are valid

    _incomingMessageHandler = nullptr;

    memcpy(_localName, localName, strlen(localName) + 1);
    memcpy(_uuid, uuid, strlen(uuid) + 1);

    _outgoingMsgChar = BLEStringCharacteristic(OUTGOING_MSG_CHAR_UUID, BLERead, 120);
    _incomingMsgChar = BLEStringCharacteristic(INCOMING_MSG_CHAR_UUID, BLERead | BLEWrite, 120);
}

int WithoutNet::begin()
{
    // Begin initialization
    if (!_BLE.begin())
    {
        return 0;
    }

    //Serial.begin(9600);

    /*if(!Serial) 
    {
        return 0;
    }*/

    // TODO: Check if serial has been initialized before printing

    //Serial.println("Hello!");

    Serial.print("UUID: ");
    Serial.println(_uuid);

    Serial.print("Local name: ");
    Serial.println(_localName);

    pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

    _BLE.setLocalName(_localName);
    _BLE.setAdvertisedService(_WNService); // Add the service UUID

    // Add the outgoing message characteristic (where messages sent from this
    // device are written to)
    _WNService.addCharacteristic(_outgoingMsgChar);

    // Add the incoming message characteristic (where messages sent to this
    // device are written to)
    _WNService.addCharacteristic(_incomingMsgChar);

    _incomingMsgChar.setEventHandler(BLEWritten, onIncomingMsgCharWritten);
    _outgoingMsgChar.setEventHandler(BLERead, moveToNextMsg);

    _BLE.setEventHandler(BLEConnected, onConnected);
    _BLE.setEventHandler(BLEDisconnected, onDisconnected);

    _BLE.addService(_WNService); // Add the WN Service

    _BLE.advertise();

    return 1;
}

void WithoutNet::send(char *msg, char *destUuid)
{
    Serial.print("Message to be added to the message queue: ");
    Serial.println(msg);
    
    Message message = Message(_messageCounter, (MessageType)DATA, millis(), _uuid, destUuid, msg);

    char fullMessage[512];
    message.toCharArray(fullMessage);
    Serial.print("Full message to be added to the message queue: ");
    Serial.println(fullMessage);

    Serial.print("Message id: ");
    char idChar[12];
    sprintf(idChar, "%d", message.getId());
    Serial.println(idChar);

    MessageQueue testQ = MessageQueue(50);

    //testQ.addMessage(message);

    Serial.print("Message added to the message queue: ");
    Serial.println(fullMessage);
}

void WithoutNet::setMaxPendingMsgs(int size)
{
    // TODO
}

void WithoutNet::getPendingMsgNum()
{
    // TODO
}

void WithoutNet::setIncomingMessageHandler(IncomingMessageHandler incomingMessageHandler)
{
    _incomingMessageHandler = incomingMessageHandler;
}

void WithoutNet::moveToNextMsg(BLEDevice central, BLECharacteristic characterstic)
{
    Message message = _messageQueue.getNextMessage();

    // Write message to outgoing message char
    char messageCharArray[512];
    message.toCharArray(messageCharArray);
    _outgoingMsgChar.setValue(messageCharArray);
}

void WithoutNet::onConnected(BLEDevice central)
{
    Serial.println("Connected");
    digitalWrite(LED_BUILTIN, HIGH);
    resetMessagePointer();
}

void WithoutNet::onDisconnected(BLEDevice central)
{
    Serial.println("Disconnected");
    digitalWrite(LED_BUILTIN, LOW);
}

void WithoutNet::resetMessagePointer()
{
    _messageQueue.moveToStart();

    Message message = _messageQueue.getNextMessage();

    // Write message to outgoing message char
    char messageCharArray[512];
    message.toCharArray(messageCharArray);
    _outgoingMsgChar.setValue(messageCharArray);
}

/* TODO: This might be removed in the future
char* generateMessageUuid() {
    UUID uuid;
    uuid.generate();
    return uuid.toCharArray();
}*/

void WithoutNet::onIncomingMsgCharWritten(BLEDevice central, BLECharacteristic characterstic)
{
    // Get the char* inside the imchar
    // Call onIncomingMessageReceived(char* msg)

    char *messageChar = (char *)characterstic.value();

    Message message = Message(messageChar);

    if (message.getType() == ACK)
    {
        // TODO: Check if content is a valid message ID
        _messageQueue.removeMessage(atol(message.getContent()));
    }
    else
    {
        _incomingMessageHandler(message);
    }
}

// void WithoutNet::dequeueMsg(char* msgUuid);