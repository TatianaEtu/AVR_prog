#include "../lib_periphery\common.h"

#include "../lib_periphery\uart.h"
#include "../lib_periphery\uart.c"

#include "../lib_periphery\timers.h"
#include "../lib_periphery\timers.c"



/**
 *@brief    Modbus initialization
 *@note
 */
void mbInit(void)
{
	MB_STATE = MB_STATE_IDLE; //ready to receive
	uartInit(); // note: RX and TX interrupts enable
			
	/* zero initialization of the rx_buffer and tx_buffer*/	
	mbClearRecieveBuffer();
	mbClearTransmitBuffer();
	
	/* Timer0 stands for 3.5 character delay detect */
	/* processing a received message, generating a response, 
	and initiating the sending of a response occurs in the interrupt handler*/
	timer0Init();
	timer0Start();
	timer0OCRAInrerruptEn();  
	

	/*
	VAR1 = 34780; // test
	uint16_reg_array[0] = VAR1;
	uint16_reg_array[1] = 278;
	uint16_reg_array[MB_ADR_TEMPERATURE] = 278;
	uint16_reg_array[MB_ADR_TEMPERATURE+1] = 32;
	*/
	
	

	
	

			
	/* Timer1 stands for time intervals measurement*/
	//timer1Init();
	
	/* Code to measure time intervals. Put it in that part of the program the runtime of which you want to measure 
	timer1Start();
	// ----- block of code ------ //	
	timer1Stop();
	test_time.f = timer1GetTimeInterval();
	timer1ResetCounter();
	uint16_reg_array[MB_REG_ADR_FLOAT_BEGINING +10] = ( test_time.u[0] << 8 ) | ( test_time.u[1] );
	uint16_reg_array[MB_REG_ADR_FLOAT_BEGINING + 11] = ( test_time.u[2] << 8 ) | ( test_time.u[3] ); */
	
}

/**
 *@brief    The function allows you to find out what state the modbas protocol state machine is in
 *@return   Current Modbus state (mb_state - typedef enum)
 *@note     Modbus states specified in modbus.h
 */
mb_state mbGetState (void){
	return MB_STATE;
}

/**
 *@brief  filling the rx_buffer array with zeros
 */
void mbClearRecieveBuffer (void){
	rx_buf_index = 0;
	for (int i = 0; i< RX_BUFFER_SIZE; i++){
		rx_buffer[i] = 0;
	}
}

/**
 *@brief  filling the tx_buffer array with zeros
 */
void mbClearTransmitBuffer (void){
	tx_buf_index = 0;
	for (int i = 0; i< TX_BUFFER_SIZE; i++){
		tx_buffer[i] = 0;
	}
}

/**
 *@brief      CRC16 calculation, regular (non-table) method
 *@params     msg* - pointer to message array, msg_length - number of bytes in message (without CRC_low, CRC_high)
 *@return     16bit CRC. LSB first: CRC_low|CRC_high
 *@note		  This function should be called only after the buffer is completely filled (CRC is not calculated on the fly)
 */
static const uint16_t mb_crc_poly = 0xA001; //polynomial value (for CRC16 XOR POLY)
uint16_t mbCRCCalc (uint8_t* msg, uint8_t msg_length){
	/* CRC value initialization*/
	uint16_t crc_val = 0xFFFF;
	
	for (uint8_t msg_index = 0; msg_index < msg_length; msg_index ++){
		crc_val = crc_val ^ msg[msg_index];
		for (uint8_t crc_index = 0; crc_index < 8; crc_index++){

			if ( crc_val & (1 << 0) ){
				crc_val = ( crc_val >> 1 ) ^ mb_crc_poly;
			}
			else{
				crc_val = crc_val >> 1; 
			}
		}
	}

	/* swap bytes in crc_val */
	crc_val = (crc_val << 8)|(crc_val >> 8);
	return crc_val;
}

