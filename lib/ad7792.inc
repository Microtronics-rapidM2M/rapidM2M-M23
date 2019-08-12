/*
 *                  _     _  __  __ ___  __  __
 *                 (_)   | ||  \/  |__ \|  \/  |
 *  _ __ __ _ _ __  _  __| || \  / |  ) | \  / |
 * | '__/ _` | '_ \| |/ _` || |\/| | / /| |\/| |
 * | | | (_| | |_) | | (_| || |  | |/ /_| |  | |
 * |_|  \__,_| .__/|_|\__,_||_|  |_|____|_|  |_|
 *           | |
 *           |_|
 *
 * AD7792 interface functions
 * 
 * The AD7792 is a low power, low noise, complete analog front end for high precision measurement applications. 
 * This file provides interface functions for accessing the chip via SPI digital output interface.
 * 
 * Interfaces: 1x SPI
 *             1x GPIO
 *
 * @version 20190806
 * 
 * Revision history:
 * 20190806
 * - Added JSDoc comments
 * - Added additional constants and descriptions
 * - Configuration is handled via AD7792 handle
 * - Refactored INIT, READ and WRITE functions
 */


/* Register masks */
const
{
  AD7792_ADDR_MSK         = 0x00FF,
  AD7792_U8               = 0x0000,
  AD7792_U16              = 0x0200,
  AD7792_TYPE_MSK         = 0x0F00,
}





/* 
COMMUNICATIONS REGISTER 

The number in parentheses indicates the default value of that bit.

CR7    | CR6    | CR5    | CR4    | CR3    | CR2      | CR1  | CR0
-------------------------------------------------------------------
WEN(0) | R/W(0) | RS2(0) | RS1(0) | RS0(0) | CREAD(0) | 0(0) | 0(0) 
*/

/* Communications register bit designations */
const                           // Bit location | Description
{                               // --------------------------
  AD7792_COM_WEN     = (0x80),  // CR7          | Writes enable bit
  AD7792_COM_WRITE   = (0x00),  // CR6          | A 0 at CR6 indicates that the next operation is a write to a specified register
  AD7792_COM_READ    = (0x40),  // CR6          | A 1 at CR6 indicates that the next operation is a write to a specified register
  AD7792_COM_CREAD   = (0x04),  // CR2          | Continuous read of the data register
}

/* Register selection */
const                                        // Register size | RS2 RS1 RS0 | Description
{                                            // -----------------------------------------
  AD7792_REG_COM     = (0x00) | AD7792_U8,   // 8Bit          | 0   0   0   | Communications register during a write operation
  AD7792_REG_STATUS  = (0x00) | AD7792_U8,   // 8Bit          | 0   0   0   | Status register during a read operation
  AD7792_REG_MODE    = (0x08) | AD7792_U16,  // 16Bit         | 0   0   1   | Mode register 
  AD7792_REG_CONFIG  = (0x10) | AD7792_U16,  // 16Bit         | 0   1   0   | Configuration register 
  AD7792_REG_DATA    = (0x18) | AD7792_U16,  // 16Bit         | 0   1   1   | Data register 
  AD7792_REG_ID      = (0x20) | AD7792_U8,   // 8Bit          | 1   0   0   | ID register 
  AD7792_REG_IO      = (0x28) | AD7792_U8,   // 8Bit          | 1   0   1   | IO register
  AD7792_REG_OFFSET  = (0x30) | AD7792_U16,  // 16Bit         | 1   1   0   | Offset register 
  AD7792_REG_FS      = (0x38) | AD7792_U16,  // 16Bit         | 1   1   1   | Full-scale register 
}





/* 
STATUS REGISTER 

The number in parentheses indicates the default value of that bit.

SR7    | SR6    | SR5  | SR4  | SR3 | SR2    | SR1    | SR0
--------------------------------------------------------------
RDY(1) | ERR(0) | 0(0) | 0(0) | 0/1 | CH2(0) | CH1(0) | CH0(0) 
*/

