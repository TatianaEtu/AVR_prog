#ifndef INCLUDE_ADC_H_
#define INCLUDE_ADC_H_

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
void adcInit ( void );

/**
 *@brief           ADC Start 
 *@note            In Free Running mode, write  this bit to one to start the first conversion. 
 *                 The first conversion after ADSC has been written after the ADC has been enabled, 
 *                 or if ADSC is written at the same time as the ADC is enabled, will take 25 ADC clock cycles instead of the normal 13. 
 *                 This first conversion performs initialization of the ADC
 */
void adcStart (void);

/**
 *@brief           ADC Stop (disable, adc clock stops)
 *@note            By writing ADEN to zero, the ADC is turned off. Turning the ADC off
 *                 while a conversion is in progress, will terminate this conversion.
 */
void adcStop (void);

/**
 *@brief	       When an ADC conversion is complete, the result is found in two registers: ADCH, ADCL
 *                 Function return result of conversation as 16-bit variable. But only 10 bits is significant (right adjustment).
 *@note            When ADCL is read, the ADC Data Register is not updated until ADCH is read. 
 *                 So if more than 8-bit precision is required ADCL must be read first, then ADCH.
 */
uint16_t adcRead ( void );






#endif /* INCLUDE_ADC_H_ */