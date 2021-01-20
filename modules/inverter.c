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
#include "inverter.h"
#include "resourceAssignNShare.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
// FTM defines 
#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)


// interrupt defines
#define INV_IRQ FTM3_IRQn
#define INV_IRQ_HANDLER FTM3_IRQHandler
#define FTM_INT_TYPE kFTM_TimeOverflowInterruptEnable

/*******************************************************************************
 * Enums
 ******************************************************************************/

/*******************************************************************************
 * Structs
 ******************************************************************************/

 /*******************************************************************************
 * Prototypes
 ******************************************************************************/
static float MaxAmpModIndAdj();
static float FreqModAdj(float fundFreq, float pwmFreq);
static void FtmModification(FTM_Type *base);
static void UpdateDsChngFreq(modulated_sig_char_t* modSigChar, switching_sig_char_t* pwmSigChar);
static void EnableInterrupts();
static void InitFTMChannels();
static void InitSignalChar(modulated_sig_char_t* modSigChar, switching_sig_char_t* pwmSigChar);

/*******************************************************************************
 * Variables
 ******************************************************************************/
// FTM driver variables and config
//static FTM_Type* ftmBase[FTM_NUMBERS] = {FTM_BASE0,FTM_BASE1};
static ftm_pwm_level_select_t pwmLevel = kFTM_HighTrue;
static ftm_deadtime_prescale_t dTimePreScalar = kFTM_Deadtime_Prescale_4;
static ftm_config_t ftmConfig;
static ftm_chnl_pwm_signal_param_t ftmParams[6];

// inverter driver variables and config
modulated_sig_char_t modSignalChar;
switching_sig_char_t pwmSignalChar;


/*******************************************************************************
 * Code
 ******************************************************************************/

/*! @brief starts the FTM timers for inverter switches */
void StartInverterControl()
{
		FTM_StartTimer(FTM_BASE_INV, kFTM_SystemClock);
}

/*! @brief stops the FTM timers for inverter switches */
void StopInverterControl()
{
		FTM_StopTimer(FTM_BASE_INV);
}

/*! @brief if the duty cycle of each phase isnt updated in each PWM period, this function adjusts the fundamental frequency
so that the frequency modulation index is an odd multiple of 3 and then updates the duty cycles of all the inverter switches for the 3 phases 
in accordance with the updated fundamental frequency*/
void InverterControl()
{
	if(!processDenotions.dsUpdated)
		{
			modSignalChar.fundFreq = FreqModAdj(modSignalChar.fundFreq,pwmSignalChar.switchFreq); 
			modSignalChar.fundPeriod = (1.0f/modSignalChar.fundFreq);
			modSignalChar.freqModIndex = pwmSignalChar.switchFreq/modSignalChar.fundFreq;
			UpdateDsChngFreq(&modSignalChar, &pwmSignalChar);
		}
}
	
/*! @brief  initialize and configure FTM channels in accordance with the physical layout of the high side and low side switches,
inserting a suitable dead time in accordance with the physical switch limitations and enabling the FTM interrupts*/
static void InitFTMChannels()
{
	FTM_GetDefaultConfig(&ftmConfig);
	ftmConfig.reloadPoints = kFTM_CntMin;
	ftmConfig.deadTimePrescale = dTimePreScalar;
	ftmConfig.deadTimeValue = DEADTIMETICKS;
	
	 for(int a=0;a<2;a++)
		{
			ftmParams[a].chnlNumber            = a;
			ftmParams[a].level                 = pwmLevel;
			ftmParams[a].dutyCyclePercent      = OFFDS;
			ftmParams[a].firstEdgeDelayPercent = 0U;
			ftmParams[a].enableDeadtime        = true; 
		}
		
	 for(int a=4;a<8;a++)
		{
			ftmParams[a-2].chnlNumber            = a;
			ftmParams[a-2].level                 = pwmLevel;
			ftmParams[a-2].dutyCyclePercent      = OFFDS;
			ftmParams[a-2].firstEdgeDelayPercent = 0U;
			ftmParams[a-2].enableDeadtime        = true; 
		}
			
	FTM_Init(FTM_BASE_INV, &ftmConfig);
	FtmModification(FTM_BASE_INV);
	FTM_SetupPwm(FTM_BASE_INV, ftmParams, FTM_INV_CHNLS, kFTM_CenterAlignedPwm, PWM_FREQ, FTM_SOURCE_CLOCK);
		
  EnableInterrupts();
			
}

