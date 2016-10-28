
#include <inttypes.h>
#include <stdarg.h>			// variadic

#include "SEGGER_RTT.h"


#ifdef LOGGING

void initLogging() {
	SEGGER_RTT_Init();
}

void log(const char* aString) {
	SEGGER_RTT_WriteString(0, aString);
}


void logLongLong(uint64_t value ){
	// Print 64-bit int as two uint32-t on same line, hex notation
	// TODO this should work, but it doesn't????
	//(void) SEGGER_RTT_printf(0, "%x %x\n", *(((uint32_t*) &value) + 1), value);

	// Print pieces on separate lines
	(void) SEGGER_RTT_printf(0, "%x \n", value);
	(void) SEGGER_RTT_printf(0, "%x \n", *(((uint32_t*) &value) + 1)  );
}

#else

void initLogging() {}
void log(const char* aString) {}
void logLongLong(uint64_t value ){}


#endif


#ifdef FUTURE
??? this should work
		// Not exposed by SEGGER_RTT.h
		int SEGGER_RTT_vprintf(unsigned BufferIndex, const char * sFormat, va_list * pParamList);


// Adapt variadic to SEGGER vprintf
void logf(const char* formatString, ...) {
	va_list argp;
	va_start(argp, formatString);
	(void) SEGGER_RTT_vprintf(0, formatString, &argp);
	va_end(argp);
}

#endif
