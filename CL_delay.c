
#include "CL_delay.h"

uint32_t CL_delayTicks = 0x00000000;

//***************************************************************
//						G4 related defines / macros / functions
//***************************************************************
#ifdef CL_USING_G4 
	#ifdef CL_DELAY_USE_TIM2
	#define TIMER TIM2
	#define enableTIMER()  (RCC->APB1ENR1 |= (RCC_APB1ENR1_TIM2EN))
   #define enablleRQ()		(NVIC_EnableIRQ(TIM2_IRQn))
	#endif

	#ifdef CL_DELAY_USE_TIM3
	#define TIMER TIM3
	#define enableTIMER()  (RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN)
    #define enableIRQ()		(NVIC_EnableIRQ(TIM3_IRQn))
	#endif
  
	#ifdef CL_DELAY_USE_TIM4
	#define TIMER TIM4
	#define enableTIMER()	(RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN)
	#define enableIRQ()		(NVIC_EnableIRQ(TIM4_IRQn))
	#endif
	
	//***************************************************************
	//						Init functions
	//***************************************************************
	#ifdef CL_delay_USE_LL
	void CL_delay_init() //supports General Purpose TImers 2,3,4,
		{
			enableTIMER();
			enableIRQ();
			
			LL_TIM_InitTypeDef myTIM; 
			LL_TIM_StructInit(&myTIM);
	
			#ifdef CL_DELAY_US_ENABLE
	
				myTIM.Autoreload  = 10;
				myTIM.Prescaler = 17;
	
			#else

				myTIM.Autoreload  = 1000;
				myTIM.Prescaler = (SystemCoreClock / 1000000);
	
			#endif
				
			LL_TIM_SetUpdateSource(TIM4, LL_TIM_UPDATESOURCE_COUNTER);     //update even will only be set by over/undeflow of counter
			LL_TIM_EnableIT_UPDATE(TIMER);
			//	LL_TIM_GenerateEvent_UPDATE(TIMER);	
			LL_TIM_Init(TIMER, &myTIM);
		}	

	#else //if not using LL

	void CL_delay_init() //supports General Purpose TImers 2,3,4,
		{
 			enableTIMER();
			enableIRQ();
			#ifdef CL_DELAY_US_ENABLE //change prescaler for micro second delay	
				//@@@@@@@@ NEED calculaiting 
				TIMER->ARR = 10;
				TIMER->PSC = 17;
			#else
			TIMER->PSC = (SystemCoreClock / 1000000);
				TIMER->ARR = 1000;
			#endif
				TIMER->CR1 |= TIM_CR1_URS;
				TIMER->DIER |= TIM_DIER_UIE;
				TIMER->EGR |= TIM_EGR_UG;  
		}	
	#endif //CL_delay_USE_LL

#endif //End CL_USING_G4


