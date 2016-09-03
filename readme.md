
Raw Wireless Protocol for nRF5

Work in progress.  Untested on hw.

About
-

A primitive (raw) protocol stack for Nordic nRF52 family radio chips (SoC which includes ARM mcu)

Raw :
- broadcast, all units transmit and receive same address
- no channel hopping (use one channel not used by WiFi or BT connections)
- unreliable, no acks
- unbuffered xmit and rcv

I.e. simple use of radio peripheral in lower layers of stack, and not much else.

For algorithms that sleep mostly, transmit rarely, and deal w/ contention/noise and reliability in upper layers.

Dev environment
-

Uses:
- Eclipse project.
- gcc ARM toolchain.
- hand maintained Makefile.
- linked resources (to the NRF SDK.)
I followed the tutorial for this combination on Nordic website, and hacked the Makefile.

Derives from:
- ble-mesh repository on github (it implements a rawish protocol that cooperates with a BT protocol.)
- Nordic proprietary (sic?) protocol ESB examples in NRF SDK (simple protocol, but uses acks.)
 
 Compatibility
 -
 
 Uses nRF52 family chip.
 Might be compatible with earlier chips nRF51, but I noticed in passing that the state diagram for the radio has changed and I have not explored this thoroughly.
 
Hacking
-
Follow the tutorial on the Nordic website.  https://devzone.nordicsemi.com/tutorials/7/  This is a brief synopsis.

Install Eclipse C/C++ and GNU dev tools like make.

Download the NRF SDK.  Create a folder somewhere and extract it into the folder (unlike many archives, it is not one folder at the top.)

Get GNU ARM toolchain: sudo apt-get install  gcc-arm-none-eabi

Get Eclipse plugin for GNU ARM development.  http://gnuarmeclipse.github.io/plugins/install/  (Its slow to resolve, provision.)

Install a device pack for nRF52  (else in building: "core_cm4.h: No such file or directory" )

Per the tutorial, hack Makefile.posix in the NRF SDK (ugh) to point to your local GNU ARM installation.  For me this just meant:  GNU_INSTALL_ROOT := /usr/bin
(In other words, Makefile.posix just tells the Makefile how to construct the path to the GNU ARM compiler executable.)

Hack the Makefile variable NRF_SDK_ROOT to point to it.

Hack the Makefile to add any source files you add.

Hack the Makefile to fix filename capitalization issues in the NRF SDK v12 (see Nordic DevZone forum)

Fix any other problems in the project, such as Properties>Resources>Linked Resources?
 