/* Status register bit designations */
const                           // Bit location | Description
{                               // --------------------------
  AD7792_STATUS_NRDY  = (0x80), // SR7          | Ready bit for ADC
  AD7792_STATUS_ERR   = (0x40), // SR6          | ADC error bit
  AD7792_STATUS_CHMSK = (0x07), // SR2 to SR0   | Channel that is being converted by the ADC
}





/*
MODE REGISTER

The number in parentheses indicates the default value of that bit.

MR15   | MR14   | MR13   | MR12 | MR11 | MR10 | MR9  | MR8
-----------------------------------------------------------
MD2(0) | MD1(0) | MD0(0) | 0(0) | 0(0) | 0(0) | 0(0) | 0(0)

MR7     | MR6     | MR5  | MR4  | MR3    | MR2    | MR1    | MR0
-------------------------------------------------------------------
CLK1(0) | CLK0(0) | 0(0) | 0(0) | FS3(1) | FS2(0) | FS1(1) | FS0(0)
*/

/*  Operating modes */
const                                   // MD2 MD1 MD0 | Description
{                                       // -------------------------
  AD7792_MODE_CONT_CONV     = (0x0000), // 0   0   0   | Continuous conversion mode
  AD7792_MODE_SINGLE_CONV   = (0x2000), // 0   0   1   | Single conversion mode
  AD7792_MODE_IDLE          = (0x4000), // 0   1   0   | Idle mode
  AD7792_MODE_POWER_DOWN    = (0x6000), // 0   1   1   | Power-down mode
  AD7792_MODE_INT_ZS_CALIB  = (0x8000), // 1   0   0   | Internal zero-scale calibration
  AD7792_MODE_INT_FS_CALIB  = (0xA000), // 1   0   1   | Internal full-scale calibration
  AD7792_MODE_SYS_ZS_CALIB  = (0xC000), // 1   1   0   | System zero-scale calibration
  AD7792_MODE_SYS_FS_CALIB  = (0xE000), // 1   1   1   | System full-scale calibration
  AD7792_MODE_MSK           = (0xE000), // 1   1   1   | MASK
}

/* Clock source */
const                                   // CLK1 CLK0 | ADC clock source
{                                       // ----------------------------
  AD7792_MODE_INT_CLK       = (0x0000), // 0    0    | Internal 64 kHz clock
  AD7792_MODE_INT_CLK_OUT   = (0x0040), // 0    1    | Internal 64 kHz clock
  AD7792_MODE_EXT_CLK       = (0x0080), // 1    0    | External 64 kHz clock used
  AD7792_MODE_EXT_CLK_2     = (0x00C0), // 1    1    | External clock used
}

/* Filter update rates */
const                                   // FS3 FS2 FS1 FS0 | Rejection @ 50 Hz/60 Hz (internal clock)
{                                       // ----------------------------------------------------------
  AD7792_MODE_FILTER_X      = (0x0000), // 0   0   0   0   | 
  AD7792_MODE_FILTER_4      = (0x0001), // 0   0   0   1   | 
  AD7792_MODE_FILTER_8      = (0x0002), // 0   0   1   0   | 
  AD7792_MODE_FILTER_16     = (0x0003), // 0   0   1   1   | 
  AD7792_MODE_FILTER_32     = (0x0004), // 0   1   0   0   | 
  AD7792_MODE_FILTER_40     = (0x0005), // 0   1   0   1   | 
  AD7792_MODE_FILTER_48     = (0x0006), // 0   1   1   0   | 
  AD7792_MODE_FILTER_60     = (0x0007), // 0   1   1   1   | 
  AD7792_MODE_FILTER_101    = (0x0008), // 1   0   0   0   | 90 dB (60 Hz only)
  AD7792_MODE_FILTER_120_80 = (0x0009), // 1   0   0   1   | 80 dB (50 Hz only) 
  AD7792_MODE_FILTER_120_65 = (0x000A), // 1   0   1   0   | 65 dB (50 Hz and 60 Hz) 
  AD7792_MODE_FILTER_160    = (0x000B), // 1   0   1   1   | 66 dB (50 Hz and 60 Hz) 
  AD7792_MODE_FILTER_200    = (0x000C), // 1   1   0   0   | 69 dB (50 Hz and 60 Hz) 
  AD7792_MODE_FILTER_240    = (0x000D), // 1   1   0   1   | 70 dB (50 Hz and 60 Hz) 
  AD7792_MODE_FILTER_320    = (0x000E), // 1   1   1   0   | 72 dB (50 Hz and 60 Hz) 
  AD7792_MODE_FILTER_480    = (0x000F), // 1   1   1   1   | 74 dB (50 Hz and 60 Hz) 
  AD7792_MODE_FILTER_MSK    = (0x000F), // 1   1   1   1   | 
}





