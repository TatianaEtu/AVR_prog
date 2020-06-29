#ifndef INCLUDE_COMMON_H_
#define INCLUDE_COMMON_H_


/* UINT16_t*/
#define MB_ADR_VAR1                         0
/* FLOAT */
#define MB_ADR_TEMPERATURE                  5  //0
#define MB_ADR_SET_POINT                    7  //1
#define MB_ADR_PID_CONTROL_ERROR            9  //2
#define MB_ADR_PID_CONTROL_P_FACTOR         11 //3
#define MB_ADR_PID_CONTROL_I_FACTOR         13 //4
#define MB_ADR_PID_CONTROL_D_FACTOR         15 //5
#define MB_ADR_PID_CONTROL_OUT              17 //6
#define MB_ADR_PID_CONTROL_KP_COEFF         19 //7
#define MB_ADR_PID_CONTROL_KI_COEFF         21 //8
#define MB_ADR_PID_CONTROL_KD_COEFF         23 //9



/************************************************************************/
/*        All modbus uint16_t variables definition                      */
/************************************************************************/

uint16_t mb_uint16_variables_arr [5];
#define  VAR1  mb_uint16_variables_arr[0]






/************************************************************************/
/* All modbus float variables definition                                */
/************************************************************************/

/**
 *@brief    All float type variables are declared as union data type.
 *			This is necessary to easily access each of the four bytes of the float variable
 */
typedef union{
	float f;       // float variable
	uint8_t u[4];  // an array of four byte elements
}mb_float_type;    // new data type name

mb_float_type union_arr[20];
 
#define TEMPERATURE                union_arr[0]
#define SET_POINT                  union_arr[1]
#define PID_CONTROL_ERROR          union_arr[2]
#define PID_CONTROL_P_FACTOR       union_arr[3]
#define PID_CONTROL_I_FACTOR       union_arr[4]
#define PID_CONTROL_D_FACTOR       union_arr[5]
#define PID_CONTROL_OUT            union_arr[6]




/**
 *@brief    Modbus memory card is an array of uint16_t values.
 *          In every cell of the array the register is stored.
 *          Register addresses from 0 to (uint16_reg_array_len-1).
 *          At the beginning of the array registers of type uint16_t are located, 
 *          then variables of type float are located (each variable takes 2 registers).    
 */
#define uint16_reg_array_len       45  // total number of registers
#define uint16_var_reg_num         5   // number of float variables
#define MB_REG_ADR_FLOAT_BEGINING  (uint16_var_reg_num ) // address where float variables start (adress bias)
uint16_t uint16_reg_array [uint16_reg_array_len]; // modbus registers array





#endif /*INCLUDE_COMMON_H_*/