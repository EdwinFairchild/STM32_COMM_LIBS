//==============================================================================
//
//  CRYSTALFONTZ CFAF240320A0-024SC
//
//  Code written for Seeeduino v4.2 set to 3.3v (important!)
//
//  This code uses the 4-wire SPI mode of the display,
//  and I2C mode for the touch panel interface.
//
//  The controller is a Sitronix ST7789V:
//    https://www.crystalfontz.com/controllers/Sitronix/ST7789V
//
//  Seeeduino v4.2, an open-source 3.3v capable Arduino clone.
//    https://www.seeedstudio.com/Seeeduino-V4.2-p-2517.html
//    https://github.com/SeeedDocument/SeeeduinoV4/raw/master/resources/Seeeduino_v4.2_sch.pdf
//
//==============================================================================
//
//  2017-10-02 Brent A. Crosby / Crystalfontz
//  2018-08-22 Max Roberg / Crystalfontz
//  2019-09-09 Mark Williams / Crystalfontz
//  2020-08-03 Kelsey Zaches / Crystalfontz
//
//==============================================================================
//This is free and unencumber ed software released into the public domain.
//
//Anyone is free to copy, modify, publish, use, compile, sell, or
//distribute this software, either in source code form or as a compiled
//binary, for any purpose, commercial or non-commercial, and by any
//means.
//
//In jurisdictions that recognize copyright laws, the author or authors
//of this software dedicate any and all copyright interest in the
//software to the public domain. We make this dedication for the benefit
//of the public at large and to the detriment of our heirs and
//successors. We intend this dedication to be an overt act of
//relinquishment in perpetuity of all present and future rights to this
//software under copyright law.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//OTHER DEALINGS IN THE SOFTWARE.
//
//For more information, please refer to <http://unlicense.org/>
//==============================================================================

// LCD / Touch Panel / Seeeduino / microSD Wiring Connections

// |  LCD Pin   | Seeeduino Pin |    Connection Description    |
// |------------|---------------|------------------------------|
// | 1 (LED-A)  |               | LED Power (Approx 9V @ 40mA) |
// | 2 (LED-K)  |               | LED Ground                   |
// | 3 (IOVCC)  |               | +3.3V Power                  |
// | 24 (SDA)   | 11            | LCD MOSI                     |
// | 25 (GND)   | GND           | Ground                       |
// | 27 (GND)   | GND           | Ground                       |
// | 32 (WRB)   | 8             | LCD R/S                      |
// | 33 (RS)    | 13            | LCD SCK                      |
// | 34 (CSB)   | 10            | LCD CS                       |
// | 35 (RESET) | 9             | LCD Reset                    |
// | 36 (IM0)   | GND           | Ground                       |
// | 37 (IM1)   |               | +3.3V Power                  |
// | 38 (IM2)   |               | +3.3V Power                  |
// | 39 (VCI)   |               | +3.3V Power                  |
// | 40 (GND)   | GND           | Ground                       |

// | Touch Panel Pin | Seeeduino Pin | Connection Description |
// |-----------------|---------------|------------------------|
// | 1 (GND)         | GND           | Ground                 |
// | 2 (RESET)       | 2             | TP Reset               |
// | 3 (INT)         | 3             | TP Interrupt           |
// | 4 (SDA)         | SDA           | I2C Data               |
// | 5 (SCL)         | SCL           | I2C Clock              |
// | 6 (VDD)         |               | +3.3V Power            |

// | microSD Pin | Seeeduino Pin | Connection Description |
// |-------------|---------------|------------------------|
// | 2 (CS)      | 7             | SD CS                  |
// | 3 (DI)      | 11            | SD MOSI                |
// | 4 (VDD)     |               | +3.3V Power            |
// | 5 (SCLK)    | 13            | SD SCLK                |
// | 6 (VSS)     | GND           | Ground                 |
// | 7 (DO)      | 12            | SD MISO                |
//
// (microSD use is optional, if using enable DEMO_BMPIMAGES below)

//==============================================================================
//--------------|  This is an STM32 Port By www.edwinfairchild.com |------------


#ifndef CL_CFA_240x320_H_
#define CL_CFA_240x320_H_

#include "CL_CONFIG.h"
#include "CL_CFA_240x320_CONFIG.h"
#include "font-ascii-12x16.h"
#include <stdint.h>
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"
#include "string.h"

#define mSwap(a, b, t) \
	{                  \
		t = a;         \
		a = b;         \
		b = t;         \
	}

#define F12CENTREX(a) ((240-((a)*13))/2) /*center text*/

//-------------------------| TOUCH Controller
#define TOUCH_I2C_ADDR 0x55
#define SITRONIX_MAX_SUPPORTED_POINT 8

#define X_RES_H_SHFT 4
#define X_RES_H_BMSK 0xf
#define Y_RES_H_SHFT 0
#define Y_RES_H_BMSK 0xf
#define FINGERS_SHFT 0
#define FINGERS_BMSK 0xf
#define X_COORD_VALID_SHFT 7
#define X_COORD_VALID_BMSK 0x1
#define X_COORD_H_SHFT 4
#define X_COORD_H_BMSK 0x7
#define Y_COORD_H_SHFT 0
#define Y_COORD_H_BMSK 0x7


