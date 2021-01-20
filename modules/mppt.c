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
#include "mppt.h"
#include "resourceAssignNShare.h"
#include "timers.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
// ADC interrupt defines 
#define ADC0_INT ADC0_IRQn
#define ADC1_INT ADC1_IRQn
#define ADC0_IRQ_HANDLER ADC0_IRQHandler
#define ADC1_IRQ_HANDLER ADC1_IRQHandler

// ADC channel defines 
#define CHNL_CONFIG_NO_I 0
#define CHNL_CONFIG_NO_V_PV 1
#define CHNL_CONFIG_NO_V_DC 2


// FTM defines 
#define FTM_SOURCE_CLOCK CLOCK_GetFreq(kCLOCK_BusClk)

// FTM interrupt defines
#define BOOST_IRQ FTM1_IRQn
#define BOOST_IRQ_HANDLER FTM1_IRQHandler
#define FTM_INT_TYPE kFTM_TimeOverflowInterruptEnable

//MPPT defines 
#define MPPT_BST_LIM (95)

/*******************************************************************************
 * Enums
 ******************************************************************************/

/*******************************************************************************
 * Structs
 ******************************************************************************/

 /*******************************************************************************
 * Prototypes
 ******************************************************************************/
// For ADC
static void EnableAdcInterrupts();
static void InitMpptAdc();
static void InitAdcChannel(int chnlNo, adc16_channel_config_t* adcChannelConfig);
static void AdcModifications(adc16_config_t* adcConfig);
static void AdcCalibration(ADC_Type* adcBase);
static void AdcConversion(ADC_Type* adcBase,int chnlGrp,adc16_channel_config_t* adcChannelConfig);
static float AdcToTransducer(adc_output_t* adcOutputs);
static float AdcToRealValConv(adc_output_t* adcOutputs, char configNo);


// For FTM
static void FtmModification(FTM_Type *base);
static void EnableFtmInterrupts();
static void InitMpptFtm();

/*******************************************************************************
 * Variables
 ******************************************************************************/
// ADC driver structs and config
static adc16_config_t adcConfig;
static adc16_channel_config_t adcChannelConfig[NO_ADC_INPUTS];
static adc16_clock_divider_t adcClockDiv = kADC16_ClockDivider8; // ADCK between 2-12 MHz for 16 bit
static adc16_hardware_average_mode_t hwAverageMode = kADC16_HardwareAverageDisabled;// kADC16_HardwareAverageCount4;// 
static adc16_long_sample_mode_t sampleTime = kADC16_LongSampleCycle24; // kADC16_LongSampleDisabled;// 
static ADC_Type* adcBase[2] = {MPPT_ADC_BASE_0,MPPT_ADC_BASE_1};
static int adcChnlNum[NO_ADC_INPUTS] = {MPPTIPV_CHNL, MPPTVPV_CHNL, MPPTVDC_CHNL};

// MPPT driver structs and config
static mppt_params_t mpptParams;
static adc_output_t adcOutputs;	
static mppt_boost_t mpptBoost = {0,0,0.1}; 

// FTM driver variables and config
static ftm_pwm_level_select_t pwmLevel = kFTM_HighTrue;
static ftm_config_t ftmConfig;
static ftm_chnl_pwm_signal_param_t ftmParams[2];

// temporary 
bool dir=0;
volatile int count = 0;
volatile int near =0;
volatile int temp=0;
volatile float point=0,dcavg=0,curavg=0,curHigh = 0,curLow = 0, pvavg =0, curMed=0;
float thresh[30000]={0},currarr[400] = {0},output[11000] ={0}, curSampArr[10]={0}, ar[3]={0};
float mpptEfficiency=0;
float dynamicEff = 0;
volatile float maxppv = 0;
volatile float minppv = 0;
volatile float threshold = 0;

/*******************************************************************************
 * Code
 ******************************************************************************/
void MpptFault()
{
	FTM_UpdatePwmDutycycle(FTM_BASE_MPPT, BST_CHNL_0, kFTM_CenterAlignedPwm, OFFDS);
	FTM_UpdatePwmDutycycle(FTM_BASE_MPPT, BST_CHNL_1, kFTM_CenterAlignedPwm, OFFDS);
	
	FTM_SetSoftwareTrigger(FTM_BASE_MPPT, true);
}