/*!
@brief update the modulated and switching signal charasteristics 
@param modulated signal properties structure
@param switching signal properties structure
*/
static void InitSignalChar(modulated_sig_char_t* modSigChar, switching_sig_char_t* pwmSigChar)
{
	pwmSigChar->switchFreq = PWM_FREQ;
	pwmSigChar->switchPeriod = (1.0f/(float)PWM_FREQ);
	
	modSigChar->curPhAngle = 0;
	modSigChar->fundFreq = INIT_FUND_FREQ;
	modSigChar->fundPeriod = (1.0f/(float)INIT_FUND_FREQ);
	modSigChar->harOrder = HAR_ORDER;
	modSigChar->harScalar = HAR_SCA_NUM/HAR_SCA_DEN;
	modSigChar->ampModDifference = MaxAmpModIndAdj();
	modSigChar->ampModIndex = AMP_MOD_IND; // for now, have to change 
}

/*! @brief enable FTM interrupts through FTM drivers */
static void EnableInterrupts()
{
	FTM_EnableInterrupts(FTM_BASE_INV, FTM_INT_TYPE);
	EnableIRQ(INV_IRQ);

}

/*! @brief initialize inverter switches with starting signals and initializes FTM channels*/
void InitInverter()
{
	InitFTMChannels();
	InitSignalChar(&modSignalChar, &pwmSignalChar);
	modSignalChar.fundFreq = FreqModAdj(modSignalChar.fundFreq,pwmSignalChar.switchFreq); 
	modSignalChar.fundPeriod = (1.0f/modSignalChar.fundFreq);
	modSignalChar.freqModIndex = pwmSignalChar.switchFreq/modSignalChar.fundFreq;
	UpdateDsChngFreq(&modSignalChar,&pwmSignalChar);
	FTM_SetSoftwareTrigger(FTM_BASE_INV, true);
	processDenotions.dsUpdated = false;
}

/*! @brief  modify FTM registers for complementary PWM and global time for all FTMS*/
static void FtmModification(FTM_Type *base)
{
  base->SYNCONF &= ~FTM_SYNCONF_SWRSTCNT_MASK;
	base->COMBINE |= FTM_COMBINE_COMP2(1) | FTM_COMBINE_DTEN2(1); 
	base->COMBINE |=  FTM_COMBINE_COMP3(1) | FTM_COMBINE_DTEN3(1); 
	base->COMBINE |=  FTM_COMBINE_COMP0(1) | FTM_COMBINE_DTEN0(1); 
}

/*!
@brief calculates the phase angle midway into the next PWM period and calculates and updates the corresponding duty cycles of the 3 phases
@param modulated signal properties structure
@param switching signal properties structure
*/
static void UpdateDsChngFreq(modulated_sig_char_t* modSigChar, switching_sig_char_t* pwmSigChar)
{
  uint32_t perDivisions = (modSigChar->fundPeriod/pwmSigChar->switchPeriod);
	float stepPhase = pwmSigChar->stepPhase = (float)(PER_ANGLE/perDivisions);
	float curPhAngle = modSigChar->curPhAngle;
	float harScalar = modSigChar->harScalar; 
	
	
	if(curPhAngle == 0)
		curPhAngle = stepPhase/2;
	else if(!processDenotions.freqChange)
		curPhAngle += stepPhase;
	else if(processDenotions.freqChange)
		curPhAngle = (int)(curPhAngle/stepPhase)+(stepPhase/2);
	
	if(curPhAngle > PER_ANGLE)
		curPhAngle = curPhAngle - PER_ANGLE;
	
	modSigChar->curPhAngle = curPhAngle;
	
	#if (HARMONIC_INJ_EN)
		modSigChar->modSigU = sinf((float)(curPhAngle)/57.2958f) + harScalar*sinf((float)(HAR_ORDER*curPhAngle)/57.2958f);
		modSigChar->modSigV = sinf((float)(curPhAngle-120)/57.2958f) + harScalar*sinf((float)(HAR_ORDER*(curPhAngle-120))/57.2958f);
		modSigChar->modSigW = sinf((float)(curPhAngle+120)/57.2958f) + harScalar*sinf((float)(HAR_ORDER*(curPhAngle+120))/57.2958f);
	#else
		modSigChar->modSigU = sinf((float)(curPhAngle)/57.2958f);
		modSigChar->modSigV = sinf((float)(curPhAngle-120)/57.2958f);
		modSigChar->modSigW = sinf((float)(curPhAngle+120)/57.2958f);
	#endif

	

	modSigChar->DsU = ((modSigChar->modSigU)*100*0.5)*modSigChar->ampModIndex+50;
	modSigChar->DsV = ((modSigChar->modSigV)*100*0.5)*modSigChar->ampModIndex+50;
	modSigChar->DsW = ((modSigChar->modSigW)*100*0.5)*modSigChar->ampModIndex+50;
	
																																///////////////////////////////////
	
	FTM_UpdatePwmDutycycle(FTM_BASE_INV, UH, kFTM_CenterAlignedPwm, modSigChar->DsU);
	FTM_UpdatePwmDutycycle(FTM_BASE_INV, VH, kFTM_CenterAlignedPwm, modSigChar->DsV);
	FTM_UpdatePwmDutycycle(FTM_BASE_INV, WH, kFTM_CenterAlignedPwm, modSigChar->DsW);
	
	processDenotions.dsUpdated = true;
}

