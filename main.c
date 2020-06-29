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
*/


#include "lib_periphery\PWM.h"
#include "lib_periphery\PWM.c"

#include "lib_pid_controller\PID.h"
#include "lib_pid_controller\PID.c"
/*
static pidContext_t  pidCtx;
pidCtx.kp = 1.0;
pidCtx.ki   = 0.01;
pidCtx.kd   = 0;
*/
static pidContext_t  pidCtx = {
	.kp   = 1.0,
	.ki   = 0.01,
	.kd   = 0
};

int main(void)
{
	
	// Input/Output Ports initialization
	// Port B initialization -> Led
	DDRB |= (1 << DDB5);

	
	mbInit();
	
/*	adcInit();
	adcStart(); */
	pwmInit();
	pwmStart();
	
	pidInit(&pidCtx);
	
	
	
	// Globally enable interrupts
	sei();
	
    while (1) 
    {
		
		PID_CONTROL_ERROR.f = SET_POINT.f - TEMPERATURE.f;
		pidCtx.err = PID_CONTROL_ERROR.f; // + check dead zone
		PID_CONTROL_OUT.f = pidCalculation(&pidCtx);
		pwmSetDutyCycle (PID_CONTROL_OUT.f);
		PID_CONTROL_P_FACTOR.f = pidCtx.P_factor; // for test via modbus
		PID_CONTROL_I_FACTOR.f = pidCtx.I_factor; // for test via modbus
		PID_CONTROL_D_FACTOR.f = pidCtx.D_factor; // for test via modbus
		
		
		uint16_reg_array[MB_ADR_PID_CONTROL_ERROR] = ( PID_CONTROL_ERROR.u[0] << 8 ) | ( PID_CONTROL_ERROR.u[1] );
		uint16_reg_array[MB_ADR_PID_CONTROL_ERROR+1] = ( PID_CONTROL_ERROR.u[2] << 8 ) | ( PID_CONTROL_ERROR.u[3] );
		
		uint16_reg_array[MB_ADR_PID_CONTROL_OUT] = ( PID_CONTROL_OUT.u[0] << 8 ) | ( PID_CONTROL_OUT.u[1] );
		uint16_reg_array[MB_ADR_PID_CONTROL_OUT+1] = ( PID_CONTROL_OUT.u[2] << 8 ) | ( PID_CONTROL_OUT.u[3] );
		
		uint16_reg_array[MB_ADR_PID_CONTROL_P_FACTOR] = ( PID_CONTROL_P_FACTOR.u[0] << 8 ) | ( PID_CONTROL_P_FACTOR.u[1] );
		uint16_reg_array[MB_ADR_PID_CONTROL_P_FACTOR+1] = ( PID_CONTROL_P_FACTOR.u[2] << 8 ) | ( PID_CONTROL_P_FACTOR.u[3] );
		
		uint16_reg_array[MB_ADR_PID_CONTROL_I_FACTOR] = ( PID_CONTROL_I_FACTOR.u[0] << 8 ) | ( PID_CONTROL_I_FACTOR.u[1] );
		uint16_reg_array[MB_ADR_PID_CONTROL_I_FACTOR+1] = ( PID_CONTROL_I_FACTOR.u[2] << 8 ) | ( PID_CONTROL_I_FACTOR.u[3] );
		
		_delay_ms(10);	// sampling frequency 100 Hz
	
	}
	
}

