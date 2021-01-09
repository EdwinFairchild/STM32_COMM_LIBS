#include "CL_printMsg.h"

#ifdef CL_USING_F1
void CL_printMsg_init(printMsg_config_Type config)
{
	/* UART2 
	 * GPIOA A9 TX 
	 * Alternate Function
	 * */
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; //alternate fucntion periph clock enable
	
	//TX pin port clock enable
	if(config.tx_port == GPIOA)
		 RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	if(config.tx_port == GPIOB)
		 RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	if(config.tx_port == GPIOC)
		 RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	/*may fill in more ports if available or needed
	if(config.tx_port == GPIOC)
		 RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;*/
	
	//enalbe USART clock
	if(config.Uart_instance == USART1)
		 RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	if(config.Uart_instance == USART2)
	 RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	if(config.Uart_instance == USART3)
		RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	//may add more as needed
	
	//configure TX pin as 50MHZ mode and Alt Function push pull cnf
	if(config.TX_pin > 7) // config pin on CR-High
	{
		config.tx_port->CRH |= (1<<MODE_BIT_0_CRH) | (1<<MODE_BIT_1_CRH) | (1<<CNF_BIT_1_CRH) ;  	// mode = 11 :  cnf_1 = 1  
		config.tx_port->CRH &= ~(1<<CNF_BIT_0_CRH);    //cnf_0 = 0
	}
	else // config pin on CR-LOW
	{
		config.tx_port->CRL |= (1<<MODE_BIT_0_CRL) | (1<<MODE_BIT_1_CRL) | (1<<CNF_BIT_1_CRL) ;  	// mode = 11 :  cnf_1 = 1  
		config.tx_port->CRL &= ~(1<<CNF_BIT_0_CRL); //cnf_0 = 0		
	}
	
	//config USART BAUD RATE
	// clkPer / (baudR x 16 ) = 
	//72Mhz / (9600 x 16) = 468.75 = 0x1d4C //uart 2 is at 36MHZ fix this when i get a chance
	// 
	config.Uart_instance->BRR = 0x1d4C; //0x271;   //ill figure out how to get clock freq. so that this doesnt have to be hard coded
	config.Uart_instance->CR1 |=  USART_CR1_TE;
	config.Uart_instance->CR1 |= USART_CR1_UE;	
}

void CL_printMsg_init_Default(bool fullDuplex)
{
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;  //alternate fucntion periph clock enable
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //gpioa
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; //uart1
	
	GPIOA->CRH &= GPIO_CRH_CNF9;
	GPIOA->CRH |= GPIO_CRH_CNF9_1 | GPIO_CRH_MODE9;
	
	//0x271 = 115200   0x1d4c = 9600
	USART1->BRR = 0x271; //0x1d4C;    //ill figure out how to get clock freq. so that this doesnt have to be hard coded
	USART1->CR1 |=  USART_CR1_TE;
	USART1->CR1 |= USART_CR1_UE;
}
void CL_printMsg(char *msg, ...)
{	
	char buff[80];	
	va_list args;
	va_start(args, msg);
	vsprintf(buff, msg, args);
		
	for (int i = 0; i < strlen(buff); i++)
	{		
		USART1->DR = buff[i];
		while( !( USART1->SR & USART_SR_TXE )  );
	}		
		
	while (!(USART1->SR & USART_SR_TC));		
}

#endif

