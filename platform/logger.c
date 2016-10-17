
#include "SEGGER_RTT.h"


void initLogging() {
	SEGGER_RTT_Init();
}

void log(const char* aString) {
	SEGGER_RTT_WriteString(0, aString);
}
