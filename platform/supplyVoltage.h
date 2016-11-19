

/*
 * Knows the system power supply voltage.
 */
class SupplyVoltage {
public:
	/*
	 * Returns an unsigned int having this relation to Vdd in volts:
	 *
	 * (returnValue/1024) == (Vdd/3.6)
	 */
	static unsigned int getProportionTo1024();


	bool isVddGreaterThan2_2();


};
