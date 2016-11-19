
//#define USE_TESTMAIN 1
//#define USE_WEDGED_MAIN 1
#define USE_POWERMANAGED_MAIN 1



#ifdef USE_TESTMAIN

// Test without real app
void testMain();

int main() {
	testMain();
}
#endif


#ifdef USE_WEDGED_MAIN

void wedgedMain();

int main() {
	// continues in external library
	wedgedMain();
}

#endif

#ifdef USE_POWERMANAGED_MAIN

void powerManagedMain();

int main() {
	// continues in external library
	powerManagedMain();
}

#endif