/* 
CONFIGURATION REGISTER 

The number in parentheses indicates the default value of that bit.

CON15     | CON14     | CON13 | CON12  | CON11    | CON10 | CON9  | CON8
-------------------------------------------------------------------------
VBIAS1(0) | VBIAS0(0) | BO(0) | U/B(0) | BOOST(0) | G2(1) | G1(1) | G0(1)

CON7      | CON6 | CON5 | CON4   | CON3 | CON2   | CON1   | CON0
------------------------------------------------------------------
REFSEL(0) | 0(0) | 0(0) | BUF(1) | 0(0) | CH2(0) | CH1(0) | CH0(0)
*/

/* Bias voltage generator */
const                                   // VBIAS1 VBIAS0 | Bias voltage
{                                       // ----------------------------
  AD7792_CFG_NO_BIAS        = (0x0000), // 0      0      | Bias voltage generator disabled 
  AD7792_CFG_BIPOLAR        = (0x0000), // 0      1      | Bias voltage connected to AIN1(-) 
  AD7792_CFG_UNIPOLAR       = (0x1000), // 1      0      | Bias voltage connected to AIN2(-) 
}

/* Gain select bits */
const                                   // G2 G1 G0 | ADC input range (2.5 V reference) 
{                                       // --------------------------------------------
  AD7792_CFG_GAIN_1         = (0x0000), // 0  0  0  | 2.5 V
  AD7792_CFG_GAIN_2         = (0x0100), // 0  0  1  | 1.25 V
  AD7792_CFG_GAIN_4         = (0x0200), // 0  1  0  | 625 mV
  AD7792_CFG_GAIN_8         = (0x0300), // 0  1  1  | 312.5 mV
  AD7792_CFG_GAIN_16        = (0x0400), // 1  0  0  | 156.2 mV 
  AD7792_CFG_GAIN_32        = (0x0500), // 1  0  1  | 78.125 mV
  AD7792_CFG_GAIN_64        = (0x0600), // 1  1  0  | 39.06 mV 
  AD7792_CFG_GAIN_128       = (0x0700), // 1  1  1  | 19.53 mV
}

/* Reference select bit */
const                                   // REFSEL | Reference source 
{                                       // -------------------------
  AD7792_CFG_EXTREF         = (0x0000), // 0      | External reference applied between REFIN(+) and REFIN(–)
  AD7792_CFG_INTREF         = (0x0080), // 1      | Internal reference selected
}

const
{
  AD7792_CFG_BUF            = (0x0010), // Configures the ADC for buffered mode of operation (BUF == 1)
}

/**
* Structure for AD7792 interface configuration
*
* cs:s32      - CS port used for communication
* spi:s32     - SPI port used for communication
*/
#define TAD7792_Handle[.cs, .spi]


/**
 * Initialisation of SPI communication with AD7792
 * 
 * This function copies the CS port and SPI interface to the given structure. 
 * The AD7792 is reset by writing 32-Bits consecutive 1s to the SPI interface. The ID and 
 * configuration register are checked to ensure that the AD7792 was initialised correctly.
 * 
 * @param handle:TAD7792_Handle     - Structure that contains AD7792 interface configuration
 * @param cs:s32                    - SPI CS port
 * @param spi:s32      	            - Number of SPI interface
 * @return s32                 	    - OK:    If successful
 *							   		              - ERROR: SPI communication error or not AD7792
 */