//***************************************************************
//						F1 related defines / macros / functions
//***************************************************************
#ifdef CL_USING_F1 
	#ifdef CL_DELAY_USE_TIM2
		#define TIMER TIM2
		#define enableTIMER()  (RCC->APB1ENR |= (RCC_APB1ENR_TIM2EN))
		#define enableIRQ()		(NVIC_EnableIRQ(TIM2_IRQn))
	#endif

	#ifdef CL_DELAY_USE_TIM3
		#define TIMER TIM3
		#define enableTIMER()  (RCC->APB1ENR |= RCC_APB1ENR_TIM3EN)
		#define enableIRQ()		(NVIC_EnableIRQ(TIM3_IRQn))
	#endif
  
	#ifdef CL_DELAY_USE_TIM4
		#define TIMER TIM4
		#define enableTIMER()	(RCC->APB1ENR |= RCC_APB1ENR_TIM4EN)
		#define enableIRQ()		(NVIC_EnableIRQ(TIM4_IRQn))
	#endif


	//***************************************************************
	//						Init functions
	//***************************************************************
	#ifdef CL_delay_USE_LL
	void CL_delay_init() //supports General Purpose TImers 2,3,4,
	{
	
		enableTIMER();
		enableIRQ();
	
		LL_TIM_InitTypeDef myTIM; 
		LL_TIM_StructInit(&myTIM);
		
	#ifdef CL_DELAY_US_ENABLE
	
		myTIM.Autoreload  = 1000;
		myTIM.Prescaler = 170;
	
	#else

		myTIM.Autoreload  = 1000;
		//uint32_t  temp  = (SystemCoreClock / 1000000);
		myTIM.Prescaler = (SystemCoreClock/1000000);
	
	#endif
	
		LL_TIM_SetUpdateSource(TIMER, LL_TIM_UPDATESOURCE_COUNTER);      //update even will only be set by over/undeflow of counter
		LL_TIM_EnableIT_UPDATE(TIMER);
		//	LL_TIM_GenerateEvent_UPDATE(TIMER);	
		LL_TIM_Init(TIMER, &myTIM);
	}	

	#else //if not using LL

	void CL_delay_init() //supports General Purpose TImers 2,3,4,
	{
		enableIRQ();
		enableTIMER();
	
	#ifdef CL_DELAY_US_ENABLE //change prescaler for micro second delay	
		//@@@@@@@@ NEED calculaiting 
		TIMER->ARR = 70;
		TIMER->PSC = 0;
	#else
		TIMER->PSC = (SystemCoreClock / 1000000);
		TIMER->ARR = 1000;
	#endif
		TIMER->CR1 |= TIM_CR1_URS;
		TIMER->DIER |= TIM_DIER_UIE;
		//TIMER->EGR |= TIM_EGR_UG;  
	}	
	#endif //CL_delay_USE_LL
	
#endif // End USING_F1


//***************************************************************
//						F1 related defines / macros / functions
//***************************************************************
#ifdef CL_USING_L0
#ifdef CL_DELAY_USE_TIM2
#define TIMER TIM2
#define enableTIMER()  (RCC->APB1ENR |= (RCC_APB1ENR_TIM2EN))
#define enableIRQ()		NVIC_EnableIRQ(TIM2_IRQn)
#endif

#ifdef  CL_DELAY_USE_TIM3 //there is no TIM3 in L031K6
#define TIMER TIM2
#define enableTIMER()  (RCC->APB1ENR |= RCC_APB1ENR_TIM2EN)
#define enableIRQ()		(NVIC_EnableIRQ(TIM2_IRQn))
#endif


	//***************************************************************
	//						Init functions
	//***************************************************************
#ifdef CL_delay_USE_LL
	void CL_delay_init() //supports General Purpose TImers 2 only
{
	
	enableTIMER();
	enableIRQ();
	
	LL_TIM_InitTypeDef myTIM; 
	LL_TIM_StructInit(&myTIM);
		
#ifdef CL_DELAY_US_ENABLE
	
	myTIM.Autoreload  = 1000;
	myTIM.Prescaler = 170;
	
#else

	myTIM.Autoreload  = 1000;
	//uint32_t  temp  = (SystemCoreClock / 1000000);
	myTIM.Prescaler = (SystemCoreClock / 1000000);
	
#endif
	
	LL_TIM_SetUpdateSource(TIMER, LL_TIM_UPDATESOURCE_COUNTER);       //update even will only be set by over/undeflow of counter
	LL_TIM_EnableIT_UPDATE(TIMER);
	//	LL_TIM_GenerateEvent_UPDATE(TIMER);	
	LL_TIM_Init(TIMER, &myTIM);
}	

#else //if not using LL

void CL_delay_init() //supports General Purpose TImers 2,3,4,
{
	enableIRQ();
	enableTIMER();
	
#ifdef CL_DELAY_US_ENABLE //change prescaler for micro second delay	
	//@@@@@@@@ NEED calculaiting 
	TIMER->ARR = 32;
	TIMER->PSC = 0;
#else
	TIMER->PSC = (SystemCoreClock / 1000000);
	TIMER->ARR = 1000;
#endif
	TIMER->CR1 |= TIM_CR1_URS;
	TIMER->DIER |= TIM_DIER_UIE;
	//TIMER->EGR |= TIM_EGR_UG;  
}	
#endif //CL_delay_USE_LL
	
#endif // End USING_F1


