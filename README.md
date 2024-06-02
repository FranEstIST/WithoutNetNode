# WithoutNet Arduino Library

## About

This is the Arduino library which enables BLE-capable Arduino boards to communicate using WithoutNet.

## How to Install

To install this Arduino library, simply download the source code for this library, and then, in your Arduino IDE, go to `sketch` > `Include Library` > `Add .ZIP Library...`, and select this library's zip file.
To include it in your Arduino code, simply write `#include <WithoutNet.h>` at the beginning of the file.

## How to Use the WithoutNet Library

To initialize WithoutNet, simply run in the `setup` block:
<code>
while(!begin(<node-id>, <name>, <timestamp-offset>, <is-verbose>)) {
if(verbose) {
Serial.println("Failed to start WN");
Serial.println("Retrying...");
}
}
</code>

To send a message, call either:

<ol>
  <li>`send(<int-message>, <dest-node-id>)`</li>
  <li>`sendInt(<byte-array-message>, <dest-node-id>)`</li>
</ol>

To set the routine for handling incoming messages, run `setIncomingMessageHandler(<routine>)` in the `setup` block.
