
//#define USE_TESTMAIN 1
//#define USE_WEDGED_MAIN 1
#define USE_POWERMANAGED_MAIN 1
//#define USE_SNIFFER_MAIN 1



#ifdef USE_TESTMAIN
// Test without real app
void testMain();
int main() {testMain();}
#endif


#ifdef USE_WEDGED_MAIN
void wedgedMain();
int main() {wedgedMain();}
#endif

#ifdef USE_POWERMANAGED_MAIN
void powerManagedMain();
int main() {powerManagedMain();}

#endif


#ifdef USE_SNIFFER_MAIN
void snifferMain();
int main() {snifferMain();}
#endif
