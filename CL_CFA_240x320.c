#include "CL_CFA_240x320.h"

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



