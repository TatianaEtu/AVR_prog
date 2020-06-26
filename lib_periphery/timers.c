/******************************************************************************************************************/
/************************************************ TIMER 0 *********************************************************/
/******************************************************************************************************************/

/* Timer 0 is used to control time gaps between ModBus frames*/

/**
 *@brief    Initialize Timer0
 *@params   Normal operation mode (OC0A, OC0B disconnected)
 			Clock source: System Clock
 			Clock Prescaler	= 256. Timer frequency = 62,5 kHz	
 			Timer1 Overflow Interrupt: Off
			Compare A Match Interrupt: On
			Compare B Match Interrupt: Off			
		
 *@note     After initialization timer0 is stopped! To start timer0 call timer0Start();
 */
void timer0Init (void){

	TCCR0A = (0<<COM0A1) | (0<<COM0A0) | (0<<COM0B1)| (0<<COM0B0)| (0<<WGM01)| (0<<WGM00);
	TCCR0B = (0<<FOC0A) | (0<<FOC0B) | (0<<WGM02)| (0<<CS02)| (0<<CS01)| (0<<CS00);
	OCR0A = 126; //t3.5
	OCR0B = 54;  //t1.5
	/*  Timer/Counter 0 Interrupt(s) initialization*/
	TIMSK0 = (0<<OCIE0B) | (0<<OCIE0A) | (0<<TOIE0);
}

/**
 *@brief        Function resets Timer 0 counter
 */
void timer0ResetCounter (void){
	TCNT0 = 0;
}

/**
 *@brief        Function return Timer 0 counter value
 */
uint8_t timer0GetCounterValue (void){
	return TCNT0;
}

/**
 *@brief        Function starts Timer 0 
 */
void timer0Start (void){
	TCCR0B |= (1<<CS02)| (0<<CS01)| (0<<CS00);
}

/**
 *@brief        Function stops Timer 0 and clear it's counter (sets equal to zero)
 */
void timer0Stop (void){
	TCCR0B &= (0<<CS02)| (0<<CS01)| (0<<CS00);
	timer0ResetCounter ();
}

/**
 *@brief        Timer/Counter0 Output Compare Match A Interrupt Enable
 */
void timer0OCRAInrerruptEn (void){
	TIMSK0 |= (1<<OCIE0A);
}

/**
 *@brief        Timer/Counter0 Output Compare Match A Interrupt Disable
 */
void timer0OCRAInrerruptDis (void){
	TIMSK0 &= (0<<OCIE0A);
}

/**
 *@brief        Timer/Counter0 Output Compare Match B Interrupt Enable
 */
void timer0OCRBInrerruptEn (void){
	TIMSK0 |= (1<<OCIE0B);
}

/**
 *@brief        Timer/Counter0 Output Compare Match B Interrupt Disable
 */
void timer0OCRBInrerruptDis (void){
	TIMSK0 &= (0<<OCIE0B);
}

/**
 *@brief        Timer/Counter0 Overflow Interrupt Enable
 */
void timer0OVFInrerruptEn (void){
	TIMSK0 |= (1<<TOIE0);
}

/**
 *@brief        Timer/Counter0 Overflow Interrupt Disable
 */
void timer0OVFInrerruptDis (void){
	TIMSK0 &= (0<<TOIE0);
}


//#define USE_TIMER1_AS_TIME_INTERVAL_MESUREMENT_UNIT
#ifdef USE_TIMER1_AS_TIME_INTERVAL_MESUREMENT_UNIT

/******************************************************************************************************************/
/************************************************ TIMER 1 *********************************************************/
/******************************************************************************************************************/

/* Timer 1 is used to control time period needed for operations such as: 
time to receive all buffer, time to clear buffer, time to calculate crc etc. */

/**
 *@brief    Initialize Timer1
 *@params   Normal operation mode (OC0A, OC0B disconnected)
 			Clock source: System Clock
 			Clock Prescaler	= 8. Timer frequency = 2 MHz. Since Timer 1 - 16bit, then t_max = 32,76 ms	
 			Timer1 Overflow Interrupt: On
			Compare A Match Interrupt: Off
			Compare B Match Interrupt: Off			
		
 *@note     After initialization timer1 is stopped! To start timer1 call timer1Start();
 */
void timer1Init (void){

	TCCR1A = (0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1)| (0<<COM1B0)| (0<<WGM11)| (0<<WGM10);
	TCCR1B = (0<<ICNC1) | (0<<ICES1) | (0<<WGM13)| (0<<WGM12)| (0<<CS12)| (0<<CS11)| (0<<CS10);
	
	/*  Timer/Counter 1 Interrupt(s) initialization*/
	TIMSK1 = (0<<ICIE1) |(0<<OCIE1B) | (0<<OCIE1A) | (1<<TOIE1);
}

/**
 *@brief        Function resets Timer 1 counter
 */
void timer1ResetCounter (void){
	TCNT1L = 0;
	TCNT1H = 0;
	
	timer1_overflow_counter = 0;
}

/**
 *@brief        Function return Timer 1 counter value
 */
uint16_t timer1GetCounterValue (void){
	return (TCNT1H << 8) | TCNT1L;
	// return (255 << 8) | 124 ;  // test fun
}

/**
 *@return  period of time in s 
 */
float timer1GetTimeInterval (void){
	 return ((float)(timer1_overflow_counter*65535 + timer1GetCounterValue() ))/ ((float) F_CPU);
	// return ((float)(3*65535 + timer1GetCounterValue() ))/ ((float) F_CPU); //test fun
}
/**
 *@brief        Function starts Timer 1
 */
void timer1Start (void){
	TCCR1B |= TIMER1_CLOCK_SELECT_START;
}

/**
 *@brief        Function stops Timer 1 and clear it's counter (sets equal to zero)
 */
void timer1Stop (void){
	TCCR1B &= TIMER1_CLOCK_SELECT_STOP;
	//timer1ResetCounter ();
}

/**
 *@brief        Timer/Counter1 Output Compare Match A Interrupt Enable
 */
void timer1OCRAInrerruptEn (void){
	TIMSK1 |= (1<<OCIE1A);
}

/**
 *@brief        Timer/Counter1 Output Compare Match A Interrupt Disable
 */
void timer1OCRAInrerruptDis (void){
	TIMSK1 &= (0<<OCIE1A);
}

/**
 *@brief        Timer/Counter1 Output Compare Match B Interrupt Enable
 */
void timer1OCRBInrerruptEn (void){
	TIMSK1 |= (1<<OCIE1B);
}

/**
 *@brief        Timer/Counter1 Output Compare Match B Interrupt Disable
 */
void timer1OCRBInrerruptDis (void){
	TIMSK1 &= (0<<OCIE1B);
}

/**
 *@brief        Timer/Counter1 Overflow Interrupt Enable
 */
void timer1OVFInrerruptEn (void){
	TIMSK1 |= (1<<TOIE1);
}

/**
 *@brief        Timer/Counter1 Overflow Interrupt Disable
 */
void timer1OVFInrerruptDis (void){
	TIMSK1 &= (0<<TOIE1);
}


ISR (TIMER1_OVF_vect){
	timer1_overflow_counter ++;
	TCNT1L = 0;
	TCNT1H = 0;
	
}


#endif