
#define USE_TESTMAIN 1





#ifdef USE_TESTMAIN

// Test without real app
void testMain();

int main() {
	testMain();
}

#else

void wedgedMain();

int main() {
	// continues in external library
	wedgedMain();
}

#endif