//***************************************************************
//						G4 init functions
//***************************************************************
#ifdef CL_USING_G4
	// initialize using custom UART
	void CL_printMsg_init(printMsg_config_Type config)
	{
	
	}
	
	//initialize using default UART2 on PA2 
	void CL_printMsg_init_Default(bool fullDuplex)
	{
		//clocks
		RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
		//gpio
		GPIOA->MODER &= ~(GPIO_MODER_MODE2);   
		GPIOA->MODER |= GPIO_MODER_MODE2_1; // 0b10 = AF mode
		GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2_1;
		GPIOA->AFR[0] |= 0x07<<GPIO_AFRL_AFSEL2_Pos;	
	
		//uart
		/* if over sampling by 16 then :
		 * BRR = FPCLK / baudrate 
		 * therefore CL_printMsg_init_Default 
		 * must be called after all clocks have been setup 
		 * so that SystemCoreClock has proper value
		 */
		USART2->BRR = (uint32_t)(SystemCoreClock / 115200);
	
		if(fullDuplex)
		{
			GPIOA->MODER &= ~(GPIO_MODER_MODE10);   
			GPIOA->MODER |= GPIO_MODER_MODE10_1;  // 0b10 = AF mode
			GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10_1;
			GPIOA->AFR[1] |= 0x07 << GPIO_AFRH_AFSEL10_Pos;
			USART1->CR1 |= USART_CR1_RE;
		}
	
		USART2->CR1 |= USART_CR1_TE | USART_CR1_UE;	
	}
	void CL_printMsg(char *msg, ...)
	{	
		char buff[80];	
		va_list args;
		va_start(args, msg);
		vsprintf(buff, msg, args);
		
		for (int i = 0; i < strlen(buff); i++)
		{
		
			USART2->TDR = buff[i];
			while (!(USART2->ISR & USART_ISR_TXE)) ;
		}		
		
		while (!(USART2->ISR & USART_ISR_TC)) ;		
	}
#endif // USING G4



//***************************************************************
//						L0 init functions
//***************************************************************
#ifdef CL_USING_L0
	// initialize using custom UART
	void CL_printMsg_init(printMsg_config_Type config)
{
	
}
	
//initialize using default UART1 on PA9 
void CL_printMsg_init_Default(bool fullDuplex)
{
	//enable  clock  needed for uart gpio aswell as uart itself
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

	// PA2 and PA15 to Alternate Function Mode
	GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE2_0))
	| (GPIO_MODER_MODE2_1);

	GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE15_0))
	| (GPIO_MODER_MODE15_1);

	//Select the specific Alternate function
	GPIOA->AFR[0] |= 4 << GPIO_AFRL_AFSEL2_Pos;
	GPIOA->AFR[1] |= 4 << GPIO_AFRH_AFSEL15_Pos;

	// Baudrate = clk_Frq / BRR ===>  32Mhz / 9600 = 
	USART2->BRR = 0xD05;// 320000 / 96; 
	;    //160000 / 96;
	// Enable RX_NE interrupt and TXE interrupt, enable UART, RECEIVE , TRANSMIT COMPLETE
	USART2->CR1 = USART_CR1_TE | USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_RE	| USART_CR1_TCIE;
}
void CL_printMsg(char *msg, ...)
{	
	char buff[80];	
	va_list args;
	va_start(args, msg);
	vsprintf(buff, msg, args);
		
	for (int i = 0; i < strlen(buff); i++)
	{
		
		USART2->TDR = buff[i];
		while (!(USART2->ISR & USART_ISR_TXE)) ;
	}		
		
	while (!(USART2->ISR & USART_ISR_TC)) ;		
}
#endif // USING L0

#ifdef CL_USING_F4
#include <stm32f4xx_ll_gpio.h>
#include <stm32f4xx_ll_usart.h>
#include <stm32f4xx_ll_bus.h>
void CL_printMsg_init_Default(bool fullDuplex)
{

	LL_USART_InitTypeDef USART_InitStruct = { 0 };

	LL_GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* Peripheral clock enable */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

	LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
	/**USART1 GPIO Configuration  
	PA9   ------> USART1_TX
	PA10   ------> USART1_RX 
	*/
	LL_GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* USER CODE BEGIN USART1_Init 1 */

	/* USER CODE END USART1_Init 1 */
	LL_USART_StructInit(&USART_InitStruct);
	USART_InitStruct.BaudRate = 115200;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX;
	LL_USART_Init(USART1, &USART_InitStruct);
	LL_USART_ConfigAsyncMode(USART1);
	USART1->BRR = 0x30D;
	LL_USART_Enable(USART1);
}//--------------------------------------------------------------------------------
void CL_printMsg(char *msg, ...)
{	
	char buff[150];	
	va_list args;
	va_start(args, msg);
	vsprintf(buff, msg, args);
		
	for (int i = 0; i < strlen(buff); i++)
	{
		
		USART1->DR = buff[i];
		while (!(USART1->SR & USART_SR_TXE)) ;
	}		
	while (!(USART1->SR & USART_SR_TC)) ;		
		
}
#endif


//***************************************************************
//						Univeral Print functions
//***************************************************************