/*! @brief starts the FTM timers for MPPT BOOST switches */
void StartMpptControl()
{
	FTM_StartTimer(FTM_BASE_MPPT, kFTM_SystemClock);
//	mpptParams.stepVoltage = CalcStepVolt();
	InitBoostDs();
}

/*! @brief stops the FTM timers for MPPT BOOST switches */
void StopMpptControl()
{
	FTM_StopTimer(FTM_BASE_MPPT);
}

void InitMppt()
{
	InitMpptFtm();
	InitMpptAdc();
	
}

 void MpptSensorInput()
{
	float tempPv = 0, tempDc = 0, tempcur = 0, avgSmplCur = 0, aquisitionDelay  = 0, curAccBandwidth = 0.01;
	int sampleNo = 10,effectiveSamples = 0; 
	
	if(mpptBoost.dsBst1 <=  50)
	{
		aquisitionDelay = (1.0/(float)BST_FREQ)*1000000 -3-5.25;// first 3 is for delay after ftm overflow flag and the 5 is the actual PIT delaty for the starting of ADC sample time
	}
	else 
		aquisitionDelay = (0.5/(float)BST_FREQ)*1000000 -3-5.25;
	
	ConfigureTimer(aquisitionDelay);
	
	 for(int a=0;a<sampleNo;a++)
	{
		if ((FTM_GetStatusFlags(FTM_BASE_MPPT) & kFTM_TimeOverflowFlag) == kFTM_TimeOverflowFlag)
	{
		FTM_ClearStatusFlags(FTM_BASE_MPPT, kFTM_TimeOverflowFlag);
	}
	FTM_EnableInterrupts(FTM_BASE_MPPT, FTM_INT_TYPE);
	
	while(!processDenotions.bstCycleRefresh)
	{}
		processDenotions.bstCycleRefresh = false;
		
		startPauseTimer();
		
		GPIOE->PCOR = 1<<5;////////////////////////
		
		for(int b=0; b<2;b++)
		{
			if(b)
			{
				if(!(a%2))
					AdcConversion(adcBase[b],MPPT_ADC_CHAN_GRP,&adcChannelConfig[CHNL_CONFIG_NO_V_PV]);	
				else 
					AdcConversion(adcBase[b],MPPT_ADC_CHAN_GRP,&adcChannelConfig[CHNL_CONFIG_NO_V_DC]);	
			}
			else
				AdcConversion(adcBase[b],MPPT_ADC_CHAN_GRP,&adcChannelConfig[CHNL_CONFIG_NO_I]);	
		}
			while (!(processDenotions.adcdone[0] && processDenotions.adcdone[1])) // 
			{				
			}

			GPIOE->PDOR = 1<<5;////////////////////
			
			adcOutputs.adcOut = ADC16_GetChannelConversionValue(adcBase[1], MPPT_ADC_CHAN_GRP);
			if(!(a%2))
			tempPv += adcOutputs.adcOut;
			else
			tempDc += adcOutputs.adcOut;
			
			adcOutputs.adcOut = ADC16_GetChannelConversionValue(adcBase[0], MPPT_ADC_CHAN_GRP);
			tempcur += adcOutputs.adcOut;
			curSampArr[a] = adcOutputs.adcOut;
			
			if(!a)
			{
				curHigh = adcOutputs.adcOut;
				curLow = adcOutputs.adcOut;
			}
				if(adcOutputs.adcOut < curHigh) // the signs are like this because current transducer is fixed in the opp diection which makes the cur value negative
				curHigh = adcOutputs.adcOut;
			if(adcOutputs.adcOut > curLow)
				curLow = adcOutputs.adcOut;
	
			
	}
		
	
	

		avgSmplCur = tempcur = (tempcur-curLow-curHigh)/(sampleNo-2);// tempcur/sampleNo;// 
		curLow = tempcur-(curAccBandwidth*tempcur);
		curHigh = tempcur+(curAccBandwidth*tempcur);
		tempcur = 0;
	mean_based_sampling:
		for(int a=0; a<sampleNo; a++)
		{
			if(curSampArr[a]<curLow && curSampArr[a] > curHigh)
			{
				tempcur += curSampArr[a];
				effectiveSamples++;
			}
		}
		if (effectiveSamples==0)
		{
			curAccBandwidth+=0.005;
			curLow = avgSmplCur-(curAccBandwidth*avgSmplCur);
	  	curHigh = avgSmplCur+(curAccBandwidth*avgSmplCur);
			goto mean_based_sampling;
		}
			

		adcOutputs.adcOut  = tempcur/effectiveSamples;
		mpptParams.pvCurrent = AdcToRealValConv(&adcOutputs,CHNL_CONFIG_NO_I) ;
		adcOutputs.adcOut =  tempPv/(sampleNo/2);
		mpptParams.pvVoltage = AdcToRealValConv(&adcOutputs,CHNL_CONFIG_NO_V_PV);
		adcOutputs.adcOut =  tempDc/(sampleNo/2);
		mpptParams.dcLinkVoltage = AdcToRealValConv(&adcOutputs,CHNL_CONFIG_NO_V_DC);
			
//		if(count<300 && count > 49)
//		{
//			if(!count)
//				curLow = mpptParams.pvCurrent;
//			currarr[count] = mpptParams.pvCurrent;
//			curavg += mpptParams.pvCurrent;
//			pvavg += mpptParams.pvVoltage; 
//		  dcavg += mpptParams.dcLinkVoltage;
//			if(mpptParams.pvCurrent > curHigh)
//				curHigh = mpptParams.pvCurrent;
//			if(mpptParams.pvCurrent < curLow)
//				curLow = mpptParams.pvCurrent;
//		}
//		else if(count==300)
//		{
//			pvavg /=250; 
//			dcavg /=250; 
//			curavg /=250; 
//		}

//    ar[count%3] = mpptParams.pvCurrent;
//		if(count>=2)
//		{
//		  if(fabs(ar[0] - ar[1]) < fabs(ar[1] - ar[2])) 
//			{
//				if(fabs(ar[2] - ar[1]) < fabs(ar[0] - ar[2]))
//					curMed = ar[1];
//				else 
//					curMed = ar[0];
//			}
//			else if(fabs(ar[0] - ar[1]) > fabs(ar[0] - ar[2]))
//			{
//				curMed = ar[2];
//			}
//			else 
//				curMed = ar[1];
//			
//			mpptParams.pvCurrent = curMed;
//			
//			curavg = (ar[0]+ar[1]+ar[2])/3; 
//		}
		count++;
 
		
	


}

