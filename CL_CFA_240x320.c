#include "CL_CFA_240x320.h"

//STM32 Specific
uint8_t volatile *spidr = ((__IO uint8_t *)&SPI1->DR);
extern void spiSend(uint8_t *data, uint32_t len);
extern void delayMS(uint32_t ms); 


//--------------|  Capacitive Touch ST1624 Controller (i2c)
typedef enum
{
	FIRMWARE_VERSION,
	STATUS_REG,
	DEVICE_CONTROL_REG,
	TIMEOUT_TO_IDLE_REG,
	XY_RESOLUTION_HIGH,
	X_RESOLUTION_LOW,
	Y_RESOLUTION_LOW,
	DEVICE_CONTROL_REG2 = 0x09,
	FIRMWARE_REVISION_3 = 0x0C,
	FIRMWARE_REVISION_2,
	FIRMWARE_REVISION_1,
	FIRMWARE_REVISION_0,
	FINGERS,
	KEYS_REG,
	XY0_COORD_H,
	X0_COORD_L,
	Y0_COORD_L,
	I2C_PROTOCOL        = 0x3E,
	MAX_NUM_TOUCHES,
	DATA_0_HIGH,
	DATA_0_LOW,
	MISC_CONTROL        = 0xF1,
	SMART_WAKE_UP_REG   = 0xF2,
	CHIP_ID             = 0xF4,
	PAGE_REG            = 0xff,
} RegisterOffset;

typedef enum {
	XY_COORD_H,
	X_COORD_L,
	Y_COORD_L,
} DataCoords;



struct sitronix_ts_point
{
	uint8_t valid;
	uint16_t x;
	uint16_t y;
};

struct sitronix_ts_data
{
	uint8_t fw_version;
	uint8_t fw_revision[4];
	int resolution_x;
	int resolution_y;
	uint8_t max_touches;
	//polled data
	uint8_t water_flag;
	uint8_t proximity_flag;
	uint8_t gesture_type;
	uint8_t keys_bits;
	struct sitronix_ts_point coords[SITRONIX_MAX_SUPPORTED_POINT];
};

struct RGB
{
	uint8_t r;
	uint8_t g;
	uint8_t b;
};

const struct RGB touch_colors[SITRONIX_MAX_SUPPORTED_POINT] =
{
	{ 0xFF, 0x00, 0x00 },
	{ 0x00, 0xFF, 0x00 },
	{ 0x00, 0x00, 0xFF },
	{ 0xFF, 0xFF, 0x00 },
	{ 0xFF, 0x00, 0xFF },
	{ 0x00, 0xFF, 0xFF },
	{ 0xFF, 0xFF, 0xFF },
	{ 0x80, 0x80, 0x80 }
};





void F12x16__printMsg(uint16_t x, uint16_t y, char *msg, ...)
{	
	char buff[80];	
	va_list args;
	va_start(args, msg);
	vsprintf(buff, msg, args);
		
	for (int i = 0; i < strlen(buff); i++)
	{
		
		
		x += FONT_ASCII_12X16_WIDTH;
		F12x16_DrawChar(x, y, buff[i]);
	}		
		
	
}

