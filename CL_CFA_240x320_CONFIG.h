#ifndef CL_CFA_240x320_CONFIG_H_
#define CL_CFA_240x320_CONFIG_H_

/*
 * Pin Definitions / Maping to TFT and Touch
 * 
 * LCD_MOSI		24 -->	PA7 SPI MOSI
 * LCD_SCL		33  --> PA5 SPI CLK
 * LCD_ WRB		32 -->  PB7
 * LCD_CS		34 -->  PB6
 * LCD_RESET	35 -->  PB5
 * 
 * Touch_GND     1/12
 * Touch_Reset	2/11 --> PB4
 * Touch_IRQ	3/10 --> PB3
 * Touch_SDA	4/9  --> PA8 I2C SDA
 * Touch_SCL	5/8  --> PA9 I2C CLK
 * Touch_V+     6/7
 * 
 *
 **/
#ifdef CL_USING_G4 //--------------------------  USING G4

#include "stm32g4xx.h"
#endif

#ifdef CL_USING_L0 //--------------------------  USING G4

#include "stm32g4xx.h"
#endif

#ifdef CL_USING_F1 //--------------------------  USING G4

#include "stm32f1xx.h"

#endif


//--------------| TOUCH I2C CONTROLLER
#define TOUCH_RESET_PIN 4 //PORTB
#define TOUCH_INT_PIN	3 //PORTB


//--------------|  TFT SPI Controller
#define SPIPORT GPIOB



//----------PB7  is RS   (output) red    OLED pin 9
#define SPI_RS_PIN (7)

#define CLR_RS (GPIOB->BSRR =(1<<(SPI_RS_PIN + 16)))
#define SET_RS (GPIOB->BSRR =(1<<SPI_RS_PIN ) )

//----------PB1 (0x02) is RESET  (output) yellow OLED pin 7
#define SPI_RESET_PIN (5)

#define CLR_RESET (GPIOB->BSRR =(1<<(SPI_RESET_PIN + 16)))
#define SET_RESET (GPIOB->BSRR =(1<<SPI_RESET_PIN ))

//----------PB2 (0x04) is CS   (output) gray   OLED pin 15
#define SPI_CS_PIN (6)

#define CLR_CS (GPIOB->BSRR =(1<<(SPI_CS_PIN + 16))) //make CS PIN go LOW
#define SET_CS (GPIOB->BSRR =(1<<SPI_CS_PIN ))




#endif