//==============================================================================
// Defines for the ST7789 registers.
// ref: https://www.crystalfontz.com/controllers/Sitronix/ST7789V/
#define ST7789_00_NOP (0x00)
#define ST7789_01_SWRESET (0x01)
#define ST7789_04_RDDID (0x04)
#define ST7789_09_RDDST (0x09)
#define ST7789_0A_RDDPM (0x0A)
#define ST7789_0B_RDDMADCTL (0x0B)
#define ST7789_0C_RDDCOLMOD (0x0C)
#define ST7789_0D_RDDIM (0x0D)
#define ST7789_0E_RDDSM (0x0E)
#define ST7789_0F_RDDSDR (0x0F)
#define ST7789_10_SLPIN (0x10)
#define ST7789_11_SLPOUT (0x11)
#define ST7789_12_PTLON (0x12)
#define ST7789_13_NORON (0x13)
#define ST7789_20_INVOFF (0x20)
#define ST7789_21_INVON (0x21)
#define ST7789_26_GAMSET (0x26)
#define ST7789_28_DISPOFF (0x28)
#define ST7789_29_DISPON (0x29)
#define ST7789_2A_CASET (0x2A)
#define ST7789_2B_RASET (0x2B)
#define ST7789_2C_RAMWR (0x2C)
#define ST7789_2E_RAMRD (0x2E)
#define ST7789_30_PTLAR (0x30)
#define ST7789_33_VSCRDEF (0x33)
#define ST7789_34_TEOFF (0x34)
#define ST7789_35_TEON (0x35)
#define ST7789_36_MADCTL (0x36)
#define ST7789_37_VSCRSADD (0x37)
#define ST7789_38_IDMOFF (0x38)
#define ST7789_39_IDMON (0x39)
#define ST7789_3A_COLMOD (0x3A)
#define ST7789_3C_RAMWRC (0x3C)
#define ST7789_3E_RAMRDC (0x3E)
#define ST7789_44_TESCAN (0x44)
#define ST7789_45_RDTESCAN (0x45)
#define ST7789_51_WRDISBV (0x51)
#define ST7789_52_RDDISBV (0x52)
#define ST7789_53_WRCTRLD (0x53)
#define ST7789_54_RDCTRLD (0x54)
#define ST7789_55_WRCACE (0x55)
#define ST7789_56_RDCABC (0x56)
#define ST7789_5E_WRCABCMB (0x5E)
#define ST7789_5F_RDCABCMB (0x5F)
#define ST7789_68_RDABCSDR (0x68)
#define ST7789_B0_RAMCTRL (0xB0)
#define ST7789_B1_RGBCTRL (0xB1)
#define ST7789_B2_PORCTRL (0xB2)
#define ST7789_B3_FRCTRL1 (0xB3)
#define ST7789_B7_GCTRL (0xB7)
#define ST7789_BA_DGMEN (0xBA)
#define ST7789_BB_VCOMS (0xBB)
#define ST7789_C0_LCMCTRL (0xC0)
#define ST7789_C1_IDSET (0xC1)
#define ST7789_C2_VDVVRHEN (0xC2)
#define ST7789_C3_VRHS (0xC3)
#define ST7789_C4_VDVSET (0xC4)
#define ST7789_C5_VCMOFSET (0xC5)
#define ST7789_C6_FRCTR2 (0xC6)
#define ST7789_C7_CABCCTRL (0xC7)
#define ST7789_C8_REGSEL1 (0xC8)
#define ST7789_CA_REGSEL2 (0xCA)
#define ST7789_CC_PWMFRSEL (0xCC)
#define ST7789_D0_PWCTRL1 (0xD0)
#define ST7789_D2_VAPVANEN (0xD2)
#define ST7789_DA_RDID1 (0xDA)
#define ST7789_DB_RDID2 (0xDB)
#define ST7789_DC_RDID3 (0xDC)
#define ST7789_DF_CMD2EN (0xDF)
#define ST7789_E0_PVGAMCTRL (0xE0)
#define ST7789_E1_NVGAMCTRL (0xE1)
#define ST7789_E2_DGMLUTR (0xE2)
#define ST7789_E3_DGMLUTB (0xE3)
#define ST7789_E4_GATECTRL (0xE4)
#define ST7789_E8_PWCTRL2 (0xE8)
#define ST7789_E9_EQCTRL (0xE9)
#define ST7789_EC_PROMCTRL (0xEC)
#define ST7789_FA_PROMEN (0xFA)
#define ST7789_FC_NVMSET (0xFC)
#define ST7789_FE_PROMACT (0xFE)


//--------------| Prototypes

void F12x16__printMsg(uint16_t x, uint16_t y, char *msg, ...);
void F12x16_DrawString(uint16_t x, uint16_t y, const char* text);
void F12x16_DrawChar(uint16_t x, uint16_t y, char c);
void SPI_send_pixels(uint8_t len, uint8_t* data);
void LCD_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t r, uint8_t g, uint8_t b);
void Fast_Horizontal_Line(uint16_t x0, uint16_t y, uint16_t x1, uint8_t r, uint8_t g, uint8_t b);
void LCD_Circle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t R, uint16_t G, uint16_t B);
void Put_Pixel(uint16_t x, uint16_t y, uint8_t R, uint8_t G, uint8_t B);
void Fill_LCD(uint8_t R, uint8_t G, uint8_t B);
void Set_LCD_for_write_at_X_Y(uint16_t x, uint16_t y);
void Initialize_LCD(void);
void SPI_sendCommand(uint8_t cmd);
void SPI_sendData(uint8_t data);
void uGui_set_pixel(uint16_t x, uint16_t y, uint32_t color);
#endif //End of header #if
