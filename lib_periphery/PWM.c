﻿/* PWM settings */
#include <avr/interrupt.h>
#include "../lib_periphery\common.h"

/**
 *@brief    Initialize PWM (Timer 1)
 *@params   Fast PWM, 10-bit. OCR1A update at bottom, TOV1 flag set on top, top = 1023	
 *          Non-inverting mode:	clear OC1A on Compare Match, set OC1A at BOTTOM
 *          Prescaler = 64, timer1 frequency = 16000000|64 = 250 kHz. PWM frequency = 250000[Hz]/1023[tics] = 244,4 [Hz]	
 *          All interrupts disable	
 *@note     After initialization timer1 is stopped! To start timer1 call pwmStart();
 */
void pwmInit (void){
	/*Port B (pin1) -> output*/
	DDRB = (1 << DDB1);
	/* 	Bits 4, 5, 6, 7  Compare Output Mode for Channel */
	/* Bits 0, 1 – WGM10, WGM11: Waveform Generation Mode */
	TCCR1A |= (1<<COM1A1) | (1<<WGM11)| (0<<WGM10);
	/* Bit 7 – ICNC1: Input Capture Noise Canceler */
	/* Bit 6 – ICES1: Input Capture Edge Select */
	/* Bits 0, 1, 2 – CS10, CS11, CS12: Clock Select 1 [n = 0..2] */
	TCCR1B |= (1<<WGM13)| (1<<WGM12);	
	/*  Timer/Counter 1 Interrupt(s) initialization*/
	//TIMSK1 |= (0<<ICIE1) |(0<<OCIE1B) | (0<<OCIE1A) | (0<<TOIE1);
	
	ICR1 = 0xFFFF; 	// set TOP to 16bit

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
	TCNT1L = 0;
	TCNT1H = 0;
}

/**
 *@brief        Change OCR1A to change pwm duty cycle
 *@params       duty_cycle [%] in the range 0.0...100.0%
 */
void pwmSetDutyCycle (float duty_cycle){
	uint16_t reg_val = (uint16_t) ((duty_cycle/100.0)*65536.0);
	OCR1A = reg_val;
}