/*
 *@brief     Function check received frame ( is slave ID matches, is CRC OK)
 *@return    -1 - the message is not addressed to me (ID does not match), 
 *            0 - ID is the same, error in the CRC, 
 *			  1 - ID is the same, CRC is OK, 250 is initial value
 */
int8_t mbFrameCheck( void ){
	
	//if (rx_buffer[0] == ADDRESS_FIELD)
	//{
		//PORTB = (1<< PORTB5); // set led on
	//}
	
	
	if ( ADDRESS_FIELD == SLAVE_ID ){
		/*this variable contains the last 2 bytes of received message*/
		uint16_t rx_crc_field = 0; 

		/* error: 
		uint16_t rx_crc_field = (rx_buffer[RX_BUFFER_SIZE - 2] << 8) | rx_buffer[RX_BUFFER_SIZE - 1]; 
		uint16_t crc = mbCRCCalc( rx_buffer, RX_BUFFER_SIZE - 2 ); */	
		
		/* last 2 bytes of received message are read, not last 2 bytes of rx_buffer! (compare with the code above, comment as error) */
		rx_crc_field = (rx_buffer[rx_buf_index - 2] << 8) | rx_buffer[rx_buf_index - 1];
		/* calculation of the checksum of the received message*/
		uint16_t crc = mbCRCCalc( rx_buffer, rx_buf_index - 2 );		
		
		/* Compare the calculated checksum with the received */
		if (crc == rx_crc_field) return 1; // CRC is OK -> normal reply
		else                     return 0; // CRC is not OK  -> ignore message		
	}

	/* The message is not addressed to me. Ignore message */
	else {
		
		return -1;
	} 	

}

/**
 *@brief     Function 0x03 - Read multiple registers
 *			 This function filling tx_buffer with answer. 
 *           if starting address and (starting address + number of register) is OK then response form is:
 *           ID|FUN_number|Number of bytes (N registers*2)| reg1 high byte| reg1 low byte| ...| regN high byte| regN low byte | CRC low |CRC high
 *           if starting address and (starting address + number of register) is NOT OK then response form is:
 *           ID|FUN_number|error code|exception code| CRC low |CRC high 
 *
 *@note     tx_buffer is a global array, declared in modbus.h
 */
void mbFun03Execution (void)
{
	/* The first 2 bytes of the response are always the same (in normal reply and in the error reply) */
	/* so at the beginning we write them in the response buffer (tx_buffer) */
	tx_buffer[0] = SLAVE_ID;
	tx_buffer[1] = FUNCTION_FIELD;

	/* Read the starting address */
	uint16_t starting_adr = rx_buffer[2]<<8 | rx_buffer[3];
	uint16_t num_of_reg   = rx_buffer[4]<<8 | rx_buffer[5];
	
	/* if the addresses of all the registers do not lie in the available address space then answer error*/
	if ( (starting_adr < 0) || (starting_adr >= uint16_reg_array_len) || ((starting_adr+num_of_reg) > uint16_reg_array_len) ){
		tx_buffer[2] = 0x83;  // error code
		tx_buffer[3] = 0x02;  // exception code
		uint16_t crc_of_respond_msg = mbCRCCalc( tx_buffer, 4 );
		tx_buffer[4] =  (crc_of_respond_msg >> 8) & 0xFF;
		tx_buffer[5] =  crc_of_respond_msg & 0xFF;
		tx_buf_bytes_number = 6;
		return;	
	}
	
	else{ // if start adr is ok, and num of byte is ok -> sending requested registers
		tx_buffer[2] = (2 * num_of_reg) & 0xFF; // num of bytes
		
		uint8_t k =0; // index_step_counter		
		for (uint16_t j = 0; j < num_of_reg; j++){
			/* first 3 bytes of response frame is a system information: 
			ID, function number, number of bytes (number of registers * 2).
			Therefore, indexing the area of the tx_buffer where the registers will be written starts with 3 */
			
			tx_buffer[3+k]= (uint16_reg_array[starting_adr+j] >> 8) & 0xFF;
			tx_buffer[3+k+1]= uint16_reg_array[starting_adr+j] & 0xFF;
			k = k + 2; // since the register is a double byte number
		}
		
		uint16_t crc_of_respond_msg = mbCRCCalc( tx_buffer, 3 + tx_buffer[2] ); // (3 + tx_buffer[2]) - message length up to checksum bytes
		/* At the last iteration of the loop, changing the counter "k" was the last operation. 
		Therefore, the counter "3+k" contains the index of the next cell in the array*/
		tx_buffer[3 + k] =  (crc_of_respond_msg >> 8) & 0xFF;
		tx_buffer[3 + k+1] =  crc_of_respond_msg & 0xFF;
		k = k + 2;
		
		tx_buf_bytes_number = 3 + k; // total length of the response message
		/* This variable is necessary to send only the generated message. 
		Since the answer can be of different lengths, 
		and the size of the buffer is designed for a response of maximum length.
		This variable is global and is used in the body of the interrupt handler when the byte is sent by UART*/
	}	
}

