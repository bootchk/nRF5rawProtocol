#pragma once

#include <inttypes.h>

#ifdef FUTURE

// lkk Can't get this to work
#define NRF_LOG_ENABLED 1
#define NRF_LOG_DEFAULT_LEVEL 0
#define NRF_LOG_USES_UART = 1

 /extern "C" {
ret_code_t nrf_log_init(nrf_log_timestamp_func_t timestamp_func);
}

void initLogging(void)
{
    // Initialize logging library.
	__attribute__((unused)) uint32_t err_code = NRF_LOG_INIT((nrf_log_timestamp_func_t) NULL);
    // APP_ERROR_CHECK(err_code);
}

// Macro definitions.
// Leave NRF_LOG macros in place, these define them null ifndef NRF_LOG_ENABLED
#include "nrf_log.h"	// writing to log
#include "nrf_log_ctrl.h"	// initing log

#endif

void initLogging();
void log(const char* aString);
void logByte(uint8_t);
void logInt(uint32_t);
void logLongLong(uint64_t);
// Until I wrap it, use original prototype
//extern "C" {

// !!! Note this does not support ll conversion code
//int SEGGER_RTT_printf(unsigned, const char* formatString, ...);
//}

//void logf(const char* formatString, ...);
