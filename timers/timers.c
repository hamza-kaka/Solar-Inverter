/*! 
@file 
@brief 
@details 

@author Hamza Naeem Kakakhel
@copyright Taraz Technologies Pvt. Ltd.
*/
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "resourceAssignNShare.h"
#include "fsl_common.h"
#include "timers.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
 /*! Macro to convert a microsecond period to raw count value */
#define MICROSEC_TO_COUNT(us, clockFreqInHz) (uint64_t)(((float)(us) * (float)(clockFreqInHz)) / 1000000.0f)
	
/*******************************************************************************
 * Enums
 ******************************************************************************/

/*******************************************************************************
 * Structs
 ******************************************************************************/

 /*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
 // PIT shared timer
pit_config_t pitConfig;	

/*******************************************************************************
 * Code
 ******************************************************************************/

void PIT_HANDLER()
{
    /* Clear interrupt flag.*/
    PIT_ClearStatusFlags(PIT, kPIT_Chnl_0, kPIT_TimerFlag);
		processDenotions.pitTimerdone = true;
    /* Added for, and affects, all PIT handlers. For CPU clock which is much larger than the IP bus clock,
     * CPU can run out of the interrupt handler before the interrupt flag being cleared, resulting in the
     * CPU's entering the handler again and again. Adding DSB can prevent the issue from happening.
     */
    __DSB();
}

void InitPit()
{
	PIT_GetDefaultConfig(&pitConfig);
	PIT_Init(PIT, &pitConfig);
	PIT_EnableInterrupts(PIT, kPIT_Chnl_0, kPIT_TimerInterruptEnable);
	EnableIRQ(PIT_IRQ);
}

void ConfigureTimer( float uSeconds)
{
	int test = MICROSEC_TO_COUNT(uSeconds, PIT_SOURCE_CLOCK);//USEC_TO_COUNT(uSeconds, PIT_SOURCE_CLOCK);//
	PIT_SetTimerPeriod(PIT, kPIT_Chnl_0, test);
}
 
void startPauseTimer()
{
	PIT_StartTimer(PIT, kPIT_Chnl_0);
	while (!processDenotions.pitTimerdone)
	{}
	processDenotions.pitTimerdone = false;
	stopTimer();
}

void startTimer()
{
	PIT_StartTimer(PIT, kPIT_Chnl_0);
}

void stopTimer()
{
	PIT_StopTimer(PIT, kPIT_Chnl_0);
}

bool CheckPitFlag()
{
	return PIT_GetStatusFlags(PIT, kPIT_Chnl_0);
}


/* EOF */