/**
 *@brief     Function 0x06 - Write single register
 *           If register address corresponds to the area of memory where variables of type uint16 are stored then
 *			 this function is writes the received value to an array "uint16_reg_array" and filling tx_buffer with answer. 
 *           This is necessary so that only one register of two registers of a variable of type float is not changed
 *           
 *           If starting address is OK then response form is:
 *           ID|FUN_number|starting address| value high byte| value low byte|CRC low |CRC high
 
 *           If starting address and is NOT OK then response form is:
 *           ID|FUN_number|error code|exception code| CRC low |CRC high 
 *
 *@note     tx_buffer is a global array, declared in modbus.h
 */
void mbFun06Execution (void)
{
	uint16_t reg_adr = rx_buffer[2]<<8 | rx_buffer[3];
	uint16_t value   = rx_buffer[4]<<8 | rx_buffer[5];
	/* target register address is not in the area where uint16_t variables are located - > response with error */
	if (reg_adr >= MB_REG_ADR_FLOAT_BEGINING){ 
		tx_buffer[0] = SLAVE_ID;
		tx_buffer[1] = FUNCTION_FIELD;
		tx_buffer[2] = 0x86;
		tx_buffer[3] = 0x02;
		
		uint16_t crc_of_respond_msg = mbCRCCalc( tx_buffer, 4 );
		tx_buffer[4] =  (crc_of_respond_msg >> 8) & 0xFF;
		tx_buffer[5] =  crc_of_respond_msg & 0xFF;
		
		tx_buf_bytes_number = 6;
		return;		
	}
	
	else{ // if everything is ok -> writing a value to an data array and send normal respond 
		uint16_reg_array[reg_adr] = value;
				
		for (uint16_t j = 0; j < 6; j++){
			tx_buffer[j]= rx_buffer[j];
		}
		
		uint16_t crc_of_respond_msg = mbCRCCalc( tx_buffer, 6 );
		tx_buffer[6] =  (crc_of_respond_msg >> 8) & 0xFF;
		tx_buffer[7] =  crc_of_respond_msg & 0xFF;
		
		tx_buf_bytes_number = 8;
	}
}

/**
 *@brief     Function 0x10 - Write multiple register
 *           This function is writes the received value to an array "uint16_reg_array" and filling tx_buffer with answer. 
 *           This is necessary so that only one register of two registers of a variable of type float is not changed
 *           
 *           If starting address is OK then response form is:
 *           ID|FUN_number|starting address| value high byte| value low byte|CRC low |CRC high
 
 *           If starting address and is NOT OK then response form is:
 *           ID|FUN_number|error code|exception code| CRC low |CRC high 
 *
 *@note     tx_buffer is a global array, declared in modbus.h
 */