//***************************************************************
//						F4 related defines / macros / functions
//***************************************************************
//		NOTES:
//		APB1 Timer clocks run at 90MHz max when the system clock is set to 180MHz
//		so the delay function uses 90MHz in its calculations 

#ifdef CL_USING_F4 
#ifdef CL_DELAY_USE_TIM2
#define TIMER TIM2
#define enableTIMER()  (RCC->APB1ENR |= (RCC_APB1ENR_TIM2EN))
#define enableIRQ()		(NVIC_EnableIRQ(TIM2_IRQn))
#endif

#ifdef CL_DELAY_USE_TIM3
#define TIMER TIM3
#define enableTIMER()  (RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN)
#define enableIRQ()		(NVIC_EnableIRQ(TIM3_IRQn))
#endif
  
#ifdef CL_DELAY_USE_TIM4
#define TIMER TIM4
#define enableTIMER()	(RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN)
#define enableIRQ()		(NVIC_EnableIRQ(TIM4_IRQn))
#endif
	
//***************************************************************
//						Init functions
//***************************************************************
#ifdef CL_delay_USE_LL
void CL_delay_init() //supports General Purpose TImers 2,3,4,
{
	enableTIMER();
	enableIRQ();
			
	LL_TIM_InitTypeDef myTIM; 
	LL_TIM_StructInit(&myTIM);
	
#ifdef CL_DELAY_US_ENABLE
	
	myTIM.Autoreload  = 10;
	myTIM.Prescaler = 17;
	
#else

	
	myTIM.Prescaler = 90; //90MHz divided by 90 is 1MHz (why 90? see notes on F4 seection )
	myTIM.Autoreload  = 1000; //then 1MHz divided by 1000 is 1000 
	//and thus each timer tick is 1ms
#endif
	//	( (SystemCoreClock/1000000) / 4 )	* 2 
	//	according to clock tree settings this should give 90 when
	// system clock is 180
	
	LL_TIM_SetUpdateSource(TIMER, LL_TIM_UPDATESOURCE_COUNTER); //update even will only be set by over/undeflow of counter
	LL_TIM_EnableIT_UPDATE(TIMER);
	//	LL_TIM_GenerateEvent_UPDATE(TIMER);	
	LL_TIM_Init(TIMER, &myTIM);
}	

#else //if not using LL

void CL_delay_init() //supports General Purpose TImers 2,3,4,
{
	enableTIMER();
	enableIRQ();
#ifdef CL_DELAY_US_ENABLE //change prescaler for micro second delay	
	//@@@@@@@@ NEED calculaiting 
	TIMER->ARR = 10;
	TIMER->PSC = 17;
#else
	TIMER->PSC = (SystemCoreClock / 1000000);
	TIMER->ARR = 1000;
#endif
	TIMER->CR1 |= TIM_CR1_URS;
	TIMER->DIER |= TIM_DIER_UIE;
	TIMER->EGR |= TIM_EGR_UG;  
}	
#endif //CL_delay_USE_LL

#endif //End CL_USING_F4


//***************************************************************
//						F7 related defines / macros / functions
//***************************************************************
//		NOTES:
//		APB1 Timer clocks run at 90MHz max when the system clock is set to 180MHz
//		so the delay function uses 90MHz in its calculations 

#ifdef CL_USING_F7
#ifdef CL_DELAY_USE_TIM2
#define TIMER TIM2
#define enableTIMER()  (RCC->APB1ENR |= (RCC_APB1ENR_TIM2EN))
#define enableIRQ()		(NVIC_EnableIRQ(TIM2_IRQn))
//#define enableIRQ()		(NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn))
#endif

#ifdef CL_DELAY_USE_TIM3
#define TIMER TIM3
#define enableTIMER()  (RCC->APB1ENR1 |= RCC_APB1ENR1_TIM3EN)
#define enableIRQ()		(NVIC_EnableIRQ(TIM3_IRQn))
#endif
  
#ifdef CL_DELAY_USE_TIM4
#define TIMER TIM4
#define enableTIMER()	(RCC->APB1ENR |= RCC_APB1ENR_TIM4EN)
#define enableIRQ()		(NVIC_EnableIRQ(TIM4_IRQn))
#endif
	
