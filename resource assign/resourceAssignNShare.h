/*! 
@file resource_assignment.h
@brief allocation of all the modules of controller to different modules of project
@details 

@author Hamza Naeem Kakakhel
@copyright Taraz Technologies Pvt. Ltd.
 */
#ifndef RESOURCES_h
#define RESOURCES_h
/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "fsl_common.h"

/*******************************************************************************
 * Defines
 ******************************************************************************/
// FTM defines
	//for inverter module
#define FTM_BASE_INV FTM3
	//for MPPT module
#define FTM_BASE_MPPT FTM1
	//for AC module
#define FTM_BASE_AC FTM2

// FTM channel defines
	//for inverter module
#define WL kFTM_Chnl_7
#define WH kFTM_Chnl_6
#define VL kFTM_Chnl_5
#define VH kFTM_Chnl_4
#define UL kFTM_Chnl_1
#define UH kFTM_Chnl_0
#define FTM_INV_CHNLS (6)
	//for MPPT module
#define BST_CHNL_0 kFTM_Chnl_0
#define BST_CHNL_1 kFTM_Chnl_1
#define MPPT_BST_CHNLS (2)
	//for AC module
#define AC_BST_CHNL_0 kFTM_Chnl_0
#define AC_BST_CHNL_1 kFTM_Chnl_1
#define AC_BST_CHNLS (2)

// ADC defines
	//for mppt current sensor
#define MPPT_ADC_BASE_0 ADC0
#define MPPT_ADC_BASE_1 ADC1
#define MPPT_ADC_CHAN_GRP 0U


// ADC channel defines
	//for mppt current sensor
#define MPPTIPV_CHNL 1U
#define MPPTVPV_CHNL 0U
#define MPPTVDC_CHNL 3U


// PDB defines
#define MPPT_PDB_BASE PDB0

//GPIO defines 
#define RELAY_GPIO_BASE GPIOB
#define RELAY_PIN (2)

//SITUATIONAL
#define TEMP 1
#define MPPT_EN 0
#define INVERTER_EN 1
#define AC_EN 0

/*******************************************************************************
 * Enums
 ******************************************************************************/

/*******************************************************************************
 * Structs
 ******************************************************************************/
typedef struct {
	volatile bool dsUpdated;
	volatile bool freqChange;
	volatile bool adcdone[2];
	volatile bool pitTimerdone;
	volatile bool bstCycleRefresh;
	
}process_denotions_t;

 /*******************************************************************************
 * Prototypes
 ******************************************************************************/
#if defined(__cplusplus)
extern "C" {
#endif

  void InitModules();
	void StartModules();
	void ControlBlock();
	
	void SafetyCheck();
	void DriverFaultCorrection();

/*******************************************************************************
 * Variables
 ******************************************************************************/
// shared variables
extern process_denotions_t processDenotions;


/*******************************************************************************
 * Code
 ******************************************************************************/

#if defined(__cplusplus)
}
#endif
#endif
/* EOF */