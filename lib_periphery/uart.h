#ifndef INCLUDE_UART_H_
#define INCLUDE_UART_H_

#include <avr/interrupt.h>


#define BAUD_RATE 19200




/**
 *@brief    Initilize UART
 *@params   Communication Parameters: 8 Data, 2 Stop, No Parity
 			USART Mode: Asynchronous Normal
			USART Receiver: On
			USART Transmitter: On			
			RX Complete Interrupt Enable
			TX Complete Interrupt Enable 
			(!) USART Data Register Empty Interrupt Disable
			
 *@note     USART Baud Rate spcified in uart.h
 */
void uartInit (void);

int8_t uartIsDataRegisterEmpty (void);

void uartTransmitByte(uint8_t data);
int8_t uartReadByte (void);

void uartRXInterruptEn (void);
void uartRXInterruptDis (void);

void uartTXInterruptEn (void);
void uartTXInterruptDis (void);

void uartUDREmptyInterruptEn (void);
void uartUDREmptyInterruptDis (void);



#endif /* INCLUDE_UART_H_ */