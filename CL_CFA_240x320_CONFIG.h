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

//--------------| TOUCH I2C CONTROLLER
#define TOUCH_RESET_PIN 2
#define TOUCH_INT_PIN	3

//--------------|  TFT SPI Controller
#define SPIPORT (PORTB)
#define SPITOGGLE (PINB)
// PB0 (0x01) is RS   (output) red    OLED pin 9
#define SPI_RS_PIN (8)
#define RS_MASK (0x01)
#define CLR_RS (SPIPORT &= ~(RS_MASK))
#define SET_RS (SPIPORT |= (RS_MASK))
// PB1 (0x02) is RESET  (output) yellow OLED pin 7
#define SPI_RESET_PIN (9)
#define RESET_MASK (0x02)
#define CLR_RESET (SPIPORT &= ~(RESET_MASK))
#define SET_RESET (SPIPORT |= (RESET_MASK))
// PB2 (0x04) is CS   (output) gray   OLED pin 15
#define SPI_CS_PIN (10)
#define CS_MASK (0x04)
#define CLR_CS (SPIPORT &= ~(CS_MASK))
#define SET_CS (SPIPORT |= (CS_MASK))
// PB3 (0x08) is MOSI (output) violet OLED pin 14
#define SPI_MOSI_PIN (11)
#define MOSI_MASK (0x08)
#define CLR_MOSI (SPIPORT &= ~(MOSI_MASK))
#define SET_MOSI (SPIPORT |= (MOSI_MASK))
// PB4 (0x10) is MISO (input)  blue   OLED pin 13
//(reference only, it is an input)
#define SPI_MISO_PIN (12)
#define MISO_MASK (0x10)
#define CLR_MISO (SPIPORT &= ~(MISO_MASK))
#define SET_MISO (SPIPORT |= (MISO_MASK))
// PB5 (0x20) is CLK  (output) green  OLED pin 12
#define SPI_CLK_PIN (13)
#define CLK_MASK (0x20)
#define CLR_CLK (SPIPORT &= ~(CLK_MASK))
#define SET_CLK (SPIPORT |= (CLK_MASK))


#endif