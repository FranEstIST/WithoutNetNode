#include "WithoutNet.h";
#include "MessageQueue.h"
#include <Arduino.h>
#include <ArduinoBLE.h>
//#include "UUIDs.h"

const char* WNSERVICE_SIMPLE_UUID = "b19fbebe-dbd4-11ed-afa1-0242ac120002";
const char* INCOMING_MSG_CHAR_SIMPLE_UUID = "75c57d2e-7efe-4d7b-af73-e2835f2d48d4";
const char* OUTGOING_MSG_CHAR_SIMPLE_UUID = "dbe60c86-e91d-11ed-a05b-0242ac120003";
const char* NODE_UUID_CHAR_UUID = "89954326-56e6-4797-b62f-07ac5c4c3789";

char uuid[37];
char localName[33];

int id;

BLEService WNService(WNSERVICE_SIMPLE_UUID);

BLEIntCharacteristic nodeUuidChar(NODE_UUID_CHAR_UUID, BLERead);
BLECharacteristic outgoingMsgChar(OUTGOING_MSG_CHAR_SIMPLE_UUID, BLERead, 20);
BLECharacteristic incomingMsgChar(INCOMING_MSG_CHAR_SIMPLE_UUID, BLERead | BLEWrite, 20);

IncomingMessageHandler incomingMessageHandler;

long messageCounter = 0;
MessageQueue messageQueue = MessageQueue(10);

char* _separator = "#";
bool allMessagesRead = false;

bool _verbose = false;

Message _currentMesssage;
char _currentMessageByteArray[512];
short _currentMessageLength;
bool _hasSentAllChunks = true;
int _currentChunkIndex = -1;

char _currentIncomingMessageByteArray[512];
int _currentIncomingMessageChunkIndex = 0;
short _currentIncomingMessageLength = 0;

struct {
    Message message;
    byte messageByteArray[512];
    int currentChunkIndex = 0;
    short messageLength = 0;
} _incomingMessageChunks, _outgoingMessageChunks;


int begin(int idPrime, char *localNamePrime, bool verbose)
{
    incomingMessageHandler = nullptr;

    id = idPrime;

    memcpy(localName, localNamePrime, strlen(localNamePrime) + 1);

    _verbose = verbose;

    // Begin initialization
    if (!BLE.begin())
    {
        return 0;
    }

    if(_verbose) {
        Serial.println("--WithoutNet node started--");

        Serial.print(">ID: ");
        Serial.println(id);

        Serial.print(">Local name: ");
        Serial.println(localName);
    }

    pinMode(LED_BUILTIN, OUTPUT); // initialize the built-in LED pin to indicate when a central is connected

    BLE.setLocalName(localName);
    BLE.setAdvertisedService(WNService); // Add the service UUID

    // Add the node uuid characteristic 
    WNService.addCharacteristic(nodeUuidChar);

    // Add the outgoing message characteristic (where messages sent from this
    // device are written to)
    WNService.addCharacteristic(outgoingMsgChar);

    // Add the incoming message characteristic (where messages sent to this
    // device are written to)
    WNService.addCharacteristic(incomingMsgChar);

    //WNService.hasCharacteristic()

    nodeUuidChar.writeValue(id);
    outgoingMsgChar.writeValue("0");
    incomingMsgChar.writeValue("0");

    incomingMsgChar.setEventHandler(BLEWritten, onIncomingMsgCharWritten);
    outgoingMsgChar.setEventHandler(BLERead, moveToNextMsg);

    BLE.setEventHandler(BLEConnected, onConnected);
    BLE.setEventHandler(BLEDisconnected, onDisconnected);

    BLE.addService(WNService); // Add the WN Service

    BLE.advertise();

    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);

    return 1;
}

void runLoop() {
    BLEDevice central = BLE.central();
}

void sendInt(int msg, int destId)
{
    if(_verbose) {
        Serial.print(">Message to be added to the message queue: ");
        Serial.println(msg);
    }
    
    short payloadLength = sizeof(int);
    byte* payload = (byte*) &msg;
    /*byte payload[sizeof(int)];
    Message::copyAndReverseEndianness(payload, (byte*) &msg, sizeof(int));*/
    
    addMessageToQueue(payload, payloadLength, destId);
}

void sendString(char* msg, int destId)
{
    if(_verbose) {
        Serial.print(">Message to be added to the message queue: ");
        Serial.println(msg);
    }

    short payloadLength = strlen(msg) + 1;
    byte* payload = (byte*) &msg;
    
    addMessageToQueue(payload, payloadLength, destId);
}

void addMessageToQueue(byte* payload, short payloadLength, int destId) {
    Message message = Message(millis(), (MessageType)DATA, id, destId, payload, payloadLength);

    char fullMessage[512];
    message.toCharArray(fullMessage);

    messageQueue.addMessage(message);

    messageCounter++;

    if(_verbose) {
        Serial.print(">Message added to the message queue: ");
        Serial.println(fullMessage);
    }

    byte messageByteArray[512];
    message.toByteArray(messageByteArray);
    //printByteArray(messageByteArray, 20);
}

void setMaxPendingMsgs(int size)
{
    // TODO
}

void getPendingMsgNum()
{
    // TODO
}

void setIncomingMessageHandler(IncomingMessageHandler incomingMessageHandlerPrime)
{
    incomingMessageHandler = incomingMessageHandlerPrime;
}

