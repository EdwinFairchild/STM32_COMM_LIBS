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





#endif //End of header #if
