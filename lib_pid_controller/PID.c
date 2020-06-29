

void pidInit (pidContext_t *ctx)
{
	ctx->err        = 0;
	ctx->prev_err   = 0;
	ctx ->I_factor  = 0;	
}

float pidCalculation (pidContext_t *ctx)
{
	float pid_out = 0;
	
	ctx->P_factor = ctx->kp * ctx->err;
	ctx->I_factor += ctx->ki * ctx->err;  // clip -100....100 or 0...100
	ctx->D_factor = ctx->kd * (ctx->err - ctx->prev_err);	
	
	pid_out = ctx->P_factor + ctx->I_factor + ctx->D_factor; // clip -100....100 or 0...100
	
	ctx->prev_err = ctx->err;
	
	return pid_out;
}