void moveToNextMsg(BLEDevice central, BLECharacteristic characterstic)
{
    writeNextChunk();
}

void onConnected(BLEDevice central)
{
    if(_verbose) {
        Serial.print("//------\\\\ Connected to smartphone with address: ");
        Serial.print(central.address());
        Serial.println(" //------\\\\");
    }

    digitalWrite(LED_BUILTIN, HIGH);
    resetMessagePointer();
}

void onDisconnected(BLEDevice central)
{
    if(_verbose) {
        Serial.print("\\\\------// Disconnected from smartphone with address: ");
        Serial.print(central.address());
        Serial.println(" \\\\------//");
    }

    digitalWrite(LED_BUILTIN, LOW);
}

void resetMessagePointer()
{
    if(!messageQueue.isEmpty()) {
        messageQueue.moveToStart();
    }

    writeNextChunk();
}

void writeNextChunk() {
    byte messageChunkByteArray[20];
    int messageChunkLength = 0;

    if(!messageQueue.isEmpty()) {
        if(_outgoingMessageChunks.currentChunkIndex == 0) {
            _outgoingMessageChunks.message = messageQueue.getNextMessage();

            if(_verbose) {
                char currentMessageCharArray[512];
                _outgoingMessageChunks.message.toCharArray(currentMessageCharArray);

                Serial.print(">Next message in Outgoing Msg Char: ");
                Serial.println(currentMessageCharArray);
            }
        }

        messageChunkLength = _outgoingMessageChunks.message.getByteArrayChunk(messageChunkByteArray, _outgoingMessageChunks.currentChunkIndex);
    }

    if(messageChunkLength == 0) {
        // All chunks have been read by the client, now the end message must be sent

        messageChunkByteArray[0] = 0x0;
        messageChunkByteArray[1] = 0x0;

        if(messageQueue.isEmpty() || messageQueue.reachedLastMessage()) {
            if(_verbose) {
                Serial.println(">Sending \"end of message queue\" message...");
            }

            messageChunkByteArray[2] = 0x1;
            messageChunkByteArray[3] = 0x0;
        } else {
            if(_verbose) {
                Serial.println(">Sending \"end of message \" message...");
            }

            messageChunkByteArray[2] = 0x00;
            messageChunkByteArray[3] = 0x0;
        }

        _outgoingMessageChunks.currentChunkIndex = 0;
    } else {
        _outgoingMessageChunks.currentChunkIndex++;
    }

    if(_verbose) {
        Serial.print(">Next chunk in Outgoing Msg Char: ");
        printByteArray(messageChunkByteArray, messageChunkLength == 0 ? 4 : messageChunkLength);
    }

    // Write the message chunk or end message to the outgoing message char
    outgoingMsgChar.writeValue(messageChunkByteArray, 20);
}

void printByteArray(byte* byteArray, int size) {
    if(_verbose) {
        Serial.println("Byte array: ");

        for(int i = 0; i < size; i++) {
            Serial.print("Index ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println((int) byteArray[i]);
        }
    
    }
}

void onIncomingMsgCharWritten(BLEDevice central, BLECharacteristic characterstic) {
    if(_verbose) {
        Serial.println(">Message chunk received");

        char *messageChar = (char *)characterstic.value();

        Serial.print(">Message chunk: ");
        Serial.println(messageChar);

        /*Serial.print(">Characteristic size: ");
        Serial.println(characterstic.valueSize());*/
    }

    byte* messageChunkByteArray = (byte*) characterstic.value();

    short messageChunkLength;
    memcpy(&messageChunkLength, messageChunkByteArray, LENGTH_SIZE);

    if(messageChunkLength == 0) {
        // All of the message's chunks have been received. It's time to build the message

        // Add the total message length to the start of the message
        memcpy(_incomingMessageChunks.messageByteArray, 
        &_incomingMessageChunks.messageLength, 
        LENGTH_SIZE);

        Message message = Message(_incomingMessageChunks.messageByteArray);

        if(_verbose) {
            char messageChar[512];
            message.toCharArray(messageChar);

            Serial.print(">Complete message received: ");
            Serial.println(messageChar);
        }

        _incomingMessageChunks.messageLength = 0;
        _incomingMessageChunks.currentChunkIndex = 0;

        /*int msgOne = 1000;

        short payloadLength = sizeof(int);
        byte* payloadOne = (byte*) &msgOne;

        Message message = Message(12000, (MessageType)DATA, 12, 21, payloadOne, 4);*/

        /*if (message.getType() == ACK)
        {
            // TODO: Check if content is a valid message ID
            messageQueue.removeMessage(atol(message.getContent()));
        }
        else
        {*/
            incomingMessageHandler(message);
        //}

        return;
    }

    // Add the current chunk to the rest of the incoming message
    memcpy(_incomingMessageChunks.messageByteArray + (_incomingMessageChunks.currentChunkIndex == 0 ? LENGTH_SIZE : _incomingMessageChunks.currentChunkIndex * 20), 
    messageChunkByteArray + LENGTH_SIZE, 
    messageChunkLength);

    _incomingMessageChunks.messageLength += messageChunkLength;
    _incomingMessageChunks.currentChunkIndex++;
    
    if(_verbose) {
        char *messageChar = (char *)characterstic.value();

        Serial.print(">Message: ");
        Serial.println(messageChar);

        Serial.print(">Characteristic size: ");
        Serial.println(characterstic.valueSize());
    }

}