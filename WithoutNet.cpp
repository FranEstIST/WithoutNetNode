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
    writeNextMessage();
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

    writeNextMessage();

    /*if(messageQueue.isEmpty()) {
        allMessagesRead = true;
    } else {
        messageQueue.moveToStart();
        allMessagesRead = false;
        writeNextMessage();
    }*/

}

void writeNextChuck() {
    byte messageChunkByteArray[20];
    int messageChunkSize = 0;

    if(!messageQueue.isEmpty()) {
        if(_currentChunkIndex == -1) {
            _currentMesssage = messageQueue.getNextMessage();
            _currentChunkIndex = 0;

            if(_verbose) {
                char currentMessageCharArray[512];
                _currentMesssage.toCharArray(currentMessageCharArray);

                Serial.print(">Next message in Outgoing Msg Char: ");
                Serial.println(currentMessageCharArray);
            }
        }

        messageChunkSize = _currentMesssage.getByteArrayChunk(messageChunkByteArray, _currentChunkIndex);
    }
    

    if(messageChunkSize == 0) {
        // All chunks have been read by the client, now the end message must be sent

        messageChunkByteArray[0] = 0x0;

        if(messageQueue.isEmpty() || messageQueue.reachedLastMessage()) {
            if(_verbose) {
                Serial.println(">Sending \"end of message queue\" message...");
            }

            messageChunkByteArray[1] = 0x01;
        } else {
            if(_verbose) {
                Serial.println(">Sending \"end of message \" message...");
            }

            messageChunkByteArray[1] = 0x00;
        }

        _currentChunkIndex = -1;
    } else {
        _currentChunkIndex++;
    }

    if(_verbose) {
        Serial.print(">Next chunk in Outgoing Msg Char: ");
        printByteArray(messageChunkByteArray, messageChunkSize == 0 ? 2 : messageChunkSize);
    }

    // Write the message chunk or end message to the outgoing message char
    outgoingMsgChar.writeValue(messageChunkByteArray, 20);
}

void writeNextMessage() {
    if(!allMessagesRead) {
        Message message = messageQueue.getNextMessage();

        // Write message to outgoing message char
        char messageCharArray[512];
        message.toCharArray(messageCharArray);

        if(_verbose) {
            Serial.print(">Next message in Outgoing Msg Char: ");
            Serial.println(messageCharArray);
        }

        byte messageByteArray[512];
        message.toByteArray(messageByteArray);

        //printByteArray(messageByteArray, 20);

        //outgoingMsgChar.setValue(messageByteArray, 512);

        outgoingMsgChar.writeValue(messageByteArray, 512);
    } else {
        if(_verbose) {
            Serial.println(">Sending end message...");
        }

        outgoingMsgChar.writeValue((byte)0x00);

        byte* byteArray = (byte*) outgoingMsgChar.value();

        //printByteArray(byteArray, 1);
    }

    // TODO: Should it be checked if the message queue
    // is empty here?
    if(messageQueue.reachedLastMessage()) {
        allMessagesRead = true;
    }
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

void onIncomingMsgCharWritten(BLEDevice central, BLECharacteristic characterstic)
{
    // Get the char* inside the imchar
    // Call onIncomingMessageReceived(char* msg)

    //characterstic.value();

    if(_verbose) {
        Serial.println(">Message chunk received");

        char *messageChar = (char *)characterstic.value();

        Serial.print(">Message chunk: ");
        Serial.println(messageChar);

        /*Serial.print(">Characteristic size: ");
        Serial.println(characterstic.valueSize());*/
    }

    byte* messageChunkByteArray = (byte*) characterstic.value();

    short chunkLength;
    memcpy(&chunkLength, messageChunkByteArray, sizeof(short));

    if(chunkLength == 0) {
        // All of the message's chunks have been received. It's time to build the message
        memcpy(_currentIncomingMessageByteArray, 
        &_currentIncomingMessageLength, 
        sizeof(short));

        Message message = Message(_currentMessageByteArray);

        _currentIncomingMessageChunkIndex = 0;
        _currentIncomingMessageLength = 0;

        if(_verbose) {
            char messageChar[512];
            message.toCharArray(messageChar);

            Serial.print(">Complete message received: ");
            Serial.println(messageChar);
        }

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

    memcpy(_currentIncomingMessageByteArray + (_currentIncomingMessageChunkIndex == 0 ? sizeof(short) : _currentIncomingMessageChunkIndex * 20), 
    messageChunkByteArray + sizeof(short), 
    chunkLength);

    _currentIncomingMessageLength += chunkLength;
    
    if(_verbose) {
        char *messageChar = (char *)characterstic.value();

        Serial.print(">Message: ");
        Serial.println(messageChar);

        Serial.print(">Characteristic size: ");
        Serial.println(characterstic.valueSize());
    }

}