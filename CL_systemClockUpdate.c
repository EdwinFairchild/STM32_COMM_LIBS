#include "CL_systemClockUpdate.h"

#ifdef CL_USING_G4


	void CL_setSysClockTo170(void)
	{
	
	#ifdef CL_SYSTEMCLOCKUPDATE_USE_LL
		//voltage scaling
		LL_VREFBUF_SetVoltageScaling(LL_VREFBUF_VOLTAGE_SCALE1);
		// prefetch
		LL_FLASH_EnablePrefetch();
		//flash latency
		LL_FLASH_SetLatency(LL_FLASH_LATENCY_8);
		//HSE external
		LL_RCC_HSE_Enable();
		while (!(LL_RCC_HSE_IsReady())) ;
	
		//AHB and APB prescalers
		LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
		LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
		LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
		//configure PLL and change clock source
		/*---------------------| Pll Configuration |----------------
		 * 1.Disable PLL_R or just clear entire register, PLL_r is the only thing set anyways
		 * 2.set PLL_R division to 2
		 * 3.Enable PLL_R	 * 
		 * 4.Set PLL input divsion factor (PLLM) to 1	 
		 * 5.Set multiplication factor of 16 into PLLN	  
		 * 6.Select PLL source HSE
		 * 7.Enable PLL
		 * 8.Change clock source
		 * 9. call SystemCoreClockUpdate(); 
		 **/
		//LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_2, 85, LL_RCC_PLLR_DIV_2); //170
		LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_1, 20, LL_RCC_PLLR_DIV_2); //80
		LL_RCC_PLL_Enable();
		LL_RCC_PLL_EnableDomain_SYS();

		while (!(LL_RCC_PLL_IsReady()));
	
		LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
		SystemCoreClockUpdate();
		uint32_t temp = SystemCoreClock;
		//volatile uint32_t temp = SystemCoreClock; //used to check changes worked

	#else

	
	
		//voltage rang 1 is set by default
	
	
		//enable prefetch buffer
		FLASH->ACR |= FLASH_ACR_PRFTEN;
	
		//turn on high speed external
		RCC->CR |= RCC_CR_HSEON; 
		while (!(RCC->CR & RCC_CR_HSERDY)) ;
	
	
		FLASH->ACR |= FLASH_ACR_LATENCY_8WS; 
	
		//AHB and APB settings
	
		RCC->CFGR &= ~(RCC_CFGR_HPRE_3 | RCC_CFGR_PPRE1_2 | RCC_CFGR_PPRE2_2);
	
		/*---------------------| Pll Configuration |----------------
		 * 1.Disable PLL_R or just clear entire register, PLL_r is the only thing set anyways
		 * 2.set PLL_R division to 2
		 * 3.Enable PLL_R	 * 
		 * 4.Set PLL input divsion factor (PLLM) to 1	 
		 * 5.Set multiplication factor of 16 into PLLN	  
		 * 6.Select PLL source HSE
		 * 7.Enable PLL
		 * 8.Change clock source
		 * 9. call SystemCoreClockUpdate(); 
		 **/	 
		RCC->PLLCFGR = 0x00000000;      			// 1	
		RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLR);      	// 2	
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;      	// 3	
		RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM);      // 4	
		
		//85 for 170 ; 20 for 80
		//RCC->PLLCFGR |= RCC_PLLCFGR_PLLM_0; //PLL-M div by 2 for 170 MHZ, comment out for div by 1		
		RCC->PLLCFGR |= (20 << RCC_PLLCFGR_PLLN_Pos);   // 5 
		
		
		RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;      			//6	
	
		RCC->CR |= RCC_CR_PLLON;       //7
		while(!(RCC->CR & RCC_CR_PLLRDY));
		//---------------------| Pll Configuration |----------------
	
		//8
		RCC->CFGR |= RCC_CFGR_SW_PLL; 
		while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) ;
	
		SystemCoreClockUpdate();
		//uncomment systick if needed
		//SysTick_Config(SystemCoreClock / 1000);
		
	#endif	
	
	
	}

#endif 

#ifdef CL_USING_F1

