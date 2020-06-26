/*
 * Hello world.c
 *
 * Created: 18.05.2020 9:56:00
 * Author : Tanya
 */ 

#define F_CPU 16000000

#include <avr/io.h>
#include <util/delay.h>

#include "lib_periphery\common.h"

#include "lib_modbus\modbus.h"
#include "lib_modbus\modbus.c"

/*
#include "lib_periphery\ADC.h"
#include "lib_periphery\ADC.c"

#include "lib_periphery\PWM.h"
#include "lib_periphery\PWM.c"
*/


int main(void)
{
	
	// Input/Output Ports initialization
	// Port B initialization -> Led
	DDRB = (1 << DDB5);

	
	mbInit();
	/*
	adcInit();
	adcStart();
	pwmInit();
	pwmStart();
	*/
	
	
	// Globally enable interrupts
	sei();
	
    while (1) 
    {
		/*pwmSetDutyCycle (PID_OUT.f);
		_delay_ms(500);	
		*/
	}
	
}

