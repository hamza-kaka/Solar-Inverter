/*! 
@file 
@brief 
@details 

@author Hamza Naeem Kakakhel
@copyright Taraz Technologies Pvt. Ltd.
 */
#ifndef MPPT_h
#define MPPT_h
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_adc16.h"
#include "fsl_common.h"
#include "fsl_ftm.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "math.h"
#include "resourceAssignNShare.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
//ADC channel defines 
#define NO_ADC_INPUTS (3)

//ADC charasteristics
#define VREFH 2.992f
#define MAX16BIT_DIFF 32767.0f
#define CT_RATIO_NUM 1.0f //3.0f  //the actual physical testing values give a more accurate result than the datasheet ratio
#define CT_RATIO_DEN 0.01175f //0.0349f
#define CT_OFFSET 0.03f

//FTM chars
#define OFFDS 0
#define BST_FREQ 38000U


/*******************************************************************************
 * Enums
 ******************************************************************************/

/*******************************************************************************
 * Structs
 ******************************************************************************/
typedef struct {	
	float prevPvCurrent;
	float prevPvVoltage;
	float prevPvPower;
	
	float deltaPvCur;
	float deltaPvVolt;
	float deltaPvPower;
	
	float pvCurrent;
	float pvVoltage;
	float dcLinkVoltage;
	float pvPower;
	float stepVoltage;
	bool dsPositive;
	
}mppt_params_t;

typedef struct {	
	float dsBst1;
	float dsBst2;
	float stepDs; 
	
}mppt_boost_t;

typedef struct {
	signed short adcOut;
	float ctRatio;
	float isoAmpGain;
	float secStageVolDiv;
	float opAmpGain;
	float firStageVolDiv;
	
}adc_output_t;

 /*******************************************************************************
 * Prototypes
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

void MpptSensorInput();
void StartMpptControl();
void StopMpptControl();
void InitMppt();
void MpptBoostControl();
void SafetyShutdn();

static float CalcStepVolt();
void UpdateMpptParams();
void InitBoostDs();
void UpdateBoostDs();

void MpptFaultCorrection();
	
/*******************************************************************************
 * Variables
 ******************************************************************************/

/*******************************************************************************
 * Code
 ******************************************************************************/

#if defined(__cplusplus)
}
#endif
#endif
/* EOF */