void CL_setSysClockTo72(void)
{


	//turn on external crystal
	RCC->CR |= RCC_CR_HSEON;

	//wait for HSE crystal be stable
	//there are better ways to handle this but ill use simple polling
	while(!(RCC->CR & RCC_CR_HSERDY))
	       ;
 
 
	//activate prefetch buffer but it should already be on
	FLASH->ACR |= FLASH_ACR_PRFTBE;

	// Flash 2 wait state 
	FLASH->ACR &= ~(FLASH_ACR_LATENCY);     //reset just to be sure
	FLASH->ACR |= (uint32_t)0x2;    

 
	//configure RCC and PLL settings while PLL is off
	RCC->CFGR &= ~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |  RCC_CFGR_PLLMULL);     //reset
 
	RCC->CFGR &= ~(RCC_CFGR_PLLXTPRE);      //PLLXTPRE bit set to 0
	RCC->CFGR |= RCC_CFGR_PLLSRC;      //pll source
	RCC->CFGR |= RCC_CFGR_PLLMULL9; //  RCC_CFGR_PLLMULL9;      //pll miultiplier 7 = 28 MHz   9 = 72MHz 10 = 80MHz OVERCLOCKED 12 = 96
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;     //AHB prescaler  2 = 28 MHz   1 = 72MHz
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;     //APB1 presacaler 
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;     //APB2 prescaler
 
  
	//turn on PLL
	RCC->CR |= RCC_CR_PLLON; 
	while (!(RCC->CR & RCC_CR_PLLRDY)) ;
 
	//set pll as clock source
	RCC->CFGR &= ~(RCC_CFGR_SW);     //reset just in case 
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) ;

	SystemCoreClockUpdate();
	uint32_t temp = SystemCoreClock; //just used to verify
 
}
#endif

#ifdef CL_USING_L0

void setClockTo32Mhz(void)
{
	//adjust flash latency
	//REG_VAL = FLASH->ACR;
	FLASH->ACR |= FLASH_ACR_LATENCY;
	while ((FLASH->ACR & FLASH_ACR_LATENCY) == 0) ; //wait for latency set flag

	//set voltage scaling to range 1
	PWR->CR |= PWR_CR_VOS_0;
	PWR->CR &= ~(PWR_CR_VOS_1);
	while (((PWR->CSR) & (PWR_CSR_VOSF)) == 1) ; //wait for voltage to settle

	//turn on HSE external, HSE bypass and security
	RCC->CR |= RCC_CR_CSSHSEON | RCC_CR_HSEBYP |  RCC_CR_HSEON;
	while (((RCC->CR) & RCC_CR_HSERDY) == 0) ; //wait for the HSE to be ready

	//reset and configure pll mull and div settings, and PLL source
	RCC->CFGR = ((RCC->CFGR & ~(RCC_CFGR_PLLDIV | RCC_CFGR_PLLMUL)) | RCC_CFGR_PLLDIV2 | RCC_CFGR_PLLMUL8 | RCC_CFGR_PLLSRC_HSE);
	while ((RCC->CR & RCC_CR_PLLRDY) == 1) ;

	//turn on PLL , wait for ready
	RCC->CR |= RCC_CR_PLLON;
	while (((RCC->CR) & RCC_CR_PLLRDY) == 0) ; // wait for pll to ready

	//set PLL as system clock
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while (((RCC->CFGR)&(RCC_CFGR_SWS_PLL)) != RCC_CFGR_SWS_PLL) ;
	SystemCoreClockUpdate();
}//-------------------------------------------------------------------

#endif

#ifdef CL_USING_F4

void setClockTo180(void)
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);

	if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_5)
	{
		//Error_Handler();  
	}
	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
	LL_PWR_EnableOverDriveMode();
	LL_RCC_HSE_Enable();

	/* Wait till HSE is ready */
	while (LL_RCC_HSE_IsReady() != 1)
	{
    
	}
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_4, 180, LL_RCC_PLLP_DIV_2);
	LL_RCC_PLL_Enable();

	/* Wait till PLL is ready */
	while (LL_RCC_PLL_IsReady() != 1)
	{
    
	}

	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	/* Wait till System clock is ready */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
	{
  
	}
	LL_SetSystemCoreClock(180000000);
	LL_RCC_SetTIMPrescaler(LL_RCC_TIM_PRESCALER_TWICE);
	//SystemCoreClockUpdate();
	//uint32_t temp = SystemCoreClock;
	
}//--------------------------------------------------------------------------------

#endif 


#ifdef CL_USING_F7

void CL_configClock(void)
{
	LL_FLASH_SetLatency(LL_FLASH_LATENCY_7);

	if (LL_FLASH_GetLatency() != LL_FLASH_LATENCY_7)
	{
		//Error_Handler();  
	}
	LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
	LL_PWR_EnableOverDriveMode();
	LL_RCC_HSE_Enable();

	/* Wait till HSE is ready */
	while (LL_RCC_HSE_IsReady() != 1)
	{
    
	}
	LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_4, 216, LL_RCC_PLLP_DIV_2);
	LL_RCC_PLL_Enable();

	/* Wait till PLL is ready */
	while (LL_RCC_PLL_IsReady() != 1)
	{
    
	}
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
	LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

	/* Wait till System clock is ready */
	while (LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
	{
  
	}
	LL_Init1msTick(216000000);
	LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
	LL_SetSystemCoreClock(216000000);
	LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_PCLK1);
	LL_RCC_SetI2CClockSource(LL_RCC_I2C1_CLKSOURCE_PCLK1);
	//-----------------------------------------------------------------
}
#endif 