void F12x16_DrawString(uint16_t x, uint16_t y, const char* text)
{
	uint8_t c;

	c = 0;
	while (text[c] != 0)
	{
		x += FONT_ASCII_12X16_WIDTH;
		F12x16_DrawChar(x, y, text[c]);
		c++;
	}
}
void F12x16_DrawChar(uint16_t x, uint16_t y, char c)
{
	uint8_t state;
	uint8_t xc, yc;

	//draw character to frame buffer
	if(c > FONT_ASCII_12X16_BASE - 1)
	{
		c -= FONT_ASCII_12X16_BASE;
		if (c > FONT_ASCII_12X16_CHARNUM)
			c = ' ';

		for (yc = 0; yc < FONT_ASCII_12X16_HEIGHT; yc++)
		{
			//12x16 is two bytes wide
			uint16_t w;
			uint8_t b;
			b = Font_ASCII_12X16[((c * FONT_ASCII_12X16_HEIGHT) + yc) * 2];
			w = b << 8;
			b = Font_ASCII_12X16[(((c * FONT_ASCII_12X16_HEIGHT) + yc) * 2) + 1];
			w |= b;

			//			w = (Font_ASCII_12X16[((c * FONT_ASCII_12X16_HEIGHT) + yc) * 2] << 8) |
			//				Font_ASCII_12X16[(((c * FONT_ASCII_12X16_HEIGHT) + yc) * 2) + 1];

			for(xc = 0 ; xc < FONT_ASCII_12X16_WIDTH ; xc++)
			{
				state = (w >> (15 - xc)) & 1 ? 0xFF : 0x00;
				if (state)
					//only write "on" pixels
					Put_Pixel(x + xc, y + yc, state, state, state);
			}
		}
	}
}
void SPI_send_pixels(uint8_t len, uint8_t* data)
{
	uint8_t subpixel;

	// Select the LCD's data register
	SET_RS;
	// Select the LCD controller
	CLR_CS;

	uint8_t volatile *spidr = ((__IO uint8_t *)&SPI1->DR);
	while (len > 0)
	{
		while (!(SPI1->SR&SPI_SR_TXE)) {
			;
		}
		*spidr = *data++;
		len--;
	}	
	while ((SPI1->SR&SPI_SR_BSY)) {
		;
	}
	SET_CS;
}
void LCD_Line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint8_t r, uint8_t g, uint8_t b)
{
	int16_t dx;
	int16_t sx;
	int16_t dy;
	int16_t sy;
	int16_t err;
	int16_t e2;

	//General case
	if(y0 != y1)
	{
		dx = abs((int16_t)x1 - (int16_t)x0);
		sx = x0 < x1 ? 1 : -1;
		dy = abs((int16_t)y1 - (int16_t)y0);
		sy = y0 < y1 ? 1 : -1;
		err = (dx > dy ? dx : -dy) / 2;

		for (;;)
		{
			Put_Pixel(x0, y0, r, g, b);
			if ((x0 == x1) && (y0 == y1))
				break;
			e2 = err;
			if (e2 > -dx)
			{
				err -= dy;
				x0 = (uint16_t)((int16_t)x0 + sx);
			}
			if (e2 < dy)
			{
				err += dx;
				y0 = (uint16_t)((int16_t)y0 + sy);
			}
		}
	}
	else
	{
		//Optimized for LCD
		Fast_Horizontal_Line(x0, y0, x1, r, g, b);
	}
}
void Fast_Horizontal_Line(uint16_t x0, uint16_t y, uint16_t x1, uint8_t r, uint8_t g, uint8_t b)
{
	uint16_t temp;
	if (x1 < x0)
		mSwap(x0, x1, temp);
	Set_LCD_for_write_at_X_Y(x0, y);
	while (x0 <= x1)
	{
		//Write the single pixel's worth of data
		SPI_sendData(r);   //Blue
		SPI_sendData(g);   //Green
		SPI_sendData(b);   //Red
		x0++;
	}
}
void LCD_Circle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t R, uint16_t G, uint16_t B)
{
	uint16_t x = radius;
	uint16_t y = 0;
	int16_t radiusError = 1 - (int16_t)x;

	while (x >= y)
	{
		//11 O'Clock
		Put_Pixel(x0 - y, y0 + x, R, G, B);
		//1 O'Clock
		Put_Pixel(x0 + y, y0 + x, R, G, B);
		//10 O'Clock
		Put_Pixel(x0 - x, y0 + y, R, G, B);
		//2 O'Clock
		Put_Pixel(x0 + x, y0 + y, R, G, B);
		//8 O'Clock
		Put_Pixel(x0 - x, y0 - y, R, G, B);
		//4 O'Clock
		Put_Pixel(x0 + x, y0 - y, R, G, B);
		//7 O'Clock
		Put_Pixel(x0 - y, y0 - x, R, G, B);
		//5 O'Clock
		Put_Pixel(x0 + y, y0 - x, R, G, B);

		y++;
		if (radiusError < 0)
			radiusError += (int16_t)(2 * y + 1);
		else
		{
			x--;
			radiusError += 2 * (((int16_t)y - (int16_t)x) + 1);
		}
	}
}
void Put_Pixel(uint16_t x, uint16_t y, uint8_t R, uint8_t G, uint8_t B)
{
	// Select the LCD controller
	CLR_CS;
	//CASET (2Ah): Column Address Set
	// * The value of XS [15:0] and XE [15:0] are referred when RAMWR
	//   command comes.
	// * Each value represents one column line in the Frame Memory.
	// * XS [15:0] always must be equal to or less than XE [15:0]
	SPI_sendCommand(ST7789_2A_CASET);   //Column address set
	//Write the parameters for the "column address set" command
	SPI_sendData(x >> 8);  	 //Start MSB = XS[15:8]
	SPI_sendData(x & 0x00FF);   //Start LSB = XS[ 7:0]
	SPI_sendData(0);  		  //End MSB   = XE[15:8] 240-1
	SPI_sendData(240);  		  //End LSB   = XE[ 7:0]
	//Write the "row address set" command to the LCD
	//RASET (2Bh): Row Address Set
	// * The value of YS [15:0] and YE [15:0] are referred when RAMWR
	//   command comes.
	// * Each value represents one row line in the Frame Memory.
	// * YS [15:0] always must be equal to or less than YE [15:0]
	SPI_sendCommand(ST7789_2B_RASET);   //Row address set

	//Use 1st quadrant coordinates: 0,0 is lower left, 239,319 is upper right.
	y = 319 - y;
	//Write the parameters for the "row address set" command
	SPI_sendData(y >> 8);  	 //Start MSB = YS[15:8]
	SPI_sendData(y & 0x00FF);   //Start LSB = YS[ 7:0]
	SPI_sendData(0x01);  		  //End MSB   = YE[15:8] 320-1
	SPI_sendData(0x3F);  		  //End LSB   = YE[ 7:0]
	//Write the "write data" command to the LCD
	//RAMWR (2Ch): Memory Write
	SPI_sendCommand(ST7789_2C_RAMWR);   //write data
	//Write the single pixel's worth of data
	SPI_sendData(B);   //Blue
	SPI_sendData(G);   //Green
	SPI_sendData(R);   //Red
	// Deselect the LCD controller
	SET_CS;
}
void Fill_LCD(uint8_t R, uint8_t G, uint8_t B)
{
	uint32_t i;
	Set_LCD_for_write_at_X_Y(0, 319);

	// Select the LCD controller
	CLR_CS;
	// Select the LCD's data register
	SET_RS;

	//Fill display with a given RGB value
	for(i = 0 ; i < (320UL * 240UL) ; i++)
	{
		//SPI.transfer(B);  //Blue
		//SPI.transfer(G);  //Green
		//SPI.transfer(R);  //Red
		spiSend(&B, 1);
		spiSend(&G, 1);
		spiSend(&R, 1);
	}
	// Deselect the LCD controller
	SET_CS;
}
void Set_LCD_for_write_at_X_Y(uint16_t x, uint16_t y)
{
	//CASET (2Ah): Column Address Set
	// * The value of XS [15:0] and XE [15:0] are referred when RAMWR
	//   command comes.
	// * Each value represents one column line in the Frame Memory.
	// * XS [15:0] always must be equal to or less than XE [15:0]
	SPI_sendCommand(ST7789_2A_CASET);   //Column address set
	//Write the parameters for the "column address set" command
	SPI_sendData(x >> 8);  	 //Start MSB = XS[15:8]
	SPI_sendData(x & 0x00FF);   //Start LSB = XS[ 7:0]
	SPI_sendData(0);  		  //End MSB   = XE[15:8] 240-1
	SPI_sendData(240);  		  //End LSB   = XE[ 7:0]
	//Write the "row address set" command to the LCD
	//RASET (2Bh): Row Address Set
	// * The value of YS [15:0] and YE [15:0] are referred when RAMWR
	//   command comes.
	// * Each value represents one row line in the Frame Memory.
	// * YS [15:0] always must be equal to or less than YE [15:0]
	SPI_sendCommand(ST7789_2B_RASET);   //Row address set
	//Write the parameters for the "row address set" command
	//Use 1st quadrant coordinates: 0,0 is lower left, 239,319 is upper right.
	y = 319 - y;
	SPI_sendData(y >> 8);  	 //Start MSB = YS[15:8]
	SPI_sendData(y & 0x00FF);   //Start LSB = YS[ 7:0]
	SPI_sendData(0x01);  		  //End MSB   = YE[15:8] 320-1
	SPI_sendData(0x3F);  		  //End LSB   = YE[ 7:0]
	//Write the "write data" command to the LCD
	//RAMWR (2Ch): Memory Write
	SPI_sendCommand(ST7789_2C_RAMWR);   //write data
}
void Initialize_LCD(void)
{
	//Reset the LCD controller
	CLR_RESET;
	delayMS(1);   //10µS min
	SET_RESET;
	delayMS(150);   //120mS max

	// SLPOUT (11h): Sleep Out ("Sleep Out"  is chingrish for "wake")
	// The DC/DC converter is enabled, Internal display oscillator
	// is started, and panel scanning is started.
	SPI_sendCommand(ST7789_11_SLPOUT);
	delayMS(120);   //Delay 120ms

	// MADCTL (36h): Memory Data Access Control
	// Set the RGB vs BGR order to match a windows 24-bit BMP
	SPI_sendCommand(ST7789_36_MADCTL);
	SPI_sendData(0x08 | 0x80);   // YXVL RH--
	//SPI_sendData(0x08); // YXVL RH--
						// |||| ||||-- Unused: 0
						// |||| ||---- MH: Horizontal Refresh Order
						// |||| |        0 = left to right
						// |||| |        1 = right to left
						// |||| |----- RGB: RGB vs BGR Order
						// ||||          0 = RGB color filter panel
						// ||||          1 = BGR color filter panel
						// ||||------- ML: Vertical Refresh Order
						// |||           0 = top to bottom
						// |||           1 = bottom to top
						// |||-------- MV: Row / Column Exchange
						// ||--------- MX: Column Address Order  <<<<<
						// |---------- MY: Row Address Order

	// COLMOD (3Ah): Interface Pixel Format
	// * This command is used to define the format of RGB picture
	//   data, which is to be transferred via the MCU interface.
	SPI_sendCommand(ST7789_3A_COLMOD);
	SPI_sendData(0x06);   // Default: 0x06 => 18-bit/pixel
						// IFPF[2:0] MCU Interface Color Format
						// IFPF[2:0] | Format
						//      000b | reserved
						//      001b | reserved
						//      010b | reserved
						//      011b | 12-bit/pixel
						//      100b | reserved
						//      101b | 16-bit/pixel
						//      110b | 18-bit/pixel   <<<<<
						//      111b | reserved

	// PORCTRL (B2h): Porch Setting
	SPI_sendCommand(ST7789_B2_PORCTRL);
	SPI_sendData(0x0C);   //BPA[6:0]: Back porch setting in normal mode. The minimum setting is 0x01.
	SPI_sendData(0x0C);   //FPA[6:0]: Front porch setting in normal mode. The minimum setting is 0x01.
	SPI_sendData(0x00);   //Disable(0) / Enable (1) separate porch control
	SPI_sendData(0x33);   //(high nibble) BPB[3:0]: Back porch setting in idle mode. The minimum setting is 0x01.
						//(low nibble)  FPB[3:0]: Front porch setting in idle mode. The minimum setting is 0x01.
	SPI_sendData(0x33);   //(high nibble) BPB[3:0]: Back porch setting in partial mode. The minimum setting is 0x01.
						//(low nibble)  FPC[3:0]: Front porch setting in partial mode. The minimum setting is 0x01.

	// GCTRL (B7h): Gate Control
	SPI_sendCommand(ST7789_B7_GCTRL);
	SPI_sendData(0x35);

	//(high nibble) VGHS[2:0]: VGH Setting.
	// 0x0- => 12.20v
	// 0x1- => 12.54v
	// 0x2- => 12.89v
	// 0x3- => 13.26v <<<<<
	// 0x4- => 13.65v
	// 0x5- => 14.06v
	// 0x6- => 14.50v
	// 0x7- => 14.97v
	//(low nibble) VGLS[2:0]: VGL Setting.
	// 0x-0 =>  -7.16v
	// 0x-1 =>  -7.67v
	// 0x-2 =>  -8.23v
	// 0x-3 =>  -8.87v
	// 0x-4 =>  -9.60v
	// 0x-5 => -10.43v <<<<<
	// 0x-6 => -11.38v
	// 0x-7 => -12.50v

	// VCOMS (BBh): VCOM Setting
	SPI_sendCommand(ST7789_BB_VCOMS);
	SPI_sendData(0x2B);

	// VCOMS[5:0]
	// 0x00 => 0.100v
	// 0x01 => 0.125v
	// 0x02 => 0.150v
	// 0x03 => 0.175v
	// 0x04 => 0.200v
	// 0x05 => 0.225v
	// 0x06 => 0.250v
	// 0x07 => 0.275v
	// 0x08 => 0.300v
	// 0x09 => 0.325v
	// 0x0A => 0.350v
	// 0x0B => 0.375v
	// 0x0C => 0.400v
	// 0x0D => 0.425v
	// 0x0E => 0.450v
	// 0x0F => 0.475v
	// 0x10 => 0.500v
	// 0x11 => 0.525v
	// 0x12 => 0.550v
	// 0x13 => 0.575v
	// 0x14 => 0.600v
	// 0x15 => 0.625v
	// 0x16 => 0.650v
	// 0x17 => 0.675v
	// 0x18 => 0.700v
	// 0x19 => 0.725v
	// 0x1A => 0.750v
	// 0x1B => 0.775v
	// 0x1C => 0.800v <<<< V
	// 0x1D => 0.825v
	// 0x1E => 0.850v
	// 0x1F => 0.875v
	// 0x20 => 0.900v
	// 0x21 => 0.925v
	// 0x22 => 0.950v
	// 0x23 => 0.975v
	// 0x24 => 1.000v
	// 0x25 => 1.025v
	// 0x26 => 1.050v
	// 0x27 => 1.075v
	// 0x28 => 1.100v
	// 0x29 => 1.125v
	// 0x2A => 1.150v
	// 0x2B => 1.175v <<<< W
	// 0x2C => 1.200v
	// 0x2D => 1.225v
	// 0x2E => 1.250v
	// 0x2F => 1.275v
	// 0x30 => 1.300v
	// 0x31 => 1.325v
	// 0x32 => 1.350v
	// 0x33 => 1.375v
	// 0x34 => 1.400v
	// 0x35 => 1.425v <<<< X
	// 0x36 => 1.450v
	// 0x37 => 1.475v
	// 0x38 => 1.500v
	// 0x39 => 1.525v
	// 0x3A => 1.550v
	// 0x3B => 1.575v
	// 0x3C => 1.600v
	// 0x3D => 1.625v
	// 0x3E => 1.650v
	// 0x3F => 1.675v

	// LCMCTRL (C0h): LCM Control
	SPI_sendCommand(ST7789_C0_LCMCTRL);
	SPI_sendData(0x2C);
	// 0010 1100
	// |||| ||||--  GS: Gate scan inversion enable: 1: enable, 0: disable.
	// |||| |||---  XMX: XOR MX setting in command 36h.
	// |||| ||---- *XMV: XOR MV setting in command 36h.
	// |||| |----- *XMH: XOR RGB setting in command 36h, in RGB interface
	// ||||             without RAM mode can support column address order.
	// ||||-------  XREV: XOR inverse setting in command 21h.
	// |||-------- *XBGR: XOR RGB setting in command 36h.
	// ||---------  XMY: XOR MY setting in command 36h.
	// |----------  Unused: 0

	// VDVVRHEN (C2h): VDV and VRH Command Enable
	SPI_sendCommand(ST7789_C2_VDVVRHEN);
	SPI_sendData(0x01);   // CMDEN=”0”: VDV and VRH register value comes from NVM.
						// CMDEN=”1”, VDV and VRH register value comes from command write.
	SPI_sendData(0xFF);

	// VRHS (C3h): VRH Set
	SPI_sendCommand(ST7789_C3_VRHS);
	SPI_sendData(0x20);

	// 0x00 => VAP(GVDD)(V) = +3.55 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) = -3.55 + (vcom + vcom_offset - 0.5vdv)
	// 0x01 => VAP(GVDD)(V) = +3.60 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) = -3.60 + (vcom + vcom_offset - 0.5vdv)
	// 0x02 => VAP(GVDD)(V) = +3.65 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) = -3.65 + (vcom + vcom_offset - 0.5vdv)
	// 0x03 => VAP(GVDD)(V) = +3.70 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 3.70 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x04 => VAP(GVDD)(V) = +3.75 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 3.75 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x05 => VAP(GVDD)(V) = +3.80 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 3.80 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x06 => VAP(GVDD)(V) = +3.85 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 3.85 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x07 => VAP(GVDD)(V) = +3.90 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 3.90 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x08 => VAP(GVDD)(V) = +3.95 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 3.95 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x09 => VAP(GVDD)(V) = +4.00 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.00 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x0A => VAP(GVDD)(V) = +4.05 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.05 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x0B => VAP(GVDD)(V) = +4.10 + (vcom + vcom_offset + 0.5vdv) <<<< V
	//         VAN(GVCL)(V) =- 4.10 + (vcom + vcom_offset-+ 0.5vdv) <<<< V
	// 0x0C => VAP(GVDD)(V) = +4.15 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.15 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x0D => VAP(GVDD)(V) = +4.20 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.20 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x0E => VAP(GVDD)(V) = +4.25 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.25 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x0F => VAP(GVDD)(V) = +4.30 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.30 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x10 => VAP(GVDD)(V) = +4.35 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.35 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x11 => VAP(GVDD)(V) = +4.40 + (vcom + vcom_offset + 0.5vdv) <<<< X
	//         VAN(GVCL)(V) =- 4.40 + (vcom + vcom_offset-+ 0.5vdv) <<<< X
	// 0x12 => VAP(GVDD)(V) = +4.45 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.45 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x13 => VAP(GVDD)(V) = +4.50 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.50 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x14 => VAP(GVDD)(V) = +4.55 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.55 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x15 => VAP(GVDD)(V) = +4.60 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.60 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x16 => VAP(GVDD)(V) = +4.65 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.65 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x17 => VAP(GVDD)(V) = +4.70 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.70 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x18 => VAP(GVDD)(V) = +4.75 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.75 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x19 => VAP(GVDD)(V) = +4.80 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.80 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x1A => VAP(GVDD)(V) = +4.85 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.85 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x1B => VAP(GVDD)(V) = +4.90 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.90 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x1C => VAP(GVDD)(V) = +4.95 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 4.95 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x1D => VAP(GVDD)(V) = +5.00 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 5.00 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x1E => VAP(GVDD)(V) = +5.05 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 5.05 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x1F => VAP(GVDD)(V) = +5.10 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 5.10 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x20 => VAP(GVDD)(V) = +5.15 + (vcom + vcom_offset + 0.5vdv) <<<< W
	//         VAN(GVCL)(V) =- 5.15 + (vcom + vcom_offset-+ 0.5vdv) <<<< W
	// 0x21 => VAP(GVDD)(V) = +5.20 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 5.20 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x22 => VAP(GVDD)(V) = +5.25 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 5.25 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x23 => VAP(GVDD)(V) = +5.30 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 5.30 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x24 => VAP(GVDD)(V) = +5.35 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 5.35 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x25 => VAP(GVDD)(V) = +5.40 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 5.40 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x26 => VAP(GVDD)(V) = +5.45 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 5.45 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x27 => VAP(GVDD)(V) = +5.50 + (vcom + vcom_offset + 0.5vdv)
	//         VAN(GVCL)(V) =- 5.50 + (vcom + vcom_offset-+ 0.5vdv)
	// 0x28~0x3Fh => Reserved

	//VDVS (C4h): VDV Set
	SPI_sendCommand(ST7789_C4_VDVSET);
	SPI_sendData(0x20);
	// 0x00 => -0.800
	// 0x01 => -0.775
	// 0x02 => -0.750
	// 0x03 => -0.725
	// 0x04 => -0.700
	// 0x05 => -0.675
	// 0x06 => -0.650
	// 0x07 => -0.625
	// 0x08 => -0.600
	// 0x09 => -0.575
	// 0x0A => -0.550
	// 0x0B => -0.525
	// 0x0C => -0.500
	// 0x0D => -0.475
	// 0x0E => -0.450
	// 0x0F => -0.425
	// 0x10 => -0.400
	// 0x11 => -0.375
	// 0x12 => -0.350
	// 0x13 => -0.325
	// 0x14 => -0.300
	// 0x15 => -0.275
	// 0x16 => -0.250
	// 0x17 => -0.225
	// 0x18 => -0.200
	// 0x19 => -0.175
	// 0x1A => -0.150
	// 0x1B => -0.125
	// 0x1C => -0.100
	// 0x1D => -0.075
	// 0x1E => -0.050
	// 0x1F => -0.025
	// 0x20 => +0.000 <<<<<
	// 0x21 => +0.025
	// 0x22 => +0.050
	// 0x23 => +0.075
	// 0x24 => +0.100
	// 0x25 => +0.125
	// 0x26 => +0.150
	// 0x27 => +0.175
	// 0x28 => +0.200
	// 0x29 => +0.225
	// 0x2A => +0.250
	// 0x2B => +0.275
	// 0x2C => +0.300
	// 0x2D => +0.325
	// 0x2E => +0.350
	// 0x2F => +0.375
	// 0x30 => +0.400
	// 0x31 => +0.425
	// 0x32 => +0.450
	// 0x33 => +0.475
	// 0x34 => +0.500
	// 0x35 => +0.525
	// 0x36 => +0.550
	// 0x37 => +0.575
	// 0x38 => +0.600
	// 0x39 => +0.625
	// 0x3A => +0.650
	// 0x3B => +0.675
	// 0x3C => +0.700
	// 0x3D => +0.725
	// 0x3E => +0.750
	// 0x3F => +0.775

	//FRCTRL2 (C6h): Frame Rate Control in Normal Mode
	SPI_sendCommand(ST7789_C6_FRCTR2);
	SPI_sendData(0x0F);
	// 0000 1111
	// ||||-||||--  RTNA[4:0]
	// |||--------  NLA[2:0] : Inversion selection in normal mode
	//                000 = dot inversion
	//                111 = column inversion
	// RTNA[4:0] => FR in normal mode (Hz)
	// 0x00 => 119 Hz
	// 0x01 => 111 Hz
	// 0x02 => 105 Hz
	// 0x03 =>  99 Hz
	// 0x04 =>  94 Hz
	// 0x05 =>  90 Hz
	// 0x06 =>  86 Hz
	// 0x07 =>  82 Hz
	// 0x08 =>  78 Hz
	// 0x09 =>  75 Hz
	// 0x0A =>  72 Hz
	// 0x0B =>  69 Hz
	// 0x0C =>  67 Hz
	// 0x0D =>  64 Hz
	// 0x0E =>  62 Hz
	// 0x0F =>  60 Hz <<<<<
	// 0x10 =>  58 Hz
	// 0x11 =>  57 Hz
	// 0x12 =>  55 Hz
	// 0x13 =>  53 Hz
	// 0x14 =>  52 Hz
	// 0x15 =>  50 Hz
	// 0x16 =>  49 Hz
	// 0x17 =>  48 Hz
	// 0x18 =>  46 Hz
	// 0x19 =>  45 Hz
	// 0x1A =>  44 Hz
	// 0x1B =>  43 Hz
	// 0x1C =>  42 Hz
	// 0x1D =>  41 Hz
	// 0x1E =>  40 Hz
	// 0x1F =>  39 Hz

	// PWCTRL1 (D0h): Power Control 1
	SPI_sendCommand(ST7789_D0_PWCTRL1);
	SPI_sendData(0xA4);   //Fixed vector
	SPI_sendData(0xA1);   //AVDD=6.8V, AVCL=-4.8V, VDS=2.3V
						// DDCC --VV
						// |||| ||||--  VDS[1:0]:
						// ||||           00 = 2.19v
						// ||||           01 = 2.30v <<<<<
						// ||||           10 = 2.40v
						// ||||           11 = 2.51v
						// ||||-------  AVCL[1:0]:
						// ||             00 = -4.4v
						// ||             01 = -4.6v
						// ||             10 = -4.8v <<<<<
						// ||             11 = -5.0v
						// ||---------  AVDD[1:0]:
						//                00 = 6.4v
						//                01 = 6.6v
						//                10 = 6.8v <<<<<
						//                11 = Reserved

	// PVGAMCTRL (E0h): Positive Voltage Gamma Control
	SPI_sendCommand(ST7789_E0_PVGAMCTRL);
	// Pile of magic numbers :-(
	SPI_sendData(0xD0);
	SPI_sendData(0xCA);
	SPI_sendData(0x0E);
	SPI_sendData(0x08);
	SPI_sendData(0x09);
	SPI_sendData(0x07);
	SPI_sendData(0x2D);
	SPI_sendData(0x3B);
	SPI_sendData(0x3D);
	SPI_sendData(0x34);
	SPI_sendData(0x0A);
	SPI_sendData(0x0A);
	SPI_sendData(0x1B);
	SPI_sendData(0x28);


	// NVGAMCTRL (E1h): Negative Voltage Gamma Control
	SPI_sendCommand(ST7789_E1_NVGAMCTRL);

	// Pile of magic numbers :-(
	SPI_sendData(0xD0);
	SPI_sendData(0xCA);
	SPI_sendData(0x0F);
	SPI_sendData(0x08);
	SPI_sendData(0x08);
	SPI_sendData(0x07);
	SPI_sendData(0x2E);
	SPI_sendData(0x5C);
	SPI_sendData(0x40);
	SPI_sendData(0x34);
	SPI_sendData(0x09);
	SPI_sendData(0x0B);
	SPI_sendData(0x1B);
	SPI_sendData(0x28);

	// INVON (21h): Display Inversion On (correct for W)
	SPI_sendCommand(ST7789_21_INVON);

	// CASET (2Ah): Column Address Set
	SPI_sendCommand(ST7789_2A_CASET);
	SPI_sendData(0x00);   //Start MSB Start = 0
	SPI_sendData(0x00);   //Start LSB
	SPI_sendData(0x00);   //End MSB End = 249
	SPI_sendData(0xEF);   //End LSB

	// RASET (2Bh): Row Address Set
	SPI_sendCommand(ST7789_2B_RASET);
	SPI_sendData(0x00);   //Start MSB Start = 0
	SPI_sendData(0x00);   //Start LSB
	SPI_sendData(0x01);   //End MSB End = 319
	SPI_sendData(0x3F);   //End LSB

	// DISPON (29h): Display On
	SPI_sendCommand(ST7789_29_DISPON);
	delayMS(1);

}

void SPI_sendCommand(uint8_t cmd)
{
	// Select the LCD controller

	CLR_CS;
	//Send the command via SPI:
	CLR_RS;
	//SPI.transfer(cmd);
	//uint8_t volatile *spidr = ((__IO uint8_t *)&SPI1->DR);

	while(!(SPI1->SR&SPI_SR_TXE)) 
	;		
	

	*spidr = cmd;
	
	while ((SPI1->SR&SPI_SR_BSY)) ;
	
	// Deselect the LCD controller
	SET_CS;
}
//==============================================================================
void SPI_sendData(uint8_t data)
{
	// Select the LCD controller
	CLR_CS;
	//Send the data via SPI:
	SET_RS;
	//uint8_t volatile *spidr = ((__IO uint8_t *)&SPI1->DR);

	while(!(SPI1->SR&SPI_SR_TXE));		
	
	*spidr = data;
	
	while ((SPI1->SR&SPI_SR_BSY)) ;
	// Deselect the LCD controller
	SET_CS;
}