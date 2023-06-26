#include "WithoutNetSimple.h";
#include "MessageQueue.h"
#include <Arduino.h>
#include <ArduinoBLE.h>
//#include "UUIDs.h"

const char* WNSERVICE_SIMPLE_UUID = "b19fbebe-dbd4-11ed-afa1-0242ac120002";
const char* INCOMING_MSG_CHAR_SIMPLE_UUID = "75c57d2e-7efe-4d7b-af73-e2835f2d48d4";
const char* OUTGOING_MSG_CHAR_SIMPLE_UUID = "dbe60c86-e91d-11ed-a05b-0242ac120003";

char uuid[37];
char localName[33];

BLEService WNService(WNSERVICE_SIMPLE_UUID);

BLEStringCharacteristic outgoingMsgChar(OUTGOING_MSG_CHAR_SIMPLE_UUID, BLERead, 120);
BLEStringCharacteristic incomingMsgChar(INCOMING_MSG_CHAR_SIMPLE_UUID, BLERead | BLEWrite, 120);

IncomingMessageHandler incomingMessageHandler;

long messageCounter = 0;
MessageQueue messageQueue = MessageQueue(50);

char* _separator = "#";

int begin(char *uuid, char *localName)
{
    incomingMessageHandler = nullptr;

    memcpy(localName, localName, strlen(localName) + 1);
    memcpy(uuid, uuid, strlen(uuid) + 1);

    outgoingMsgChar = BLEStringCharacteristic(OUTGOING_MSG_CHAR_SIMPLE_UUID, BLERead, 120);
    incomingMsgChar = BLEStringCharacteristic(INCOMING_MSG_CHAR_SIMPLE_UUID, BLERead | BLEWrite, 120);

    // Begin initialization
    if (!BLE.begin())
    {
        return 0;
    }

    // TODO: Check if serial has been initialized before printing

    Serial.print("UUID: ");
    Serial.println(uuid);

    Serial.print("Local name: ");
    Serial.println(localName);

    pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

    BLE.setLocalName(localName);
    BLE.setAdvertisedService(WNService); // Add the service UUID

    // Add the outgoing message characteristic (where messages sent from this
    // device are written to)
    WNService.addCharacteristic(outgoingMsgChar);

    // Add the incoming message characteristic (where messages sent to this
    // device are written to)
    WNService.addCharacteristic(incomingMsgChar);

    outgoingMsgChar.writeValue("");
    incomingMsgChar.writeValue("");

    incomingMsgChar.setEventHandler(BLEWritten, onIncomingMsgCharWritten);
    outgoingMsgChar.setEventHandler(BLERead, moveToNextMsg);

    BLE.setEventHandler(BLEConnected, onConnected);
    BLE.setEventHandler(BLEDisconnected, onDisconnected);

    BLE.addService(WNService); // Add the WN Service

    BLE.advertise();

    return 1;
}

void runLoop() {
    BLEDevice central = BLE.central();
}

void send(char *msg, char *destUuid)
{
    Serial.print("Message to be added to the message queue: ");
    Serial.println(msg);
    
    Message message = Message(messageCounter, (MessageType)DATA, millis(), uuid, destUuid, msg);

    char fullMessage[512];
    message.toCharArray(fullMessage);
    Serial.print("Full message to be added to the message queue: ");
    Serial.println(fullMessage);

    Serial.print("Message id: ");
    char idChar[12];
    sprintf(idChar, "%d", message.getId());
    Serial.println(idChar);

    MessageQueue testQ = MessageQueue(50);

    testQ.addMessage(message);

    Serial.print("Message added to the message queue: ");
    Serial.println(fullMessage);
}

void setMaxPendingMsgs(int size)
{
    // TODO
}

void getPendingMsgNum()
{
    // TODO
}

void setIncomingMessageHandler(IncomingMessageHandler incomingMessageHandler)
{
    incomingMessageHandler = incomingMessageHandler;
}

void moveToNextMsg(BLEDevice central, BLECharacteristic characterstic)
{
    Message message = messageQueue.getNextMessage();

    // Write message to outgoing message char
    char messageCharArray[512];
    message.toCharArray(messageCharArray);
    outgoingMsgChar.setValue(messageCharArray);
}

void onConnected(BLEDevice central)
{
    Serial.println("Connected");
    digitalWrite(LED_BUILTIN, HIGH);
    //resetMessagePointer();
}

void onDisconnected(BLEDevice central)
{
    Serial.println("Disconnected");
    digitalWrite(LED_BUILTIN, LOW);
}

void resetMessagePointer()
{
    messageQueue.moveToStart();

    Message message = messageQueue.getNextMessage();

    // Write message to outgoing message char
    char messageCharArray[512];
    message.toCharArray(messageCharArray);
    outgoingMsgChar.setValue(messageCharArray);
}

void onIncomingMsgCharWritten(BLEDevice central, BLECharacteristic characterstic)
{
    // Get the char* inside the imchar
    // Call onIncomingMessageReceived(char* msg)

    char *messageChar = (char *)characterstic.value();

    Message message = Message(messageChar);

    if (message.getType() == ACK)
    {
        // TODO: Check if content is a valid message ID
        messageQueue.removeMessage(atol(message.getContent()));
    }
    else
    {
        incomingMessageHandler(message);
    }
}