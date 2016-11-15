
#include "nrf.h"
#include "nrf_saadc.h"	// HAL
//#include "nrf_drv_saadc.h"

#include "supplyVoltage.h"

/*
 * Implementation notes:
 *
 * Using HAL (but slightly flawed in SDK12, defines config but doesn't use it?)
 * Uses HardwareAbstractionLayer i.e. SDK/components/drivers_nrf/hal
 * It is simpler to read.
 * It might make the code portable to nRF51?
 *
 * Uses ADC:
 * - polling (busy waiting), not interrupts
 * - one-shot mode
 * - no periodic calibration
 */


#define CHANNEL_ZERO 0


// static, local functions

namespace {

// Only LSB are filled, MSB are sign extended.
// See below, only 10-bit resolution
static nrf_saadc_value_t resultBuffer;


/*
 * Configure high-level aspects of device.
 */
void configureSAADCHighLevel() {
	//ret_code_t err_code;
	//nrf_drv_saadc_config_t config;

	/*
	 * Resolution 10-bit: output values from 0 (input is 0V) to 2^10=1024 (in is 3.6V)
	 * for channel gain setting of 1/6.
	 */
	// config.resolution = NRF_SAADC_RESOLUTION_10BIT;
	nrf_saadc_resolution_set(NRF_SAADC_RESOLUTION_10BIT);
	// NOT USED: saadc_config.oversample = SAADC_OVERSAMPLE;    // Smoothing, return average of 4 samples for every START

	// Not using interrupt, no need to configure it.
	// Before SDK 12, interrupt was configured here.
	// config.interrupt_priority = APP_IRQ_PRIORITY_LOW;

	/*
	 * Write config struct to device.
	 * Passing nullptr callback func: not using interrupt
	 */
	//err_code = nrf_drv_saadc_init(&config, nullptr);   //Initialize the SAADC with configuration and callback function. The application must then implement the saadc_callback function, which will be called when SAADC interrupt is triggered
	// APP_ERROR_CHECK(err_code);
}


/*
 * Configure channel aspects of device.
 * Since we only configure one channel, mode is one-shot.
 */
void configureSAADCChannel() {
	ret_code_t err_code;
	nrf_saadc_channel_config_t config;

	//Set internal reference of fixed 0.6 volts
	config.reference = NRF_SAADC_REFERENCE_INTERNAL;

	//Set input gain to 1/6. Max input voltage is then 0.6V/(1/6)=3.6V. The single ended input range is then 0V-3.6V
	config.gain = NRF_SAADC_GAIN1_6;

	// Set acquisition time. Set low acquisition time to enable maximum sampling frequency of 200kHz.
	// Set high acquisition time to allow maximum source resistance up to 800 kohm,
	// see the SAADC electrical specification in the PS.
	config.acq_time = NRF_SAADC_ACQTIME_10US;	// This is a middle value.

	//Set single ended. Only the comparators positive pin is input, and negative pin is shorted to ground (0V) internally.
	config.mode = NRF_SAADC_MODE_SINGLE_ENDED;

	//Select input for channel: power supply Vdd
	config.pin_p = NRF_SAADC_INPUT_VDD;

	// Since single ended, negative pin is disabled. The negative pin is shorted to ground internally.
	// According to docs, this step is not necessary since single-ended mode already implies this.
	config.pin_n = NRF_SAADC_INPUT_DISABLED;

	config.resistor_p = NRF_SAADC_RESISTOR_DISABLED;   //Disable pullup resistor on input
	config.resistor_n = NRF_SAADC_RESISTOR_DISABLED;   //Disable pulldown resistor on input

	// Write channel config to device
	// single configured channel
	nrf_saadc_channel_init(CHANNEL_ZERO, &config);
	//err_code = nrf_saadc_channel_init(0, &config);
	//APP_ERROR_CHECK(err_code);
}


void configureSAADCDma() {
	// buffer holds only one sample

	nrf_saadc_buffer_init(&resultBuffer, 1);

	// Not using this because it enables interrupts and starts sampling
	// err_code = nrf_drv_saadc_buffer_convert(resultBuffer, 1);
}

void disableADCInterrupts() {
	nrf_saadc_int_disable(NRF_SAADC_INT_ALL);
}

/*
 * Similar to nrf_drv version nrf_drv_saadc_init()
 */
void initADC() {
	// Now specific to SAADC
	// Appropriate for nRF51 ADC?
	configureSAADCHighLevel();
	configureSAADCChannel();
	configureSAADCDma();
	disableADCInterrupts();
	nrf_saadc_enable();
}

/*
 * Similar to nrf_drv_saadc_uninit()
 *
 * I can't explain, I just chose what seemed appropriate.
 * Some steps might not be necessary.
 */
void uninitADC() {
	nrf_saadc_task_trigger(NRF_SAADC_TASK_STOP);
	// Not waiting for stopped event

	nrf_saadc_disable();

	nrf_saadc_channel_input_set(CHANNEL_ZERO, NRF_SAADC_INPUT_DISABLED, NRF_SAADC_INPUT_DISABLED);

	// assert saadc interrupts disabled
}

/*
 * See nrf_drv_saadc_sample()
 * not low power, which uses interrupt.
 *
 * START dedicates the resources.
 * SAMPLE begins read.
 * Both seem necessary.
 */
void triggerSampleTask() {
	nrf_saadc_task_trigger(NRF_SAADC_TASK_START);
	nrf_saadc_task_trigger(NRF_SAADC_TASK_SAMPLE);
}

void spinUntilADCReady() {
	while (! nrf_saadc_event_check(NRF_SAADC_EVENT_END)) {};
}

void clearSampleReadyEvent() {
	// device will set END event after sampling, conversion, and DMA are complete
	nrf_saadc_event_clear(NRF_SAADC_EVENT_END);
}

void spinUntilSampleReady() {
	// an event indicates
	while (! nrf_saadc_event_check(NRF_SAADC_EVENT_END)) {};
}

unsigned int getUnsignedSample() {
	/*
	 * Docs say " Every sample will be sign extended to 16 bit before stored in the Result buffer."
	 *
	 * That seems nonsensical.  The sample is unsigned, 0-1024 <=> 0V-3.6V.
	 * In any case, make it so.
	 */
	unsigned int result = resultBuffer;
	result = result & 0x3FF;	// Mask 10-bits
	return result;
}


}	// namespace

unsigned int SupplyVoltage::getProportionTo1024() {
	unsigned int result;

	initADC();
	clearSampleReadyEvent();
	triggerSampleTask();
	spinUntilSampleReady();
	result = getUnsignedSample();
	uninitADC();	// save power

	return result;
}

