

// lkk hack, this should be in app_config.h?
// It is not clear whether this should be derived from a master template??


// Because app_timer uses clock
#define CLOCK_ENABLED 1
#define CLOCK_CONFIG_LF_SRC 1	// xtal
#define CLOCK_CONFIG_IRQ_PRIORITY 6

// My app uses app_timer.
// Project component app_timer.c includes this file and compiles conditionally on this definition
#define APP_TIMER_ENABLED 1
