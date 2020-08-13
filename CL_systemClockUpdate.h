#ifndef CL_systemClockUpdate_H_
#define CL_systemClockUpdate_H_

#include "CL_CONFIG.h"

/***********************| STM32G431KB |*************************/
#ifdef CL_USING_G4

	#include "stm32g4xx.h"

#ifdef CL_SYSTEMCLOCKUPDATE_USE_LL
			#include <stm32g4xx_ll_rcc.h>
			#include <stm32g4xx_ll_system.h>
		#endif

	//G4 specific functions
void CL_setSysClockTo170(void);
void CL_setSysClockTo80(void);
#endif
/***************************************************************/



/***********************| STM32F103C |*************************/
#ifdef CL_USING_F1

#include "stm32f1xx.h"

	#ifdef CL_F1_USE_LL
		#include <stm32f1xx_ll_rcc.h>
		#include <stm32f1xx_ll_system.h>
	#endif

//f1 specific functions
void CL_setSysClockTo72(void);

#endif
/**************************************************************/



#endif


/***********************| STM32F103C |*************************/
#ifdef CL_USING_L0

#include "stm32l0xx.h"
void setClockTo32Mhz(void);
#endif

