/*
 * Choose one at compile time:
#define USE_TESTMAIN 1
#define USE_WEDGED_MAIN 1
#define USE_POWERMANAGED_MAIN 1
#define USE_SNIFFER_MAIN 1
 */
#define USE_POWERMANAGED_MAIN 1



void testMain();
void wedgedMain();
void powerManagedMain();
void snifferMain();

int main() {

#ifdef USE_TESTMAIN
	// Test without real app
	testMain();
#endif


#ifdef USE_WEDGED_MAIN
	wedgedMain();}
#endif

#ifdef USE_POWERMANAGED_MAIN
powerManagedMain();
#endif


#ifdef USE_SNIFFER_MAIN
snifferMain();
#endif


}  // main
