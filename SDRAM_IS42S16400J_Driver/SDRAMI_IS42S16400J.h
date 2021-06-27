#pragma once
//-------------------| SDRAM DEFINES |---------------------------------------------
//MODE register definitions
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200)  

#define EXTERNAL_SDRAM_SIZE				0x400000
#define EXTERNAL_SDRAM_BANK_ADDR		((uint32_t)0xD0000000)
#define EXTERNAL_SDRAM_CAS_LATENCY		FMC_SDRAM_CAS_LATENCY_3
#define SDCLOCK_PERIOD					FMC_SDRAM_CLOCK_PERIOD_2 
#define EXTERNAL_SDRAM_READBURST		FMC_SDRAM_RBURST_DISABLE 
#define external_start_addr       *(__IO uint16_t*)(EXTERNAL_SDRAM_BANK_ADDR )
