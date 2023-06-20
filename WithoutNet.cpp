#include "WithoutNet.h"
#include "UUIDs.h"
#include "MessageQueue.h"
#include <C:\Users\Francisco\Documents\Arduino\libraries\UUID\UUID.h>
#include <list>

class WithoutNet {
    public:
        WithoutNet(char* uuid, char* localName)
        : _WNService(WNSERVICE_UUID) {
            /*BLE.address().toCharArray(uuid, 48);*/
            // TODO: Check if the uuid and local name are valid

            _incomingMessageHandler = nullptr;

            memcpy(_localName, localName, sizeof(localName)/sizeof(char));
            memcpy(_uuid, uuid, sizeof(uuid)/sizeof(char));

            _outgoingMsgChar = BLEStringCharacteristic(OUTGOING_MSG_CHAR_UUID, BLERead, 120);
            _incomingMsgChar = BLEStringCharacteristic(INCOMING_MSG_CHAR_UUID, BLERead | BLEWrite, 120);
        }

        int begin() {
            // Begin initialization
            if (!BLE.begin()) {
              return 0;
            }

            BLE.setLocalName(_localName);
            BLE.setAdvertisedService(_WNService); // Add the service UUID

            // Add the outgoing message characteristic (where messages sent from this
            // device are written to)
            _WNService.addCharacteristic(_outgoingMsgChar);

            // Add the incoming message characteristic (where messages sent to this
            // device are written to)
            _WNService.addCharacteristic(_incomingMsgChar);

            _incomingMsgChar.setEventHandler(BLEWritten, onIncomingMsgCharWritten);
            _outgoingMsgChar.setEventHandler(BLERead, moveToNextMsg);

            //BLE.setEventHandler(BLEConnected, ...);
            BLE.setEventHandler(BLEConnected, resetMessagePointer);

            BLE.addService(_WNService); // Add the WN Service

            BLE.advertise();

            return 1;
        }
        
        void send(char* msg, char* destUuid) {
            Message message = Message(_messageCounter, (MessageType) DATA, millis(), _uuid, destUuid, msg);
            _messageQueue.addMessage(message);
        }

        void setMaxPendingMsgs(int size) {
            // TODO
        }

        void getPendingMsgNum() {
            // TODO
        }

        void setIncomingMessageHandler(IncomingMessageHandler incomingMessageHandler) {
            _incomingMessageHandler = incomingMessageHandler;
        }

    private:
        char _uuid[37];
        char _localName[33];

        BLEService _WNService;
        static BLEStringCharacteristic _outgoingMsgChar;
        static BLEStringCharacteristic _incomingMsgChar;

        static IncomingMessageHandler _incomingMessageHandler;

        long _messageCounter = 0;

        static MessageQueue _messageQueue;

        char* separator = "#";
        
        static void moveToNextMsg(BLEDevice central, BLECharacteristic characterstic) {
            Message message = _messageQueue.getNextMessage();
            
            //Write message to outgoing message char
            char messageCharArray[512];
            message.toCharArray(messageCharArray); 
            _outgoingMsgChar.setValue(messageCharArray);
        }

        static void resetMessagePointer(BLEDevice central) {
            _messageQueue.moveToStart();

            Message message = _messageQueue.getNextMessage();
            
            //Write message to outgoing message char
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

        static void onIncomingMsgCharWritten(BLEDevice central, BLECharacteristic characterstic) {
            // Get the char* inside the imchar
            // Call onIncomingMessageReceived(char* msg)
            
            char* messageChar = (char*)characterstic.value();

            Message message = Message(messageChar);

            if(message.getType() == ACK) {
                // TODO: Check if content is a valid message ID
                _messageQueue.removeMessage(atol(message.getContent()));
            } else {
                _incomingMessageHandler(message);
            }
        }

        void dequeueMsg(char* msgUuid);
};