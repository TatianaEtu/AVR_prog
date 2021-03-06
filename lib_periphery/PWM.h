#ifndef INCLUDE_PWM_H_
#define INCLUDE_PWM_H_

#define TIMER1_CLOCK_SELECT_STOP         (0<<CS12)| (0<<CS11)| (0<<CS10)
#define TIMER1_CLOCK_SELECT_START        (0<<CS12)| (0<<CS11)| (1<<CS10)


/**
 *@brief    Initialize PWM (Timer 1)
 *@params   Fast PWM, 16-bit. OCR1A update at bottom, TOV1 flag set on top, top = 2^16	
 *          Non-inverting mode:	clear OC1A on Compare Match, set OC1A at BOTTOM
 *          Prescaler = 1, timer1 frequency = 16 MHz. PWM frequency = 16000000[Hz]/65536[tics] = 245 [Hz]	
 *          All interrupts disable	
 *@note     After initialization timer1 is stopped! To start timer1 call pwmStart();
 */
void pwmInit (void);

/**
 *@brief        Function starts Timer 1
 */
void pwmStart (void);

/**
 *@brief        Function stops Timer 1 and clear it's counter (sets equal to zero)
 */
void pwmStop (void);

/**
 *@brief        Change OCR1A to change pwm duty cycle
 *@params       duty_cycle [%] in the range 0.0...100.0%
 */
void pwmSetDutyCycle (float duty_cycle);



#endif /* INCLUDE_PWM_H_ */