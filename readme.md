
Raw Wireless Protocol for nRF5

Work in progress.  Untested on hw.

About
-

Raw :
- broadcast, all units transmit and receive same address
- no channel hopping (use one channel not used by WiFi or BT connections)
- unreliable, no acks
- unbuffered xmit
- buffered receive in a fifo

I.e. simple use of radio peripheral in lower layers of stack, and not much else.

For algorithms that sleep mostly, transmit rarely, and deal w/ contention/noise and reliability in upper layers.

Dev environment
-

An Eclipse project.
Uses gcc ARM toolchain.
Uses a hand maintained Makefile.
Many resources are linked (to the NRF SDK.)
I followed the tutorial for this combination on Nordic website, and hacked the Makefile.

Derives from:
 ble-mesh repository on github (it implements a rawish protocol that cooperates with a BT protocol.)
 Nordic proprietary (sic?) protocol ESB examples in NRF SDK (simple protocol, but uses acks.)
 
Hacking
-

Download the NRF SDK.
Hack the Makefile variable NRF_SDK_ROOT to point to it.
Hack the Makefile to add any source files you add.
Fix any other problems in the project, such as Properties>Resources>Linked Resources?
 
