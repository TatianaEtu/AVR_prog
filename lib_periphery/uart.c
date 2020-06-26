//#include <uart.h> /* don't needed for Atmel Studio 7*/


/* Baud rate register value calculation. For normal asynchronous mode only! */
static const uint32_t uart_baud_rate_reg = ((F_CPU/16)/BAUD_RATE)-1;


#define RX_COMPLETE          ( 1 << RXC0  )
#define TX_COMPLETE          ( 1 << TXC0  )
#define DATA_REGISTER_EMPTY  ( 1 << UDRE0 )
#define FRAMING_ERROR        ( 1 << FE0   )
#define DATA_OVERRUN         ( 1 << DOR0  )
#define PARITY_ERROR         ( 1 << UPE0  )



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
void uartInit (void){

	UCSR0B = (1<<RXCIE0) | (1<<TXCIE0) | (0<<UDRIE0) | (1<<RXEN0) | (1<<TXEN0) | (0<<UCSZ02) | (0<<RXB80) | (0<<TXB80);
	UCSR0C = (0<<UMSEL01) | (0<<UMSEL00) | (0<<UPM01) | (0<<UPM00) | (1<<USBS0) | (1<<UCSZ01) | (1<<UCSZ00) | (0<<UCPOL0);
	UBRR0H = uart_baud_rate_reg >> 8;
	UBRR0L = uart_baud_rate_reg & 0xFF;
}

int8_t uartIsDataRegisterEmpty (void)
{
	return UCSR0A & DATA_REGISTER_EMPTY;
}

/**
 *@brief
 *@note
 */
void uartTransmitByte(uint8_t data) 
{
	/*uint8_t status = UCSR0A;
	while(!(status & DATA_REGISTER_EMPTY));	
	*/
	UDR0 = data;
	
}

int8_t uartReadByte (void)
{
	uint8_t status = UCSR0A;
	uint8_t data   = UDR0;
	if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN)) == 0 )
	{
		return data;
	}
	else
	{
		return -1;
	}
	

}

void uartRXInterruptEn (void)
{
	UCSR0B |= ( 1 << RXCIE0 );
}

void uartRXInterruptDis (void)
{
	UCSR0B &= ( 0 << RXCIE0 );
}

void uartTXInterruptEn (void)
{
	UCSR0B |= ( 1 << TXCIE0 );
}

void uartTXInterruptDis (void)
{
	UCSR0B &= ( 0 << TXCIE0 );
}

void uartUDREmptyInterruptEn (void)
{
	UCSR0B |= ( 1 << UDRIE0 );
}

void uartUDREmptyInterruptDis (void)
{
	UCSR0B &= ( 0 << UDRIE0 );
}






