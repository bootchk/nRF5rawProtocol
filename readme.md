
Raw Wireless Protocol for nRF5

Status: Works, but changes still in progress.  Usually compiles cleanly.  Tested on nrf52DK and RedBear BLE Nano.   Currently the two communicate, but there seem to be many CRC errors.

About
-

A primitive (raw) protocol stack for Nordic nRF52 family radio chips (SoC which includes ARM mcu)

Raw :
- broadcast, all units transmit and receive same address
- no channel hopping (use one channel not used by WiFi or BT connections)
- unreliable datagrams, no acks
- unbuffered (no queue) xmit and rcv

I.e. simple use of radio peripheral in Physical layer of protocol stack, and not much else.
No transport layer: connections, reliability, or flow control

For algorithms that sleep mostly, transmit rarely, and deal w/ contention/noise and reliability in upper layers.

Some emphasis on minimizing power, sleeping often.  My goal is to test sleep synchronization algorithms.

See also
-

Nordic documentation for radio peripheral.  See Nordic InfoCenter>nRF52 Series>nRF52832>Product Specification>RADIO

Radiohead.  That does the same thing except it requires polling, doesn't use interrupts?

https://github.com/NordicSemiconductor/nRF51-ble-bcast-mesh . That implements a rawish protocol that cooperates with a BT protocol.  see nRF51/rbc_mesh/src/radio_control.c
Here, I have hacked out the SUntested on hw.  oftDevice (BT stack) and the Trickle algorithm, leaving just a raw wireless protocol.

Nordic proprietary (sic?) protocol ESB examples in NRF SDK (simple protocol, but uses acks.)  That is interesting because it seems to use the PPI device to transmit an ack automagically when a message is received.

Nordic example for receiving.  Source in SDK below /examples/peripheral/radio/receiver    http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.sdk5.v11.0.0%2Fnrf_dev_radio_rx_example.html&cp=4_0_0_4_4_17

Nordic example for transmitting.  Near the receiving example.

Nordic example for configuring radio compatible with ShockBurst.  In SDK at /components/drivers_nrf/radio_config

My blog about setting up a dev environment (for this project) https://plashless.wordpress.com/2016/09/14/setting-up-linuxeclipse-for-nordic-embedded-wireless-development/

Dev environment
-

Uses:
- Eclipse project.
- GNU ARM toolchain (gcc and gdb for ARM)
- hand maintained Makefile (as do most Nordic examples?)
- linked resources (to the NRUntested on hw.  F SDK, as do most Nordic examples?)
I followed the tutorial for this combination on Nordic website, and hacked the Makefile.


 
Compatibility
-
 
Uses nRF52 family chip.
Might be compatible with earlier chips nRF51, but I noticed in passing that the state diagram for the radio has changed and I have not explored this thoroughly.
 
Developed using NRF SDK v11, and then v12.

C++11
-
This code is C++.  Nordic doesn't fully support it.  It requires a few hacks to the SDK:

In ??? declare the POWER_CLOCK_IRQ handler as extern "C" so its name is not mangled and it overrides the default handler.

For the same reasons, in app_timer.c (because app_timer uses two interrupts, RTC1 and SWI0):

    #ifdef __cplusplus 
    extern "C" { 
    #endif
    void SWI0_IRQHandler(void);
    void RTC1_IRQHandler(void);
    #ifdef __cplusplus 
    } 
    #endif

 
Hacking using Eclipse and GNU ARM toolchain
-
Follow the tutorial on the Nordic website.  https://devzone.nordicsemi.com/tutorials/7/  

See my blog about my experience setting up the toolchain (link above under See Also.)


Follows a brief synopsis.

Install Eclipse C/C++ and GNU dev tools such as make.

Download the NRF SDK.  Create a folder somewhere and extract it into the folder (unlike many archives, it is not one folder at the top.)

Get GNU ARM toolchain: sudo apt-get install  gcc-arm-none-eabi

Get Eclipse plugin for GNU ARM development.  http://gnuarmeclipse.github.io/plugins/install/  (Its slow to resolve, provision.)

Install a device pack for nRF52

Per the tutorial, hack Makefile.posix in the NRF SDK (ugh) to point to your local GNU ARM installation.  For me this just meant:  GNU_INSTALL_ROOT := /usr/bin
(In other words, Makefile.posix just tells the Makefile how to construct the path to the GNU ARM compiler executable.)

Hack the Makefile variable NRF_SDK_ROOT to point to it.

Hack the Makefile to add any source files you add.

Hack the Makefile to fix filename capitalization issues in the NRF SDK v12 (see Nordic DevZone forum)

Fix any other problems in the project, such as Properties>Resources>Linked Resources?
 

Chip families and board/modules
-

The project supports and has been tested on chip families and dev boards/modules: 
- nrf51 on RedBear BLE Nano module
- nrf52 on nrf52DK dev board

The custom_board.h for BLE Nano provided by Electronut.

There are two Makefiles, .nrf51 and .nrf52.  I hacked the Makefiles (from the original) mostly in the same way, duplicating hacks.   If you intend to support both families and you add source files or make other changes, make the changes in both Makefiles.

Also two .ld files (to configure different flash/RAM amounts.)

(I haven't figured out whether I need to change the Eclipse projects linked resources.  Apparently the Makefile doesn't use them?)

Also, the LEDLogger class allows for LED differences on boards (if LED's don't exist, calls to toggleLED() have no effect.)

To change: copy one of the Makefiles.nrf5x over the Makefile and rebuild.  (I haven't figured out how to configure Eclipse for two different Makefiles.)

Other hacks:

To allow a large program to load in limited RAM:  In gcc_startup_nrf51.s     =>   .equ    Heap_Size, 0
The project does not use malloc or dynamically allocated objects.

Remember I hacked the SDK, and if you switch nrf52 to nrf51, I repeated some hacks in two places:

- (The hacks to allow C++ IRQHandler are portable nrf52 to nrf51.)

- Capitalization issues gcc_startup_nrf51.S => .s


