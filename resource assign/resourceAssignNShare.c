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
	#include "inverter.h"
	#include "mppt.h"
	#include "AC.h"
	#include "timers.h"

 /*******************************************************************************
  * Defines
  ******************************************************************************/


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
process_denotions_t processDenotions = {0,0,{0,0},0,0};

 /*******************************************************************************
  * Code
  ******************************************************************************/
 
	void InitModules()
	{
		BOARD_InitBootClocks();
		BOARD_InitBootPins();
		
		InitPit();
		InitMppt();
		InitInverter();
		InitAcBlock();
	}
	
	void StartModules()
	{
		#if (MPPT_EN)
			StartMpptControl();
		#endif
		#if (INVERTER_EN)
			StartInverterControl();
		#endif
		#if (AC_EN)
			StartAcControl();
		#endif
	}
	
	void ControlBlock()
	{
		#if (MPPT_EN)
			MpptSensorInput();
			MpptBoostControl();
		#endif
		#if (INVERTER_EN)
			InverterControl();
		#endif
		#if (AC_EN)
			
		#endif
		
		
		
	}
	
	void SafetyCheck()
	{
		SafetyShutdn();
	}
	
	void DriverFaultCorrection()
	{
//		MpptFaultCorrection();
	}
		
 /* EOF */ 