stock AD7792_Init(handle[TAD7792_Handle], cs, spi)
{
  new iResult;                              // Temporary memory for the return value of a function
  new Data{4};
  new iReadData;

  handle.cs = cs;
  handle.spi = spi;
  
  Data{0} = Data{1} = Data{2} = Data{3} = 0xFF;

  /* Inits CS for AD7792 */
  rM2M_GpioDir(handle.cs, RM2M_GPIO_OUTPUT);
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);

  /* Sets CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_LOW);

  /* Resets AD7792 by writing 32Bits '1' */
  iResult = rM2M_SpiCom(handle.spi, Data, 4, 0);

  /* Clears CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);

  if(iResult < OK) return(iResult);

  /* Checks ID register */
  AD7792_Read(handle, AD7792_REG_ID, iReadData);
  if((iReadData & 0xF) != 0xA)
  {
    /* Not AD7792 or SPI communication error */
    return(ERROR);
  }

  /* Checks reset value of configuration register */
  AD7792_Read(handle, AD7792_REG_CONFIG, iReadData);
  if(iReadData != 0x0710)
  {
    /* SPI communication error */
    return(ERROR);
  }
  return(OK);
}


/**
 * Reads register of AD7792 and returns unpacked data
 * 
 * Use this function to read unsigned 8-Bit or unsigned 16-Bit registers
 * of the AD7792. The read data is unpacked according to the data type in the register information
 * variable.
 * 
 * @param handle:TAD7792_Handle     - Structure that contains AD7792 interface configuration
 * @param iAddress:s32              - AD7792 register address
 * @param iData:s32      	          - Unpacked data read from the register
 * @return s32                 	    - OK:    If successful
 *							   		              - ERROR: SPI communication error
 */
stock AD7792_Read(handle[TAD7792_Handle], iAddress, &iData)
{
  new aRaw{3};
  new iSize, iDataType;
  new iAddr, iType;
  new iResult;

  iAddr = iAddress & AD7792_ADDR_MSK;
  iType = iAddress & AD7792_TYPE_MSK;

  switch(iType)
  {
    case AD7792_U8:
    {
      iSize     = 1;
      iDataType = RM2M_PACK_U8;
    }
    case AD7792_U16:
    {
      iSize     = 2;
      iDataType = RM2M_PACK_U16;
    }
    default:
      return(0);
  }

  aRaw{0} = AD7792_COM_READ | iAddr;

  /* Sets CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_LOW);

  /* Sets address and reads register */
  iResult = rM2M_SpiCom(handle.spi, aRaw, 1, iSize);

  /* Clears CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);

  /* Unpacks data depending on the register data type */
  rM2M_Pack(aRaw, 0, iData, iDataType + RM2M_PACK_GET + RM2M_PACK_BE);

  return(iResult);
}


/**
 * Writes register of AD7792 with packed data
 * 
 * Use this function to write unsigned 8-Bit or unsigned 16-Bit registers of the AD7792.
 * The data to be written is packed according to the data type in the register information
 * variable and sent to the AD7792.
 * 
 * @param handle:TAD7792_Handle     - Structure that contains AD7792 interface configuration
 * @param iAddress:s32              - AD7792 register address
 * @param iValue:s32      	        - Data to be packed and written to the specified register
 * @return s32                 	    - OK:    If successful
 *							   		              - ERROR: SPI communication error
 */
stock AD7792_Write(handle[TAD7792_Handle], iAddress, iValue)
{
  new aData{3};
  new iSize, iDataType;
  new iAddr, iType;
  new iResult;

  iAddr = iAddress & AD7792_ADDR_MSK;
  iType = iAddress & AD7792_TYPE_MSK;

  switch(iType)
  {
    case AD7792_U8:
    {
      iSize     = 1;
      iDataType = RM2M_PACK_U8;
    }
    case AD7792_U16:
    {
      iSize     = 2;
      iDataType = RM2M_PACK_U16;
    }
    default:
      return(0);
  }

  aData{0} = AD7792_COM_WRITE | iAddr;
  rM2M_Pack(aData, 1, iValue, iType + RM2M_PACK_BE);

  /* Sets CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_LOW);

  /* Sets address and writes to register */
  iResult = rM2M_SpiCom(handle.spi, aData, iSize + 1, 0);

  /* Clears CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);

  return(iResult)
}
