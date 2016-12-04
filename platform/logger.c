
#include <inttypes.h>
#include <stdarg.h>			// variadic

#include "SEGGER_RTT.h"


// may be defined in Makefilenrf52 -DLOGGING
#ifdef LOGGING

void initLogging() {
	SEGGER_RTT_Init();
}

void log(const char* aString) {
	SEGGER_RTT_WriteString(0, aString);
}

void logByte(uint8_t value){
	(void) SEGGER_RTT_printf(0, "x%02x\n", value);
}

void logInt(uint32_t value){
	(void) SEGGER_RTT_printf(0, "%u", value);
}

void logLongLong(uint64_t value ){
	// Print 64-bit int as two uint32-t on same line, hex notation
	// FUTURE this should work, but it doesn't????
	//(void) SEGGER_RTT_printf(0, "%x %x\n", *(((uint32_t*) &value) + 1), value);

	// Print pieces on separate lines
	//(void) SEGGER_RTT_printf(0, "%x \n", value);
	//(void) SEGGER_RTT_printf(0, "%x \n", *(((uint32_t*) &value) + 1)  );

	// Print on one line
	(void) SEGGER_RTT_printf(0, "x%04x", *(((uint32_t*) &value) + 1)  );	// MS word
	(void) SEGGER_RTT_printf(0, "%04x\n", value);	// LS word and newline

}

#else

void initLogging() {}
void log(const char* aString) { (void) aString; }
void logInt(uint32_t value ){ (void) value; }
void logLongLong(uint64_t value ){ (void) value; }


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
