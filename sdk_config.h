

// lkk hack, this should be in app_config.h?
// It is not clear whether this should be derived from a master template??


// Because app_timer uses clock
#define CLOCK_ENABLED 1
#define CLOCK_CONFIG_LF_SRC 1	// xtal
#define CLOCK_CONFIG_IRQ_PRIORITY 3
// TODO priority 6 for nrf52?

// My app uses app_timer.
// Project component app_timer.c includes this file and compiles conditionally on this definition
#define APP_TIMER_ENABLED 1

#ifdef FUTURE
// Because using NRF_LOG
// older SDK #define UART0_ENABLED 1
//#define USE_UART 1
#define NRF_LOG_ENABLED 1
// apparently not in sdk12 #define NRF_LOG_USES_UART = 1
#define NRF_LOG_USES_RTT = 0
//#define NRF_LOG_USES_TIMESTAMP 0
//#define NRF_LOG_DEFERRED 0
//#define NRF_LOG_BACKEND_MAX_STRING_LENGTH 64
//#define NRF_LOG_TIMESTAMP_DIGITS 12
#endif
//#define NRF_LOG_ENABLED 1
//#define NRF_LOG_USES_RTT 1



