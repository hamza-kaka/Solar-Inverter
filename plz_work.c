#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_ftm.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "math.h"


#define FTM_base FTM3
#define FTM_chnl_2 kFTM_Chnl_4
#define FTM_chnl_1 kFTM_Chnl_1

#define PWM_inr_no FTM3_IRQn
#define PWM_inr_handler FTM3_IRQHandler

#define FTM_inr_enable kFTM_Chnl4InterruptEnable
#define FTM_chnl_flg kFTM_Chnl4Flag

#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)

#define PWM_freq 5000U
#define out_frq 50U
#define dvsns_hlf_wv ((PWM_freq/out_frq)/2)

volatile float sin_oprnd;
volatile float div_mid;
volatile float DS[dvsns_hlf_wv] = {0};
volatile int count=0; 



 
void PWM_inr_handler()
{
	
	FTM_UpdatePwmDutycycle(FTM_base, FTM_chnl_2, kFTM_CenterAlignedPwm, DS[count]);
	FTM_UpdatePwmDutycycle(FTM_base, FTM_chnl_1, kFTM_CenterAlignedPwm, DS[count]);
	FTM_SetSoftwareTrigger(FTM_base, true);
	
	if ((FTM_GetStatusFlags(FTM_base) & FTM_chnl_flg) == FTM_chnl_flg)
    {
        /* Clear interrupt flag.*/
        FTM_ClearStatusFlags(FTM_base, FTM_chnl_flg);
    }
		
		if(count == dvsns_hlf_wv-1)
			count=0;
		else
			count++;
		
    __DSB();
}
	
void delay ()
{
  volatile uint32_t i = 0U;
    for (i = 0U; i < 8000U; ++i)
    {
        __asm("NOP"); /* delay */
    }
}
int main()
{
	
    BOARD_InitBootClocks();
    BOARD_InitBootPins();
	  

	  sin_oprnd =(float)(180/(float)(dvsns_hlf_wv));
	  div_mid  = (float)((180/(float)(dvsns_hlf_wv))/2);
	
	  for(int a=0;a<dvsns_hlf_wv;a++)
			DS[a] = (sin((float)(sin_oprnd*a + div_mid)/57.2958))*100;
	
		ftm_config_t ftmInfo;
    ftm_chnl_pwm_signal_param_t ftmParam[2];
    ftm_pwm_level_select_t pwmLevel = kFTM_HighTrue;
	
	
  	ftmParam[0].chnlNumber            = FTM_chnl_1;
    ftmParam[0].level                 = pwmLevel;
    ftmParam[0].dutyCyclePercent      = DS[count];
    ftmParam[0].firstEdgeDelayPercent = 0U;
    ftmParam[0].enableDeadtime        = false;
	
	  ftmParam[1].chnlNumber            = FTM_chnl_2;
    ftmParam[1].level                 = pwmLevel;
    ftmParam[1].dutyCyclePercent      = DS[count];
    ftmParam[1].firstEdgeDelayPercent = 0U;
    ftmParam[1].enableDeadtime        = false;
	
	
	FTM_GetDefaultConfig(&ftmInfo);
	FTM_Init(FTM_base, &ftmInfo);
	FTM_SetupPwm(FTM_base, ftmParam, 2U, kFTM_CenterAlignedPwm, PWM_freq, FTM_SOURCE_CLOCK);
	
	FTM_EnableInterrupts(FTM_base, FTM_inr_enable);
	EnableIRQ(PWM_inr_no);
	
	FTM_StartTimer(FTM_base, kFTM_SystemClock);
	
	
	while(1)
	{
//		delay();
//	  FTM_UpdatePwmDutycycle(FTM_base, FTM_chnl_1, kFTM_CenterAlignedPwm, DS[count]);
//		FTM_UpdatePwmDutycycle(FTM_base, FTM_chnl_2, kFTM_CenterAlignedPwm, DS[count]);
//		FTM_SetSoftwareTrigger(FTM_base, true);
//		if(count == dvsns_hlf_wv-1)
//			count=0;
//		else
//			count++;
	}
		
	
}