void MpptBoostControl()
{
	UpdateMpptParams();
	UpdateBoostDs();
}

   
void UpdateBoostDs()
{
	volatile float powerPerVolt, deltaConductance, pointConductance, delay, percentpow, prevpercentpow, effDeltaCond, effPointCond;
	
	deltaConductance = mpptParams.deltaPvCur/mpptParams.deltaPvVolt;
	pointConductance = mpptParams.pvCurrent/mpptParams.pvVoltage;
	powerPerVolt = mpptParams.deltaPvPower/mpptParams.deltaPvVolt;

	prevpercentpow = percentpow;
	percentpow = (mpptParams.deltaPvPower/mpptParams.prevPvPower)*100;
	
	delay = 2000.0;
	mpptBoost.stepDs = 0.75;

//	if((powerPerVolt > maxppv )&& powerPerVolt != INFINITY)
//		maxppv = powerPerVolt;
//	if((powerPerVolt < minppv )&& powerPerVolt != -INFINITY)
//		minppv = powerPerVolt;
//	
//	
//	if(deltaConductance < 0 && deltaConductance > -INFINITY)
//		threshold = (-deltaConductance - pointConductance)/(pointConductance);
	
//	thresh[count] = threshold;
//	if (threshold < 10 && threshold > -10)
//	{
//		mpptBoost.stepDs = 0.015;
//			delay = 700.0;
//	}
//	else 
//		mpptBoost.stepDs = 0.1;
		
////	threshold = (pointConductance/deltaConductance)*100;
//	if(count<30000)
//	thresh[count] = threshold;
//	count++; 
//	if(count>19000 && count<30000)
//	{
//		output[count-19000] = threshold;
//		if(threshold<0 && threshold>-INFINITY)
//			point += -threshold; 
//		else if(threshold>0 && threshold>INFINITY)
//			point += threshold;
//	}
//	if(count == 30000)	
//		point = point/11000;
//	if(count < 10000)
//	{
//	 ppv[count] = powerPerVolt;
//		pw[count] = mpptParams.pvPower;
//	}
	count++;
	
//	if((powerPerVolt <= 20 && powerPerVolt >= 0) || ( -powerPerVolt <=20 && -powerPerVolt >= 0 )) //(threshold<3.7 && threshold >-3.7) // 
//	{
//		
//		if(!near)
//			temp = count;
//		near++;
//		if(count - temp ==50 && near>20)
//		{
//			mpptBoost.stepDs = 0.05;
//			delay = 2000.0;
//			count = 0;
//			near = 0;
//			temp = 0;
//		}
//	}
//	else 
//	{
//		mpptBoost.stepDs = 0.2;
//		delay = 2000.0;
//	}
	

	
//	if(count !=0 && (percentpow > prevpercentpow))
//	{
//		mpptBoost.stepDs = 1.1*mpptBoost.stepDs;
//		if(mpptBoost.stepDs > 0.2)
//			mpptBoost.stepDs = 0.2;
//		else if(mpptBoost.stepDs<0.03)
//		{
//			mpptBoost.stepDs = 0.03;
//			delay = 800;
//		}
//	}
//	else if (count !=0 && (percentpow < prevpercentpow))
//	{
//		mpptBoost.stepDs = 0.9*mpptBoost.stepDs;
//		if(mpptBoost.stepDs > 0.2)
//			mpptBoost.stepDs = 0.2;
//		else if(mpptBoost.stepDs<0.03)
//		{
//			mpptBoost.stepDs = 0.03;
//			delay = 800;
//		}
//	}
	
//	if(deltaConductance<0 && deltaConductance>-INFINITY)
//	{
//		if(-deltaConductance/16<pointConductance)
//		{
//			mpptBoost.dsBst1 -= mpptBoost.stepDs; 
//			mpptParams.dsPositive = false;
//		}
//		else if(-deltaConductance/16>pointConductance)
//		{
//			mpptBoost.dsBst1 += mpptBoost.stepDs; 
//			mpptParams.dsPositive = true;
//		}
//	}

	if ((powerPerVolt > 0 && powerPerVolt < INFINITY) || (mpptParams.dsPositive && powerPerVolt == -INFINITY) || (!mpptParams.dsPositive && powerPerVolt == INFINITY)) // (powerPerVolt > 0)// 
	{
		mpptBoost.dsBst1 -= mpptBoost.stepDs; 
		mpptParams.dsPositive = false;
	} 
	
	else if ((powerPerVolt < 0 && powerPerVolt > -INFINITY) || (!mpptParams.dsPositive && powerPerVolt == -INFINITY) || (mpptParams.dsPositive && powerPerVolt == INFINITY)) //(powerPerVolt < 0)// 
	{
		mpptBoost.dsBst1 += mpptBoost.stepDs; 
		mpptParams.dsPositive = true;
	}
	
	
//	mpptEfficiency = (mpptParams.pvPower/1380)*100;
//	if(!count)
//		dynamicEff = mpptEfficiency;
//	dynamicEff = (dynamicEff+mpptEfficiency)/2;
	
	if(mpptBoost.dsBst1 < 0)
		mpptBoost.dsBst1 =0;
	else if(mpptBoost.dsBst1 > MPPT_BST_LIM )
		mpptBoost.dsBst1 = MPPT_BST_LIM;
	
	mpptBoost.dsBst2 = mpptBoost.dsBst1; 
	
	FTM_UpdatePwmDutycycle(FTM_BASE_MPPT, BST_CHNL_0, kFTM_CenterAlignedPwm, mpptBoost.dsBst1);
	FTM_UpdatePwmDutycycle(FTM_BASE_MPPT, BST_CHNL_1, kFTM_CenterAlignedPwm, mpptBoost.dsBst2);
	
	ConfigureTimer(delay);
	
	FTM_SetSoftwareTrigger(FTM_BASE_MPPT, true);

	if ((FTM_GetStatusFlags(FTM_BASE_MPPT) & kFTM_TimeOverflowFlag) == kFTM_TimeOverflowFlag)
	{
		FTM_ClearStatusFlags(FTM_BASE_MPPT, kFTM_TimeOverflowFlag);
	}
	FTM_EnableInterrupts(FTM_BASE_MPPT, FTM_INT_TYPE);
	
	while(!processDenotions.bstCycleRefresh)
	{}
		processDenotions.bstCycleRefresh = false;
		
	startPauseTimer();
	
}

