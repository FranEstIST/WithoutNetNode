#include "WithoutNet.h";

#include <Arduino.h>
#include <ArduinoBLE.h>

#include "MessageQueue.h"
// #include "UUIDs.h"

const char *WNSERVICE_SIMPLE_UUID = "b19fbebe-dbd4-11ed-afa1-0242ac120002";
const char *INCOMING_MSG_CHAR_SIMPLE_UUID =
    "75c57d2e-7efe-4d7b-af73-e2835f2d48d4";
const char *OUTGOING_MSG_CHAR_SIMPLE_UUID =
    "dbe60c86-e91d-11ed-a05b-0242ac120003";
const char *NODE_UUID_CHAR_UUID = "89954326-56e6-4797-b62f-07ac5c4c3789";

char uuid[37];
char localName[33];

int id;

BLEService WNService(WNSERVICE_SIMPLE_UUID);

BLEIntCharacteristic nodeUuidChar(NODE_UUID_CHAR_UUID, BLERead);
BLECharacteristic outgoingMsgChar(OUTGOING_MSG_CHAR_SIMPLE_UUID, BLERead, 20);
BLECharacteristic incomingMsgChar(INCOMING_MSG_CHAR_SIMPLE_UUID,
                                  BLERead | BLEWrite, 20);

IncomingMessageHandler incomingMessageHandler;

long messageCounter = 0;
MessageQueue messageQueue = MessageQueue(10);

char *_separator = "#";
bool allMessagesRead = false;

long _timestampOffset = 0;

bool _verbose = false;

struct {
    byte messageByteArray[512];
    int currentChunkIndex = 0;
    short messageLength = 0;
} _incomingMessageChunks, _outgoingMessageChunks;