void mbFun10Execution (void)
{
	uint8_t k =0; // index_step_counter
	uint16_t starting_adr = rx_buffer[2]<<8 | rx_buffer[3];
	uint16_t num_of_reg   = rx_buffer[4]<<8 | rx_buffer[5];
	uint8_t  num_of_bytes = rx_buffer[6];
	/* if the address of at least one register is out of the available range of address space then respond with erorr message */
	if ( (starting_adr < 0) || (starting_adr >= uint16_reg_array_len) || ((starting_adr+num_of_reg) > uint16_reg_array_len) ){ 
		tx_buffer[0] = SLAVE_ID;
		tx_buffer[1] = FUNCTION_FIELD;
		tx_buffer[2] = 0x90;
		tx_buffer[3] = 0x02;
		
		uint16_t crc_of_respond_msg = mbCRCCalc( tx_buffer, 4 );
		tx_buffer[4] =  (crc_of_respond_msg >> 8) & 0xFF;
		tx_buffer[5] =  crc_of_respond_msg & 0xFF;
		
		tx_buf_bytes_number = 6;
		return;		
	}
	/* If the addresses of all the registers are in the available range of the address space, 
	then the values of the registers are written to the data array and a normal response is generated */
	else{
		/* record new values to "uint16_reg_array" - modbas dataset */
		for (uint16_t j = 0; j < num_of_bytes; j=j+2){
			uint16_reg_array[starting_adr + k] = rx_buffer[7 + j]<<8 | rx_buffer[7 + j + 1];
		/*	if ((starting_adr + k) >= MB_REG_ADR_FLOAT_BEGINING){
				if (((starting_adr + k - MB_REG_ADR_FLOAT_BEGINING) % 2) != 0){
					union_arr[starting_adr + k - MB_REG_ADR_FLOAT_BEGINING - 1].u[0] = uint16_reg_array[starting_adr + k - 1] >> 8;
					union_arr[starting_adr + k - MB_REG_ADR_FLOAT_BEGINING - 1].u[1] = uint16_reg_array[starting_adr + k - 1] & 0xFF;
					union_arr[starting_adr + k - MB_REG_ADR_FLOAT_BEGINING - 1].u[2] = uint16_reg_array[starting_adr + k] >> 8;
					union_arr[starting_adr + k - MB_REG_ADR_FLOAT_BEGINING - 1].u[3] = uint16_reg_array[starting_adr + k] & 0xFF;
				}				
			}
			else{
				mb_uint16_variables_arr[starting_adr + k] = uint16_reg_array[starting_adr + k];
			}
			*/
			k++;
		}
		/* Normal response copies the request completely */
		for (uint16_t j = 0; j < 6; j++){
			tx_buffer[j]= rx_buffer[j];
		}
		
		
		
		
		uint16_t crc_of_respond_msg = mbCRCCalc( tx_buffer, 6 );
		tx_buffer[6] =  (crc_of_respond_msg >> 8) & 0xFF;
		tx_buffer[7] =  crc_of_respond_msg & 0xFF;
		
		tx_buf_bytes_number = 8;
	}	
}

/**
 *@brief    This function checks the second byte of the request (received buffer) 
 *          in which the modbus function number is stored. 
 *          And calls the corresponding handler (mbFun03Execution, mbFun06Execution, mbFun10Execution) 
 */
void mbReplyProcessing ( void ){
	
		if ( FUNCTION_FIELD == 0x03 )  mbFun03Execution();
		if ( FUNCTION_FIELD == 0x06 )  mbFun06Execution ();		
		if ( FUNCTION_FIELD == 0x10 )  mbFun10Execution ();	
		/* if the function number is not supported 
		in accordance with the modbas standard, 
		you need to send an error message, 
		but in the current version of the program, 
		in this case, the request is ignored (slave nothing responds) */
}

/** 
 *@brief    RX interrupt handler (single byte received) 
 */
