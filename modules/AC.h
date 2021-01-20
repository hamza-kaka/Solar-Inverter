/*! 
@file 
@brief AC block control header 
@details 

@author Hamza Naeem Kakakhel
@copyright Taraz Technologies Pvt. Ltd.
 */
#ifndef AC_h
#define AC_h
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_common.h"
#include "fsl_ftm.h"
#include "clock_config.h"
#include "pin_mux.h"
#include "math.h"
#include "fsl_gpio.h"
#include "resourceAssignNShare.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
//FTM chars
#define OFFDS 0
#define AC_BST_FREQ 38000U

//Relay status 
#define RELAY_ON (1)
#define RELAY_OFF (0)

//Relay pin shift 
#define PIN_SHIFT (1u << RELAY_PIN)

/*******************************************************************************
 * Enums
 ******************************************************************************/

/*******************************************************************************
 * Structs
 ******************************************************************************/
typedef struct {	
	float dsBst1;
	float dsBst2;
	float stepDs; 
	
}ac_boost_t;

typedef struct {	
	bool relayStatus;
	
}relay_ctrl_t;

 /*******************************************************************************
 * Prototypes
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

void InitAcBlock();
void StartAcControl();
void StopAcControl();
void AcBoostControl();

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