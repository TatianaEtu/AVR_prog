#ifndef INCLUDE_TIMERS_H_
#define INCLUDE_TIMERS_H_

#include <avr/interrupt.h>

/******************************************************************************************************************/
/************************************************ TIMER 0 *********************************************************/
/******************************************************************************************************************/

/* Timer 0 is used to control time gaps between ModBus frames*/


/* timer0 counter value for t1, t1.5, t3.5 intervals */
/* TODO: add calculation from defines (BAUD_RATE, F_OSC, prsescaler) */
static const uint8_t t_1_ch   = 35;
static const uint8_t t_1_5_ch = 54;
static const uint8_t t_3_5_ch = 126;



/**
 *@brief    Initilize Timer0
 *@params   Normal operation mode (OC0A, OC0B disconnected)
 			Clock source: System Clock
 			Clock Prescaler	= 256. Timer frquency = 62,5 kHz	
 			Timer1 Overflow Interrupt: Off
			Compare A Match Interrupt: On
			Compare B Match Interrupt: Off			
		
 *@note    After initialization timer0 is stopped! To start timer0 call timer0Start(); 
 */
void timer0Init (void);

/**
 *@brief        Function resets Timer 0 counter
 */
void timer0ResetCounter (void);

/**
 *@brief        Function return Timer 0 counter value
 */
uint8_t timer0GetCounterValue (void);

/**
 *@brief        Function starts Timer 0 
 */
void timer0Start (void);

/**
 *@brief        Function stops Timer 0 and clear it's counter (sets equal to zero)
 */
void timer0Stop (void);

/**
 *@brief        Timer/Counter0 Output Compare Match A Interrupt Enable
 */
void timer0OCRAInrerruptEn (void);

/**
 *@brief        Timer/Counter0 Output Compare Match A Interrupt Disable
 */
void timer0OCRAInrerruptDis (void);

/**
 *@brief        Timer/Counter0 Output Compare Match B Interrupt Enable
 */
void timer0OCRBInrerruptEn (void);

/**
 *@brief        Timer/Counter0 Output Compare Match B Interrupt Disable
 */
void timer0OCRBInrerruptDis (void);
/**
 *@brief        Timer/Counter0 Overflow Interrupt Enable
 */
void timer0OVFInrerruptEn (void);

/**
 *@brief        Timer/Counter0 Overflow Interrupt Disable
 */
void timer0OVFInrerruptDis (void);




#ifdef USE_TIMER1_AS_TIME_INTERVAL_MESUREMENT_UNIT

/******************************************************************************************************************/
/************************************************ TIMER 1 *********************************************************/
/******************************************************************************************************************/

/* Timer 1 is used to control time period needed for operations such as:
time to receive all buffer, time to clear buffer, time to calculate crc etc. */

#define TIMER1_CLOCK_SELECT_STOP         (0<<CS12)| (0<<CS11)| (0<<CS10)
#define TIMER1_CLOCK_SELECT_START        (0<<CS12)| (0<<CS11)| (1<<CS10)

uint16_t timer1_overflow_counter = 0;

/**
 *@brief    Initialize Timer1
 *@params   Normal operation mode (OC0A, OC0B disconnected)
 			Clock source: System Clock
 			//Clock Prescaler	= 8. Timer frequency = 2 MHz. Since Timer 1 - 16bit, then t_max = 32,76 ms	
			Clock Prescaler	= 1. Timer frequency = 16 MHz. Since Timer 1 - 16bit, then t_max = 4 ms
 			Timer1 Overflow Interrupt: Off
			Compare A Match Interrupt: On
			Compare B Match Interrupt: Off			
		
 *@note     After initialization timer1 is stopped! To start timer1 call timer1Start();
 */
void timer1Init (void);

/**
 *@brief        Function resets Timer 0 counter
 */
void timer1ResetCounter (void);

/**
 *@brief        Function return Timer 0 counter value
 */
uint16_t timer1GetCounterValue (void);

/**
 *@return  period of time in ms 
 */
float timer1GetTimeInterval (void);

/**
 *@brief        Function starts Timer 0 
 */
void timer1Start (void);

/**
 *@brief        Function stops Timer 0 and clear it's counter (sets equal to zero)
 */
void timer1Stop (void);

/**
 *@brief        Timer/Counter0 Output Compare Match A Interrupt Enable
 */
void timer1OCRAInrerruptEn (void);

/**
 *@brief        Timer/Counter0 Output Compare Match A Interrupt Disable
 */
void timer1OCRAInrerruptDis (void);

/**
 *@brief        Timer/Counter0 Output Compare Match B Interrupt Enable
 */
void timer1OCRBInrerruptEn (void);

/**
 *@brief        Timer/Counter0 Output Compare Match B Interrupt Disable
 */
void timer1OCRBInrerruptDis (void);
/**
 *@brief        Timer/Counter0 Overflow Interrupt Enable
 */
void timer1OVFInrerruptEn (void);

/**
 *@brief        Timer/Counter0 Overflow Interrupt Disable
 */
void timer1OVFInrerruptDis (void);

#endif /* USE_TIMER1_AS_TIME_INTERVAL_MESUREMENT_UNIT */

#endif /* INCLUDE_TIMERS_H_ */