ISR (USART_RX_vect){
	/* read operation from hardware register UDR0 */
	int8_t data = uartReadByte();

	/* If the curent state of modbus state machine is MB_STATE_IDLE
	then this is the first byte of the package. 
	Modbus state switches to MB_STATE_RECEPTION
	In this state, the receive buffer is allowed to fill*/
	if (MB_STATE == MB_STATE_IDLE){
		MB_STATE = MB_STATE_RECEPTION;					
	}	
	
	/* Filling receive buffer */
	if ( MB_STATE == MB_STATE_RECEPTION ){		
		if (rx_buf_index < RX_BUFFER_SIZE){// if rx_buf_index > (RX_BUFFER_SIZE - 1)  then recieve data lost
			rx_buf_index++;		
			rx_buffer[rx_buf_index] = data; 
		}
	}
	/* After each received byte reset timer (which detects t3.5 delay after frame) */
	timer0ResetCounter(); 	
}
	

/** 
 *@brief    TX interrupt handler (single byte transmitted)
 *@note     Initiation of sending does not occur in interruption 
			The first byte of the response frame must be sent to initiate
 */
ISR (USART_TX_vect){
		/* Immediately after sending the first byte of the frame, 
		the program state is switched to MB_STATE_IDLE, which means that the next request is allowed. 
		Thus, sending a response to a previous request and receiving the next request can occur in parallel */
		MB_STATE = MB_STATE_IDLE;
		
		/*Only the part of the "tx_buffer" array that contains the response frame is sent.
		"tx_buf_bytes_number" is total length of the response message
		Since the answer can be of different lengths, 
		and the size of the buffer is designed for a response of maximum length.*/
		if ( tx_buf_index < tx_buf_bytes_number ){			
			UDR0 = tx_buffer[tx_buf_index]; // if tx_buf_index > (tx_buf_bytes_number - 1)  then  data isn't transmmit further
			tx_buf_index ++;			
		}
		
		else{	
			/* After the last byte of the response frame sent, the end of transmission flag is set.
			This flag is necessary to not start processing the next request until the response to the previous request is completely sent. 
			Since the response handler uses an "tx_buffer" array.
			So it is a protection of the response message from modification during the sending process, otherwise the response will be distorted */ 
			transmittion_finish = 1;
			
			/* After the last byte of the response frame sent the "tx_buffer" array must be cleaned */
			mbClearTransmitBuffer();
			
			/* If while the response to the previous request was sent, the next request was accepted, 
			then you need to start the request handler, generate a response and initiate its transmission */
			if ( processing_queue ){ 
				mbReplyProcessing();
				MB_STATE = MB_STATE_EMISSION;
				transmittion_finish = 0; //reset finish flag (means transmission starts).
				uartTransmitByte (tx_buffer[0]);
				tx_buf_index ++;
			}						
		}
}

/** 
 *@brief    Timer0 output compare A interrupt service routine.
 *			This interrupt is triggered when the timer calculates to the value 
 *          corresponding to the time interval of 3.5 characters (at a speed of 19200 bits per second).
 *          Timer counter is reset after each byte received.
 *@note     Processing of the received frame occurs in the timer interrupt handler!
 */
ISR (TIMER0_COMPA_vect){
	timer0ResetCounter();
	
	if ( MB_STATE == MB_STATE_RECEPTION ){		
						
		MB_STATE = MB_STATE_FRAME_CHECKING;							
		frame_check_result = mbFrameCheck();
					
		MB_STATE = MB_STATE_PROCESSING_REPLY;			
		if (frame_check_result == 1){					
			if ( !transmittion_finish ){
				/* If the received message passed the frame check (the identifier and the checksum match)
				 but the response to the previous request has not yet been completely sent, 
				 then the queue flag for processing the request is set. */
				processing_queue = 1;			
				return;		
			}	
			else{
				mbReplyProcessing();
				MB_STATE = MB_STATE_EMISSION;
				transmittion_finish = 0;
				uartTransmitByte (tx_buffer[0]);
				tx_buf_index ++;
			}				
		}
		else{
			/* If the received message don't passed the frame check (the identifier or the checksum doesn't match)
			Then ignore request. No response is generated or sent. */
			
			MB_STATE = MB_STATE_IDLE;
		}
		/* Clear receive buffer in all cases! even if slave don't need to reply */
		mbClearRecieveBuffer ();
	}
}


