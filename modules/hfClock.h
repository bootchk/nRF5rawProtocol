
/*
 * The HF (16Mhz) clock.
 * Required by radio.
 * Might be used by other parts of system (not exclusive to radio peripheral.)
 * ??? What is the mcu clock?
 */

class HfClock {
public:
	static void start();
	static void stop();
};
