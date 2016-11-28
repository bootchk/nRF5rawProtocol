This directory: code to handle exceptions/IRQs.

All exceptions/IRQs have default handlers that just enter an infinite loop.
The default interrupt vectors point to the default handlers.
The default handlers are weak, meaning you override them by defining a handler of the same name.
You must define the handlers to use C linkage, when compiling with C++.

Many exceptions point to the same handler.
To tell which exception/IRQ actually occurred, it is easiest to define separate handlers.
That is what irqHandlers.c does: define a handler for the WatchDog IRQ.
(But it is unneccessary, since the watchdog is disabled by default and I never saw it called.)

Some might be cruft: hardFaultHandler.s  ?

appErrorFaultHandler redefines the default app_error_fault_handler().
That is called for app errors and when assert() evaluates to false.
Redefine so behaviour if DEBUG is: sleep in low power mode, never to wake up.

When the power supply is ultra-low power,
the default handlers (that infinite loop) can easily exhaust the power supply
(since the mcu draws 6mA while spinning)
and then a brownout can occur and the app restart.
Thus it might be hard to tell that an exception occurred.