void InitBoostDs() 
{
	mpptBoost.dsBst1 += mpptBoost.stepDs; 
	mpptBoost.dsBst2 += mpptBoost.stepDs;
	
	FTM_UpdatePwmDutycycle(FTM_BASE_MPPT, BST_CHNL_0, kFTM_CenterAlignedPwm, mpptBoost.dsBst1);
	FTM_UpdatePwmDutycycle(FTM_BASE_MPPT, BST_CHNL_1, kFTM_CenterAlignedPwm, mpptBoost.dsBst2);
	
	FTM_SetSoftwareTrigger(FTM_BASE_MPPT, true);
}
	
void UpdateMpptParams()
{
	mpptParams.prevPvCurrent = mpptParams.pvCurrent;
	mpptParams.prevPvVoltage = mpptParams.pvVoltage;
	mpptParams.prevPvPower = mpptParams.prevPvVoltage * mpptParams.prevPvCurrent;
	MpptSensorInput();
	mpptParams.pvPower = mpptParams.pvCurrent * mpptParams.pvVoltage;
	
	mpptParams.deltaPvCur = mpptParams.pvCurrent - mpptParams.prevPvCurrent;
	mpptParams.deltaPvVolt = mpptParams.pvVoltage - mpptParams.prevPvVoltage;
	mpptParams.deltaPvPower = mpptParams.pvPower - mpptParams.prevPvPower;
}

