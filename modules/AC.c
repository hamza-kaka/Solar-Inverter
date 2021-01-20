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
#include "AC.h"
#include "resourceAssignNShare.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
// FTM defines 
#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)

// FTM interrupt defines
#define AC_BOOST_IRQ FTM1_IRQn
#define AC_BOOST_IRQ_HANDLER FTM1_IRQHandler
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
// For FTM
static void FtmModification(FTM_Type *base);
static void InitAcFtm();
static void InitRelayPin();
static void AcBstParamUpdate();

/*******************************************************************************
 * Variables
 ******************************************************************************/
 //AC boost variables and config
 static ac_boost_t acBoost = {0,0,0}; 
 
// FTM driver variables and config
static ftm_pwm_level_select_t pwmLevel = kFTM_HighTrue;
static ftm_config_t ftmConfig;
static ftm_chnl_pwm_signal_param_t ftmParams[2];
 
 //Relay GPIO driver 
static gpio_pin_config_t relayConfig = { kGPIO_DigitalOutput, RELAY_OFF};

/*******************************************************************************
 * Code
 ******************************************************************************/

void InitAcBlock()
{
	InitAcFtm();
	InitRelayPin();
	
}
	


/*! @brief starts the FTM timers for MPPT BOOST switches */
void StartAcControl()
{
	
	FTM_StartTimer(FTM_BASE_AC, kFTM_SystemClock);
  GPIO_PortSet(RELAY_GPIO_BASE,PIN_SHIFT);
	
  AcBstParamUpdate();
	
}


/*! @brief stops the FTM timers for MPPT BOOST switches */
void StopAcControl()
{
	FTM_StopTimer(FTM_BASE_AC);
	GPIO_PortClear(RELAY_GPIO_BASE,PIN_SHIFT);
}


/*! @brief  initialize and configure FTM channels in accordance with the physical layout of the high side and low side switches,
inserting a suitable dead time in accordance with the physical switch limitations and enabling the FTM interrupts*/
static void InitAcFtm()
{
	FTM_GetDefaultConfig(&ftmConfig);
	ftmConfig.reloadPoints = kFTM_CntMin;
	ftmConfig.bdmMode = kFTM_BdmMode_3;
	
	for(int a=0;a<2;a++)
	{
		ftmParams[a].chnlNumber            = a;
		ftmParams[a].level                 = pwmLevel;
		ftmParams[a].dutyCyclePercent      = OFFDS;
		ftmParams[a].firstEdgeDelayPercent = 0U;
		ftmParams[a].enableDeadtime        = false; 
	}

	FTM_Init(FTM_BASE_AC, &ftmConfig);
	FtmModification(FTM_BASE_AC);
	FTM_SetupPwm(FTM_BASE_AC, ftmParams, AC_BST_CHNLS, kFTM_CenterAlignedPwm, AC_BST_FREQ, FTM_SOURCE_CLOCK);
}

/*! @brief  modify FTM registers for minimum count loading point*/
static void FtmModification(FTM_Type *base)
{
  base->SYNCONF &= ~FTM_SYNCONF_SWRSTCNT_MASK;

}

void InitRelayPin()
{
	 GPIO_PinInit(RELAY_GPIO_BASE, RELAY_PIN, &relayConfig);
}

static void AcBstParamUpdate()
{
	acBoost.dsBst1 += acBoost.stepDs; 
	acBoost.dsBst2 += acBoost.stepDs;
	
	FTM_UpdatePwmDutycycle(FTM_BASE_AC, AC_BST_CHNL_0, kFTM_CenterAlignedPwm, acBoost.dsBst1);
	FTM_UpdatePwmDutycycle(FTM_BASE_AC, AC_BST_CHNL_1, kFTM_CenterAlignedPwm, acBoost.dsBst2);
	
	FTM_SetSoftwareTrigger(FTM_BASE_AC, true);
}

/* EOF */