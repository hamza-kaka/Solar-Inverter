/*! 
@file  inverter.h
@brief inverter control header file
@details 

@author Hamza Naeem Kakakhel
@copyright Taraz Technologies Pvt. Ltd.
 */
#ifndef INVERTER_h
#define INVERTER_h
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_debug_console.h"
#include "fsl_common.h"
#include "fsl_ftm.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "math.h"
#include "resourceAssignNShare.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
// signal charasteristic defines
#define HARMONIC_INJ_EN 0
#define CLAMP 0
#define OFFDS 0

#define PWM_FREQ  15000U
#define INIT_FUND_FREQ 50U

#define AMP_MOD_IND 0.95f
#define PER_ANGLE 360.0f
#define HAR_SCA_NUM 1.0f
#define HAR_SCA_DEN 6.0f
#define HAR_ORDER 3
#define DEADTIMETICKS 12

// motor char defines
#define VF_RATIO 8

/*******************************************************************************
 * Enums
 ******************************************************************************/

/*******************************************************************************
 * Structs
 ******************************************************************************/
typedef struct {	
	float fundPeriod;
	float fundFreq;
	float ampModIndex;
	float ampModDifference;
	float freqModIndex;
	float harScalar;
	float curPhAngle;
	float DsU,DsV,DsW;
	float modSigU,modSigV,modSigW;
	int harOrder;
	
}modulated_sig_char_t;

typedef struct {	
	float switchPeriod;
	float switchFreq;
	float stepPhase;
	
}switching_sig_char_t;


	
 /*******************************************************************************
 * Prototypes
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

void InitInverter();
void StartInverterControl();
void InverterControl();
void StopInverterControl();


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