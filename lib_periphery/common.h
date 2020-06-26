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
#define  VAR1         mb_uint16_variables_arr[0]






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
#define PID_OUT                    union_arr[6]










#endif /*INCLUDE_COMMON_H_*/