static float CalcStepVolt()
{
	float deltaV;
	MpptSensorInput();
	deltaV = mpptParams.pvVoltage*0.0005;
	return deltaV;
}

void SafetyShutdn()
{
	if(mpptParams.dcLinkVoltage > 900)
	{
//		StopMpptControl();
		FTM_UpdatePwmDutycycle(FTM_BASE_MPPT, BST_CHNL_0, kFTM_CenterAlignedPwm, OFFDS);
		FTM_UpdatePwmDutycycle(FTM_BASE_MPPT, BST_CHNL_1, kFTM_CenterAlignedPwm, OFFDS);
		
		FTM_SetSoftwareTrigger(FTM_BASE_MPPT, true);
	}
}

																																	//////////////////// FTM //////////////////////////////
void BOOST_IRQ_HANDLER()
{
	processDenotions.bstCycleRefresh = true;
	if ((FTM_GetStatusFlags(FTM_BASE_MPPT) & kFTM_TimeOverflowFlag) == kFTM_TimeOverflowFlag)
	{
		/* Clear interrupt flag.*/
		FTM_ClearStatusFlags(FTM_BASE_MPPT, kFTM_TimeOverflowFlag);
	}
	FTM_DisableInterrupts(FTM_BASE_MPPT, FTM_INT_TYPE);
	__DSB();
}


/*! @brief  initialize and configure FTM channels in accordance with the physical layout of the high side and low side switches,
inserting a suitable dead time in accordance with the physical switch limitations and enabling the FTM interrupts*/
static void InitMpptFtm()
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

	FTM_Init(FTM_BASE_MPPT, &ftmConfig);
	FtmModification(FTM_BASE_MPPT);
	FTM_SetupPwm(FTM_BASE_MPPT, ftmParams, MPPT_BST_CHNLS, kFTM_CenterAlignedPwm, BST_FREQ, FTM_SOURCE_CLOCK);
  EnableFtmInterrupts();
}

/*! @brief enable FTM interrupts through FTM drivers */
static void EnableFtmInterrupts()
{
//	FTM_EnableInterrupts(FTM_BASE_MPPT, FTM_INT_TYPE);
	EnableIRQ(BOOST_IRQ);
}

/*! @brief  modify FTM registers for minimum count loading point*/
static void FtmModification(FTM_Type *base)
{
  base->SYNCONF &= ~FTM_SYNCONF_SWRSTCNT_MASK;
//	base->COMBINE |= FTM_COMBINE_COMBINE0(1);

}


																										        	//////////////////////// ADC /////////////////////////////

