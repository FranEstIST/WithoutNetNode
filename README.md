# WithoutNet Arduino Library

## About

This is the Arduino library which enables BLE-capable Arduino boards to communicate using WithoutNet.

## How to Install

To install this Arduino library, simply download the source code for this library, and then, in your Arduino IDE, go to `sketch` > `Include Library` > `Add .ZIP Library...`, and select this library's zip file.
To include it in your Arduino code, simply write `#include <WithoutNet.h>` at the beginning of the file.

## How to Use the WithoutNet Library

To initialize WithoutNet, simply run in the `setup` block:
<code>
while(!begin(&lt;node-id&gt, &lt;name&gt;, &lt;timestamp-offset&gt;, &lt;is-verbose&gt;));
</code>

To send a message, call either:

<ol>
  <li><code>sendInt(&lt;int-message&gt;, &lt;dest-node-id&gt;)</code></li>
  <li><code>send(&lt;byte-array-message&gt;, &lt;dest-node-id&gt;)</code></li>
</ol>

To set the routine for handling incoming messages, run `setIncomingMessageHandler(<routine>)` in the `setup` block.
