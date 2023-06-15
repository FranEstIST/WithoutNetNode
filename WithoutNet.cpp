#include "WithoutNet.h"
#include "UUIDs.h"
#include <ArduinoBLE.h>
#include <C:\Users\Francisco\Documents\Arduino\libraries\UUID\UUID.h>
#include <list>

//BLEService WNService("b19fbebe-dbd4-11ed-afa1-0242ac120002");

class WithoutNet {
    public:
        static WithoutNet* WN; 

        WithoutNet(char* uuid, char* localName)
        : WNService(WNSERVICE_UUID),
        outgoingMsgChar(OUTGOING_MSG_CHAR_UUID, BLERead, 120),
        incomingMsgChar(INCOMING_MSG_CHAR_UUID, BLERead | BLEWrite, 120),
        localName(localName),
        uuid(uuid) {
            /*uuid = new char[48];
            BLE.address().toCharArray(uuid, 48);*/
            WithoutNet* WN = this;
            //this -> outgoingMsgChar.setEventHandler(BLERead, moveToNextMsg);
            incomingMessageHandler = nullptr;
        }

        int begin() {
            // Begin initialization
            // TODO: WN::Begin should be called in a loop and not
            // BLE::Begin
            if (!BLE.begin()) {
              return 0;
            }

            BLE.setLocalName(localName);
            BLE.setAdvertisedService(WNService); // Add the service UUID

            // Add the outgoing message characteristic (where messages sent from this
            // device are written to)
            WNService.addCharacteristic(outgoingMsgChar);

            // Add the incoming message characteristic (where messages sent to this
            // device are written to)
            WNService.addCharacteristic(incomingMsgChar);

            incomingMsgChar.setEventHandler(BLEWritten, onIncomingMsgCharWritten);

            //outgoingMsgChar.setEventHandler(BLERead, moveToNextMsg);
            //BLE.setEventHandler(BLEConnected, ...);
            //BLE.setEventHandler(BLEDisconnected, ...);

            BLE.addService(WNService); // Add the WN Service

            BLE.advertise();

            return 1;
        }
        
        void send(char* msg, char* destUuid) {
            // Build message, with the header attached at the beginning (len + ts + sender uuid + dest uuid + msg)
            char timestampStr[10];
            char messageStr[10];
            sprintf(timestampStr, "%d", millis());
            sprintf(messageStr, "%d", msg);

            char updateStrSecondHalf[120];
            strcpy(updateStrSecondHalf, timestampStr);
            // TODO: Temporary fix. This char shouldn't be necessary since every field in the update should be of fixed len
            // (fix this using byte arrays and memcpy)
            strcat(updateStrSecondHalf, separator);
            strcat(updateStrSecondHalf, uuid);
            strcat(updateStrSecondHalf, separator);
            strcat(updateStrSecondHalf, destUuid);

            char updateLenStr[10];  
            int updateLen = strlen(updateStrSecondHalf);
            sprintf(updateLenStr, "%d", updateLen);

            char updateStr[130];
            strcpy(updateStr, updateLenStr);
            strcat(updateStr, separator);
            strcat(updateStr, updateStrSecondHalf);

            // Write message to the outgoingMsgChar
            outgoingMsgChar.writeValue(updateStr);
        }

        void queueMsg(char* msg, char* destUuid) {

        }

        void setMaxPendingMsgs(int size) {
        }

        void getPendingMsgNum() {

        }

        static void onIncomingMessageReceived(char* msg) {

        }

        void setIncomingMessageHandler(IncomingMessageHandler incomingMessageHandler) {
            this->incomingMessageHandler = incomingMessageHandler;
        }

    private:
        int messageCounter = 0;

        char* uuid;
        char* localName;

        BLEService WNService;
        BLEStringCharacteristic outgoingMsgChar;
        BLEStringCharacteristic incomingMsgChar;

        static IncomingMessageHandler incomingMessageHandler;

        char* separator = "#";

        std::list<char*> msgList;
        int msgPointer = 0;

        void moveToNextMsg(BLEDevice central, BLECharacteristic characterstic) {
            msgPointer = 1 + msgPointer;
        }
        
        char* generateMessageUuid() {
            UUID uuid;
            uuid.generate();
            return uuid.toCharArray();  
        }

        static void onIncomingMsgCharWritten(BLEDevice central, BLECharacteristic characterstic) {
            // Get the char* inside the imchar
            // call onIncomingMessageReceived(char* msg)
            
            char* msg = (char*)characterstic.value();

            incomingMessageHandler(msg);
        }

        void dequeueMsg(char* msgUuid);
};