//***************************************************************
//						Init functions
//***************************************************************
#ifdef CL_delay_USE_LL
void CL_delay_init() //supports General Purpose TImers 2,3,4,
{
	enableTIMER();
	enableIRQ();
			
	LL_TIM_InitTypeDef myTIM; 
	LL_TIM_StructInit(&myTIM);
	
#ifdef CL_DELAY_US_ENABLE
	
	myTIM.Autoreload  = 100;
	myTIM.Prescaler = 0;
	
#else

	
	myTIM.Prescaler = 99;  //90MHz divided by 90 is 1MHz (why 90? see notes on F4 seection )
	myTIM.Autoreload  = 999;  //then 1MHz divided by 1000 is 1000 
	//and thus each timer tick is 1ms
#endif
	//	( (SystemCoreClock/1000000) / 4 )	* 2 
	//	according to clock tree settings this should give 90 when
	// system clock is 180
	
	LL_TIM_SetUpdateSource(TIMER, LL_TIM_UPDATESOURCE_COUNTER);  //update even will only be set by over/undeflow of counter
	LL_TIM_EnableIT_UPDATE(TIMER);
	//	LL_TIM_GenerateEvent_UPDATE(TIMER);	
	LL_TIM_Init(TIMER, &myTIM);
}	

#else //if not using LL

void CL_delay_init() //supports General Purpose TImers 2,3,4,
{
	enableTIMER();
	enableIRQ();
#ifdef CL_DELAY_US_ENABLE //change prescaler for micro second delay	
	//@@@@@@@@ NEED calculaiting 
	TIMER->ARR = 10;
	TIMER->PSC = 17;
#else
	TIMER->PSC = (SystemCoreClock / 1000000);
	TIMER->ARR = 1000;
#endif
	TIMER->CR1 |= TIM_CR1_URS;
	TIMER->DIER |= TIM_DIER_UIE;
	TIMER->EGR |= TIM_EGR_UG;  
}	
#endif //CL_delay_USE_LL

#endif //End CL_USING_F4
//***************************************************************
//						Universal Delay functions
//***************************************************************

#ifdef CL_DELAY_US_ENABLE 

	void delayMS(uint32_t ms)
	{
		
		TIMER->CR1 |= TIM_CR1_CEN;
		CL_delayTicks = 0;
		while (CL_delayTicks < (ms * 999)) ;
		TIMER->CR1 &= ~TIM_CR1_CEN;
	}
	void delayUS(uint32_t us)
	{
		uint32_t temp = us ;
		TIMER->CR1 |= TIM_CR1_CEN;
		CL_delayTicks = 0;
		while (CL_delayTicks < (us)) ;
		TIMER->CR1 &= ~TIM_CR1_CEN;
	}

#else 

	void delayMS(uint32_t ms)
	{
		TIMER->CR1 |= TIM_CR1_CEN; //turn on the timer to start delay counter
		CL_delayTicks = 0;
		while (CL_delayTicks < (ms)) ;
		/*	after delay count is reached turn the timer off
			so that you're not interrupting the MCU uneccessarily 
		*/
		TIMER->CR1 &= ~(TIM_CR1_CEN); 
	}

#endif // end CL_DELAY_US_ENABLE


//***************************************************************
//						Interrupt Handlers
//***************************************************************

#ifdef CL_DELAY_USE_TIM2
	


	void TIM2_IRQHandler(void)
	{
		TIM2->SR &= ~TIM_SR_UIF;
		CL_delayTicks++;
	}


#endif //CL_DELAY_USE_TIM2

#ifdef CL_DELAY_USE_TIM3
	void TIM3_IRQHandler(void)
	{
		TIM3->SR &= ~(TIM_SR_UIF);
		CL_delayTicks++;
	}
#endif//CL_DELAY_USE_TIM3

#ifdef CL_DELAY_USE_TIM4
	void TIM4_IRQHandler(void)
	{
		TIM4->SR &= ~TIM_SR_UIF;
		CL_delayTicks++;
	}
#endif //CL_DELAY_USE_TIM4