void ADC0_IRQ_HANDLER(void)
{
	
    processDenotions.adcdone[0] = true;

   	(void)MPPT_ADC_BASE_0->R[MPPT_ADC_CHAN_GRP];
/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

void ADC1_IRQ_HANDLER(void)
{
	 /* Read conversion result to clear the conversion completed flag. */
    processDenotions.adcdone[1] = true;
  	(void)MPPT_ADC_BASE_1->R[MPPT_ADC_CHAN_GRP];

/* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F Store immediate overlapping
  exception return operation might vector to incorrect interrupt */
#if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
#endif
}

static void GetDefSensorRatios(adc_output_t* adcOutputs)
{
	adcOutputs->ctRatio = (CT_RATIO_NUM/CT_RATIO_DEN);
	adcOutputs->firStageVolDiv = 1204.7f/4.7f;//max 256.452442;// min 256.2696366;//
	adcOutputs->isoAmpGain = 1/8.085f; //max 8.1;// min 8.05;//
	adcOutputs->opAmpGain = 1.05f;//max 1.0752688;// min 1.0156408;//
	adcOutputs->secStageVolDiv = 1264.9f/64.9f;//max 19.49013;// min 19.3442;//
}

static float AdcToTransducer(adc_output_t* adcOutputs)
{
  float outVolt = (float)(adcOutputs->adcOut);
	outVolt = ((outVolt)/MAX16BIT_DIFF)*VREFH;
	return outVolt;
	
}

static float VolSensorValAdj(float wrngVal)
{
	float adjVal = 0;
	adjVal = (wrngVal*(498.57/523.45))-3.03667;
	return  adjVal;
}

static float CurSensorValAdj(float wrngVal)
{
	float adjVal = 0;
	adjVal = (wrngVal*(-7.5/7.37))-0.09023;
	return  adjVal;
}
	
static float AdcToRealValConv(adc_output_t* adcOutputs,char configNo)
{
	float AdcOutVolt,realVal,multiplier;
	AdcOutVolt = AdcToTransducer(adcOutputs);
	
	switch(configNo)
	{
		case CHNL_CONFIG_NO_I: 
			multiplier = adcOutputs->ctRatio;
			realVal = CurSensorValAdj(multiplier* AdcOutVolt);// multiplier* AdcOutVolt;// 
			break;
		default:
			multiplier = adcOutputs->isoAmpGain*adcOutputs->secStageVolDiv*adcOutputs->opAmpGain*adcOutputs->firStageVolDiv;
			realVal = VolSensorValAdj(multiplier*AdcOutVolt);// multiplier* AdcOutVolt;// 
		break;
	}
	
	return (realVal);
}

static void AdcCalibration(ADC_Type* adcBase)
{
	#if defined(FSL_FEATURE_ADC16_HAS_CALIBRATION) && FSL_FEATURE_ADC16_HAS_CALIBRATION
		while(ADC16_DoAutoCalibration(adcBase) != kStatus_Success);
	#endif
	
}

static void EnableAdcInterrupts()
{
	EnableIRQ(ADC0_INT);
	EnableIRQ(ADC1_INT);
}

static void AdcModifications(adc16_config_t* adcConfig)
{
	adcConfig->clockSource = kADC16_ClockSourceAlt0;
	adcConfig->enableAsynchronousClock = false;
	adcConfig->clockDivider = adcClockDiv;
	adcConfig->resolution = kADC16_ResolutionDF16Bit;
	adcConfig->enableHighSpeed = false;
	adcConfig->longSampleMode = sampleTime;
	adcConfig->enableAsynchronousClock = false;
	
}

static void InitAdcChannel(int chnlNo, adc16_channel_config_t* adcChannelConfig)
{
	adcChannelConfig->channelNumber = chnlNo;  
	adcChannelConfig->enableInterruptOnConversionCompleted = true; 
	#if defined(FSL_FEATURE_ADC16_HAS_DIFF_MODE) && FSL_FEATURE_ADC16_HAS_DIFF_MODE
		adcChannelConfig->enableDifferentialConversion = true;
	#endif /* FSL_FEATURE_ADC16_HAS_DIFF_MODE */

}




static void AdcConversion(ADC_Type* adcBase,int chnlGrp,adc16_channel_config_t* adcChannelConfig)
{
	if (adcBase == ADC0)
			processDenotions.adcdone[0] = false;
	else 
		processDenotions.adcdone[1] = false;
	
			ADC16_SetChannelConfig(adcBase, chnlGrp, adcChannelConfig);

}


static void InitMpptAdc()
{
	ADC16_GetDefaultConfig(&adcConfig);
	AdcModifications(&adcConfig);
	for (int a=0; a<2; a++)
	{
		ADC16_Init(adcBase[a], &adcConfig);
		ADC16_EnableHardwareTrigger(adcBase[a], false); /* Make sure the software trigger is used. */
		ADC16_SetHardwareAverage(adcBase[a],hwAverageMode);
		AdcCalibration(adcBase[a]);
	}
	
	for(int a=0;a<NO_ADC_INPUTS;a++)
		InitAdcChannel(adcChnlNum[a],&adcChannelConfig[a]);
	GetDefSensorRatios(&adcOutputs);
	EnableAdcInterrupts();
	
}


/* EOF */