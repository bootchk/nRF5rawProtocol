
These are test cases using four LEDs on an nrf52DK and some other unit.

The payload of most tests is some constant array of five bytes e.g. 1,3,4,5,7


Test Cases using testMain.c
=

The testMain has an event loop, that:

    sends
    sleeps while receiving
    sleeps with radio off
    
The duration of the event loop is irregular, depending on whether a message is heard to short then sleep with receiver on.

LED meaning:

    1 toggles every event loop
    2 toggles on every receive
    3 toggles on valid receive
    4 toggles on invalid receive
    
If in any test, LED1 stops toggling, the unit has halted for an exception.
    
Solitary unit
-

One unit without another unit in range.

Expect:

    LED1 toggles regularly, on, off, on, off, ..
    No other LED toggles (nothing received)
    
Two identical units
-

Two units in range of each other, identically configured.

Expect:
 
    LED1 of each unit toggles irregularly, but the two LED1's not necessarily in unison
    On a unit, LED2 and LED3 toggle in unison, irregularly (when the other unit transmits)
    LED4 might toggle rarely (when it hears an invalid transmission)
    

Note that after an invalid transmission LED2 and LED3 should still toggle in unison (on valid receive), but one might be off while the other is on.
    
Two units configured with different address lengths
-

E.g. one unit configured with address length 2 (constant defined in radio.h), other unit with address length 4, with both addresses drawn from say 0xe7e7e7e7, such that one address is a proper substring of the other.

Expect on the longer address unit:

    LED1 toggles regularly, on, off, on, off, ..
    LED2 and LED3 NOT toggle (nothing valid received)
    Almost never, LED2 and LED4 toggle (invalid packet received)
    
Expect on the shorter address unit:
    
    LED1 toggles irregularly
    LED2 and LED4 toggle in unison (invalid packet received, where the radio hears the suffix of the longer address as data, and treats the last two bytes of the data as CRC having a wrong value. 
    Almost never, LED2 and LED3 toggle (valid packet received) (if there were ever a combination of bit errors that made the CRC valid?  i.e. the message is long enough, and the shortening prefix that the radio attempts to validate might occasionally be valid.)
    

Two units configured with undocumented short address lengths
-

This tests that an addres length of two (which Nordic documents as not valid) does indeed work, as reported by others.  Both units configured with address length two (one byte prefix + one byte base.)

Same results as Two Identical Units


Two units configured with whitening on
-

FUTURE: I can't get this to work.  The docs or the example code is not clear about which side needs to configure the whitening seed and whether it derives from the configured channel.


Two units configured with different whitening enabled
-

One unit with whitening enabled, other without.  (This does NOT test different whitening seeds.  I assume that is a rarely used feature.)

(Results similar as shorter address unit of differently configured address lengths: messages received, but always invalid since CRC of unwhitened data differs from CRC of whitened data.)

Expect on the both units: 

    LED1 toggles regularly, on, off, on, off, ..
    LED2 and LED4 toggle in unison (invalid received)
    Almost never, LED2 and LED3 toggle (valid packet received, if enough bit errors occur.)


Test Cases using wedgedMain.c and libsleepSyncAgent.a
=

To build:

    edit main.c to call wedgedMain()
    edit Makefile to have source wedgedMain.c instead of testMain.c
    edit Makefile to define MYLIBS
    
All LED's mean the same as for testMain (they are toggled in nRFrawProtocol, not the library.

The event loop is constant duration.  LED1 toggles regularly, no matter what messages are received.

Test sleep synchronization
-

The duration of the test depends on the duty cycle.  You can shorten the DutyCycleInverse (to say 20) to shorten the test, but then LED's toggle faster.

To set up, repeatedly power on restart one unit until it is out of sync with the other.

Because they both have accurate xtal 32khz clocks, they won't just drift into sync; the algorithm does it.

For these tests, LED4 is also toggled by a transmit (as well as a rare invalid receive.)

Expect initially:

    LED1's of the two units toggle, not in unison
    LED2 and LED3 toggle in unison, almost never (each unit is asleep when the other transmits)
    LED4 almost never toggles (invalid)
    
Expect eventually (say after about half of DutyCycleInverse cycles):

    LED2 and LED3 of one unit toggle in unison (fished sync of the other, or heard mergeSync from other)
    LED1's of the units should toggle in unison
    
Expect subsequently:

    LED1's of the units toggle in unison
    LED2 and LED3 of Slave unit toggle in unison about every 6 cycles (hearing sync from the other unit)
    The Master unit LED4 should toggle about every 6 cycles (sending sync)
   
    
TODO Tests with more than two units
-