/*! @brief calcualtes maximum amplitude modulation index so as to incorporate the deadtime in the switching period */
static float MaxAmpModIndAdj()
{
	float timerClck = 0, deadTimeTicks = 0, temp = 0;
	float ampModDif=0, insuranceTicks = 5;
	timerClck = SystemCoreClock/2;
  deadTimeTicks = (FTM_BASE_INV->DEADTIME & (3<<6))>>6;
	switch ((int)deadTimeTicks)
	{
		case 2:
			deadTimeTicks = 4;
			break;
		case 3:
			deadTimeTicks = 16;
			break;
		default:
			deadTimeTicks = 1;
			break;
	}
	deadTimeTicks = (deadTimeTicks*(FTM_BASE_INV->DEADTIME & (0x3F))+insuranceTicks);
	ampModDif =  (deadTimeTicks/(timerClck/pwmSignalChar.switchFreq));
	return ampModDif;
}

/*! @brief adjusts the fundamental frequency so the frequency modulation index if an odd integer of 3 to avoid even harmonics and triplen harmonics 
in the output signal, the frequency is shifted to a lower value so as to maintain a const v/f ratio, where V has an upper limit coming from the PV panel*/
static float FreqModAdj(float fundFreq, float pwmFreq)
{
	float lLimit = 0,uLimit = 0,fundPer = (1.0f/fundFreq), tempPer = fundPer, pwmPer = (1.0f/pwmFreq); 
	
	if(fmod((pwmFreq/fundFreq),3) == 0 && fmod((pwmFreq/fundFreq),2) != 0)
		return fundFreq;
	else
	{
		while(!(fmod((tempPer/pwmPer),3) < pwmPer && fmod((tempPer/pwmPer),2) != 0))
			tempPer += pwmPer/10000;
		
		uLimit = tempPer;
		tempPer = fundPer;
		
//		while(!(fmod((tempPer/pwmPer),3) < pwmPer && fmod((tempPer/pwmPer),2) != 0) && tempPer > 0)
//			tempPer -= pwmPer/10000;
//		lLimit = tempPer;
		
		if(lLimit != 0)
			fundPer = (fundPer-lLimit < uLimit-fundPer) ? lLimit:uLimit;
		else
			fundPer = uLimit;
	}
	fundFreq = 1.0f/fundPer;
	
	return fundFreq;
}
	
void INV_IRQ_HANDLER()
{
	
	FTM_SetSoftwareTrigger(FTM_BASE_INV, true);
	processDenotions.dsUpdated = false;
	
	if ((FTM_GetStatusFlags(FTM_BASE_INV) & kFTM_TimeOverflowFlag) == kFTM_TimeOverflowFlag)
	{
		/* Clear interrupt flag.*/
		FTM_ClearStatusFlags(FTM_BASE_INV, kFTM_TimeOverflowFlag);
	}
	__DSB();
}





/* EOF */






   


	