int begin(int idPrime, char *localNamePrime, long timestampOffset,
          bool verbose) {
    incomingMessageHandler = nullptr;

    id = idPrime;

    memcpy(localName, localNamePrime, strlen(localNamePrime) + 1);

    _timestampOffset = timestampOffset;

    _verbose = verbose;

    // Begin initialization
    if (!BLE.begin()) {
        return 0;
    }

    if (_verbose) {
        Serial.println("--WithoutNet node started--");

        Serial.print(">ID: ");
        Serial.println(id);

        Serial.print(">Local name: ");
        Serial.println(localName);
    }

    pinMode(LED_BUILTIN, OUTPUT);  // initialize the built-in LED pin to
                                   // indicate when a central is connected

    BLE.setLocalName(localName);
    BLE.setAdvertisedService(WNService);  // Add the service UUID

    // Add the node uuid characteristic
    WNService.addCharacteristic(nodeUuidChar);

    // Add the outgoing message characteristic (where messages sent from this
    // device are written to)
    WNService.addCharacteristic(outgoingMsgChar);

    // Add the incoming message characteristic (where messages sent to this
    // device are written to)
    WNService.addCharacteristic(incomingMsgChar);

    // WNService.hasCharacteristic()

    nodeUuidChar.writeValue(id);

    byte emptyMessageByteArray[20];

    for (int i = 0; i < 20; i++) {
        emptyMessageByteArray[i] = 0;
    }

    outgoingMsgChar.writeValue(emptyMessageByteArray, 20);
    incomingMsgChar.writeValue(emptyMessageByteArray, 20);

    incomingMsgChar.setEventHandler(BLEWritten, onIncomingMsgCharWritten);
    outgoingMsgChar.setEventHandler(BLERead, moveToNextMsg);

    BLE.setEventHandler(BLEConnected, onConnected);
    BLE.setEventHandler(BLEDisconnected, onDisconnected);

    BLE.addService(WNService);  // Add the WN Service

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

void runLoop() { BLEDevice central = BLE.central(); }

void sendInt(int msg, int destId) {
    if (_verbose) {
        Serial.print(">Message to be added to the message queue: ");
        Serial.println(msg);
    }

    short payloadLength = sizeof(int);
    byte *payload = (byte *)&msg;

    addMessageToQueue(payload, payloadLength, destId);
}

void sendCharArray(char *msg, int destId) {
    if (_verbose) {
        Serial.print(">Message to be added to the message queue: ");
        Serial.println(msg);
    }

    short payloadLength = strlen(msg) + 1;
    byte *payload = (byte *)msg;

    addMessageToQueue(payload, payloadLength, destId);
}

void addMessageToQueue(byte *payload, short payloadLength, int destId) {
    Message message = Message(millis() + _timestampOffset, (MessageType)DATA,
                              id, destId, payload, payloadLength);

    char fullMessage[512];
    message.toCharArray(fullMessage);

    messageQueue.addMessage(message);

    messageCounter++;

    if (_verbose) {
        Serial.print(">Message added to the message queue: ");
        Serial.println(fullMessage);
    }

    byte messageByteArray[512];
    message.toByteArray(messageByteArray);
}

void setMaxPendingMsgs(int size) {
    // TODO
}

void getPendingMsgNum() {
    // TODO
}

void setIncomingMessageHandler(
    IncomingMessageHandler incomingMessageHandlerPrime) {
    incomingMessageHandler = incomingMessageHandlerPrime;
}

void moveToNextMsg(BLEDevice central, BLECharacteristic characterstic) {
    writeNextChunk();
}

void onConnected(BLEDevice central) {
    if (_verbose) {
        Serial.print("//------\\\\ Connected to smartphone with address: ");
        Serial.print(central.address());
        Serial.println(" //------\\\\");
    }

    digitalWrite(LED_BUILTIN, HIGH);
    resetMessagePointer();
}

void onDisconnected(BLEDevice central) {
    if (_verbose) {
        Serial.print(
            "\\\\------// Disconnected from smartphone with address: ");
        Serial.print(central.address());
        Serial.println(" \\\\------//");
    }

    digitalWrite(LED_BUILTIN, LOW);
}

void resetMessagePointer() {
    _outgoingMessageChunks.currentChunkIndex = 0;
    _incomingMessageChunks.currentChunkIndex = 0;

    _outgoingMessageChunks.messageLength = 0;
    _incomingMessageChunks.messageLength = 0;

    if (!messageQueue.isEmpty()) {
        messageQueue.moveToStart();
    }
}

void writeNextChunk() {
    byte messageChunkByteArray[20];
    short messageChunkLength = 0;

    // Init message chunk byte array to 0's,
    // so that the outgoing message characteristic
    // is 20 byte long and has no junk values at
    // the end
    for (int i = 0; i < 20; i++) {
        messageChunkByteArray[i] = 0x0;
    }

    if (!messageQueue.isEmpty()) {
        if (_outgoingMessageChunks.currentChunkIndex == 0) {
            Message outgoingMessage = messageQueue.getNextMessage();
            outgoingMessage.toByteArray(
                _outgoingMessageChunks.messageByteArray);
            _outgoingMessageChunks.messageLength = outgoingMessage.getLength();

            if (_verbose) {
                char currentMessageCharArray[512];
                outgoingMessage.toCharArray(currentMessageCharArray);

                Serial.print(">Next message in Outgoing Msg Char: ");
                Serial.println(currentMessageCharArray);
            }
        }

        short remainingMessageLength =
            _outgoingMessageChunks.messageLength -
            18 * _outgoingMessageChunks.currentChunkIndex;
        messageChunkLength =
            18 > remainingMessageLength ? remainingMessageLength : 18;
    }

    if (messageChunkLength <= 0) {
        // All chunks have been read by the client, now the end message must be
        // sent

        messageChunkByteArray[0] = 0x0;
        messageChunkByteArray[1] = 0x0;

        if (messageQueue.isEmpty() || messageQueue.reachedLastMessage()) {
            if (_verbose) {
                Serial.println(">Sending \"end of message queue\" message...");
            }

            messageChunkByteArray[2] = 0x1;
            messageChunkByteArray[3] = 0x0;
        } else {
            if (_verbose) {
                Serial.println(">Sending \"end of message\" message...");
            }

            messageChunkByteArray[2] = 0x00;
            messageChunkByteArray[3] = 0x0;
        }

        _outgoingMessageChunks.currentChunkIndex = 0;
    } else {
        short messageOffset = 2 + 18 * _outgoingMessageChunks.currentChunkIndex;

        memcpy(messageChunkByteArray, &messageChunkLength, sizeof(short));
        memcpy(messageChunkByteArray + sizeof(short),
               _outgoingMessageChunks.messageByteArray + messageOffset,
               messageChunkLength);

        _outgoingMessageChunks.currentChunkIndex++;
    }

    if (_verbose) {
        if (_outgoingMessageChunks.currentChunkIndex > 0) {
            Serial.print(">Sending chunk ");
            Serial.println(_outgoingMessageChunks.currentChunkIndex);
        }
        printByteArrayCompact(
            messageChunkByteArray,
            messageChunkLength <= 0 ? 4 : messageChunkLength + 2);
    }

    // Write the message chunk or end message to the outgoing message char
    outgoingMsgChar.writeValue(messageChunkByteArray, 20);
}

void printByteArray(byte *byteArray, int size) {
    if (_verbose) {
        Serial.println("Byte array: ");

        for (int i = 0; i < size; i++) {
            Serial.print("Index ");
            Serial.print(i);
            Serial.print(": ");
            Serial.println((int)byteArray[i]);
        }
    }
}

void printByteArrayCompact(byte *byteArray, int size) {
    if (_verbose) {
        Serial.println("Byte array: ");

        for (int i = 0; i < size; i++) {
            Serial.print((int)byteArray[i]);
            Serial.print("#");
        }

        Serial.println("");
    }
}

void onIncomingMsgCharWritten(BLEDevice central,
                              BLECharacteristic characterstic) {
    if (_verbose) {
        Serial.print(">Message chunk ");
        Serial.print(_incomingMessageChunks.currentChunkIndex + 1);
        Serial.print(" received: ");

        printByteArrayCompact((byte *)characterstic.value(), 20);
    }

    byte *messageChunkByteArray = (byte *)characterstic.value();

    short messageChunkLength;
    memcpy(&messageChunkLength, messageChunkByteArray, LENGTH_SIZE);

    if (messageChunkLength == 0) {
        // All of the message's chunks have been received. It's time to build
        // the message

        // Add the total message length to the start of the message
        memcpy(_incomingMessageChunks.messageByteArray,
               &_incomingMessageChunks.messageLength, LENGTH_SIZE);

        Message message = Message(_incomingMessageChunks.messageByteArray);

        if (_verbose) {
            char messageChar[512];
            message.toCharArray(messageChar);

            Serial.print(">Complete message received: ");
            Serial.println(messageChar);
        }

        _incomingMessageChunks.messageLength = 0;
        _incomingMessageChunks.currentChunkIndex = 0;

        if (message.getType() == ACK) {
            messageQueue.removeMessage(message.getTimestamp());
        } else {
            if (incomingMessageHandler != nullptr) {
                incomingMessageHandler(message);
            }
        }

        return;
    }

    // Add the current chunk to the rest of the incoming message
    memcpy(_incomingMessageChunks.messageByteArray + LENGTH_SIZE +
               _incomingMessageChunks.currentChunkIndex * 18,
           messageChunkByteArray + LENGTH_SIZE, messageChunkLength);

    _incomingMessageChunks.messageLength += messageChunkLength;
    _incomingMessageChunks.currentChunkIndex++;
}