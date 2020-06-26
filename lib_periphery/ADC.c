/* ADC settings */
#include "../lib_periphery\common.h"

/**
 *@brief           ADC Initialization
 *                 1. Channel 0 (pin A0)
 *                 2. Voltage reference -> AREF
 *                 3. ADC clock prescaler = 64, ADC clock frequency = 250 kHz, 
 *                 time to 1 normal conversation (13 clock cycles) = 52 us
 *                 4. Free running mode
 *                 5. Result is right adjusted
 *                 6. ADC Interrupt is enable ( ADC Conversion Complete)
 *                 7. The digital input buffer on the A0 pin is disabled
 *@note            After initialization ADC is disable! ADC clock is stops.
 *                 To start adc call function adcStart()
 */
void adcInit ( void ){
	/* ADC Multiplexer Selection Register */
	/* Bits 7:6 – REFSn: Reference Selection:  AREF, Internal Vref turned off */
	/* Bit 5 – ADLAR: ADC Left Adjust Result */
	/* Bits 3:0 – MUXn: Analog Channel Selection [n = 3:0] */
	ADMUX = (0 << REFS1) | (0 << REFS0) | (0 << ADLAR) | (0 << MUX3) | (0 << MUX2) | (0 << MUX1) | (0 << MUX0);
	/* ADC Control and Status Register A */
	/* Bit 7 – ADEN: ADC Enable */
	/* Bit 3 – ADIE: ADC Interrupt Enable */
	/* Bits 2:0 – ADPSn: ADC Prescaler Select */
	ADCSRA = (0 << ADEN) | (1 << ADIE) | (1<< ADPS2) | (1 << ADPS1) | (0 << ADPS0);	
	/* Digital Input Disable Register 0 */
	DIDR0 = (1 << ADC0D); // the digital input buffer on the A0 pin is disabled
}

/**
 *@brief           ADC Start 
 *@note            In Free Running mode, write  this bit to one to start the first conversion. 
 *                 The first conversion after ADSC has been written after the ADC has been enabled, 
 *                 or if ADSC is written at the same time as the ADC is enabled, will take 25 ADC clock cycles instead of the normal 13. 
 *                 This first conversion performs initialization of the ADC
 */
void adcStart (void){
	ADCSRA = (1 << ADEN)|( 1 << ADSC);	
}

/**
 *@brief           ADC Stop (disable, adc clock stops)
 *@note            By writing ADEN to zero, the ADC is turned off. Turning the ADC off
 *                 while a conversion is in progress, will terminate this conversion.
 */
void adcStop (void){
	ADCSRA = ( 0 << ADEN);	//Writing zero to ADSC bit has no effect (see datasheet)
}


/**
 *@brief	       When an ADC conversion is complete, the result is found in two registers: ADCH, ADCL
 *                 Function return result of conversation as 16-bit variable. But only 10 bits is significant (right adjustment).
 *@note            When ADCL is read, the ADC Data Register is not updated until ADCH is read. 
 *                 So if more than 8-bit precision is required ADCL must be read first, then ADCH.
 */
uint16_t adcRead ( void ){
	uint8_t adc_result_low_reg = ADCL;
	uint8_t adc_result_high_reg = ADCH;
	uint8_t result = (adc_result_high_reg << 8) | adc_result_low_reg;
	return result;
}


mb_float_type   temperature; // mb_float_type not defined here!!!!
/**
 *@brief          ADC Conversion Complete Interrupt handle 
 */
ISR ( ADC_vect ){
	uint16_t result =0;
	result = adcRead();
	TEMPERATURE.f = (float)result / 1024.0 * 2.5; // 2.5[V] = AREF
	
	uint16_reg_array[MB_ADR_TEMPERATURE] = ( TEMPERATURE.u[0] << 8 ) | ( TEMPERATURE.u[1] );
	uint16_reg_array[MB_ADR_TEMPERATURE+1] = ( TEMPERATURE.u[2] << 8 ) | ( TEMPERATURE.u[3] );
}