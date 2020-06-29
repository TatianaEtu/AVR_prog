/* PWM settings */
#include <avr/interrupt.h>
#include "../lib_periphery\common.h"

/**
 *@brief    Initialize PWM (Timer 1)
 *@params   Fast PWM, 16-bit. OCR1A update at bottom, TOV1 flag set on top, top = 2^16	
 *          Non-inverting mode:	clear OC1A on Compare Match, set OC1A at BOTTOM
 *          Prescaler = 1, timer1 frequency = 16 MHz. PWM frequency = 16000000[Hz]/65536[tics] = 245 [Hz]	
 *          All interrupts disable	
 *@note     After initialization timer1 is stopped! To start timer1 call pwmStart();
 */
void pwmInit (void){
	/*Port B (pin1) -> output*/
	DDRB = (1 << DDB1);
    /* set non-inverting mode for OC1A */
	TCCR1A |= (1<<COM1A1);
	
	/*// set fast PWM Mode №14: TOP = ICR1, OCR1A set at bottom and clear on top*/
	TCCR1A |= (1<<WGM11)| (0<<WGM10);
	TCCR1B |= (1<<WGM13)| (1<<WGM12);	
	
	/* set TOP to 16bit */
	ICR1 = 0xFFFF;  
}


/**
 *@brief        Function starts Timer 1
 */
void pwmStart (void){
	TCCR1B |= TIMER1_CLOCK_SELECT_START;
}

/**
 *@brief        Function stops Timer 1 and clear it's counter (sets equal to zero)
 */
void pwmStop (void){
	TCCR1B &= TIMER1_CLOCK_SELECT_STOP;
	TCNT1 = 0;
}

/**
 *@brief        Change OCR1A to change pwm duty cycle
 *@params       duty_cycle [%] in the range 0.0...100.0%
 */
void pwmSetDutyCycle (float duty_cycle){
	uint16_t reg_val = (uint16_t) ((duty_cycle/100.0)*65536.0);
	OCR1A = reg_val;
}

