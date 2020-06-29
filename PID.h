#ifndef INCLUDE_PID_CONTROLLER_H
#define INCLUDE_PID_CONTROLLER_H


typedef struct pidContext_t{
	float kp = 0.0;
	float ki = 0.0;
	float kd = 0.0;
	
	float err;
	float prev_err;
	
	float P_factor;
	float I_factor;
	float D_factor;
	
	float integer_limit;
	float integer_bias;
} pidContext_t;


void pidInit (pidContext_t *ctx);


float pidCalculation (pidContext_t *ctx);









#endif /*INCLUDE_PID_CONTROLLER_H*/