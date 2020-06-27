#ifndef INCLUDE_MODBUS_H_
#define INCLUDE_MODBUS_H_

/* this microcontroller (Atmega328P) ModBus ID */
#define SLAVE_ID 0x03

/* states of ModBus state machine */
typedef enum {
	MB_STATE_INITIAL,           // not used (reserved)
	MB_STATE_IDLE,              // ready to receive, do nothing
	MB_STATE_RECEPTION,         // receiving data, saving to array
	MB_STATE_EMISSION,          // data transfer initiation (start transmitte)
	MB_STATE_FRAME_CHECKING,    // check ID, check CRC
	MB_STATE_PROCESSING_REPLY   // processing query functions, transmitte array filling
	
}mb_state;
mb_state MB_STATE;


/**
 *@brief     USART Receiver buffer                      
 */                                             
#define RX_BUFFER_SIZE 100  // max array size for function 0x10 (write multiple registers). 
// Able to simultaneously write 45 registers
// (max byte number = 45*2 +10 bytes of system info like ID, fun number, starting address, number of registers, CRC)
uint8_t rx_buffer[RX_BUFFER_SIZE];
uint8_t rx_buf_index = 0;

/**
 *@brief     USART Transmitter buffer                      
 */
#define TX_BUFFER_SIZE 100
uint8_t tx_buffer[TX_BUFFER_SIZE];
uint8_t tx_buf_index = 0;
/* Variable "tx_buf_bytes_number" is necessary to send only the generated response message. 
Since the answer can be of different lengths, 
and the size of the buffer is designed for a response of maximum length.
This variable is global and is used in the body of the interrupt handler when the byte is sent by USART.
Such a variable is not needed for the receive buffer, since the end of the frame is determined by a timer interrupt 
and the current index counter is equal to the number of bytes received.
This is used to access the last two bytes of the message to retrieve the checksum value*/
uint8_t tx_buf_bytes_number = 0;


/*the first byte of the received sequence always contains the slave ID number*/
#define ADDRESS_FIELD  rx_buffer[0] 
/* the second byte of the received sequence always contains the function number */
#define FUNCTION_FIELD rx_buffer[1] 

/* flags */
int8_t frame_check_result = 250; // -1  - the message is not addressed to me (ID does not match), 0 - ID is the same, error in the CRC, 1 - ID is the same, CRC is OK, 250 is initial value
uint8_t transmittion_finish = 1; // 1 - finished, 0 - not finished
uint8_t processing_queue = 0;    // 1 - there is new data in the receive buffer, 0 - receive buffer is clear


/**
 *@brief    Modbus initialization
 *          UART init, timer0 init, clear all buffers
 */
void mbInit(void);


/**
 *@brief    service function
 *@return   Current Modbus state (mb_state - typedef enum)
 *@note     Modbus states specified in modbus.h
 */
mb_state mbGetState (void);

/**
 *@brief    rx_buffer clearing (FOR cycle inside)
 */
void mbClearRecieveBuffer (void);

/**
 *@brief    tx_buffer clearing (FOR cycle inside)
 */
void mbClearTransmitBuffer (void);

/**
 *@brief      CRC16 calculation, regular (non-table) method
 *@params     msg* - pointer to message array, msg_length - number of bytes in message (without CRC_low, CRC_high)
 *@return     16bit CRC. LSB first: CRC_low|CRC_high
 *@note		  This function should be called only after the buffer is completely filled (CRC is not calculated on the fly)
 */
uint16_t mbCRCCalc (uint8_t* msg, uint8_t msg_length);

/*
 *@brief     Function check received frame ( is slave ID matches, is CRC OK)
 *@return    -1 - the message is not addressed to me (ID does not match), 
 *            0 - ID is the same, error in the CRC, 
 *			  1 - ID is the same, CRC is OK, 250 is initial value
 */
int8_t mbFrameCheck( void );

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
void mbFun03Execution (void);

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
void mbFun06Execution (void);

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
void mbFun10Execution (void);


/**
 *@brief    This function checks the second byte of the request (received buffer) 
 *          in which the modbus function number is stored. 
 *          And calls the corresponding handler (mbFun03Execution, mbFun06Execution, mbFun10Execution) 
 */
void mbReplyProcessing ( void );




#endif /* INCLUDE_MODBUS_H_ */