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
 * MAX44000 interface functions
 * 
 * The MAX44000 contains a wide-dynamic range ambient light sensor with integrated infrared
 * proximity sensor. The chip supports low-power operation and has an programmable
 * single-pulse IR LED Driver integrated. It can be accessed via I2C interface.
 * 
 * Interfaces: 1x I2C
 *
 * @version 20190801
 * 
 * revision history:
 * 20190801
 * - Added additional returns
 * - Added additional constants
 * - Added overflow check
 * - Added register masks for datatype and read or write only checks
 * - Refactored READ and WRITE register functions
 * 20190718
 * - Added JSDoc comments
 * - Data is handed over via MAX44000 I2C handle
 */

/* Available MAX44000 I2C addresses */
const MAX44000_I2C_ADR = 0x94

/* Register Masks */
const
{
  MAX44000_ADDR_MSK         = 0x00FF,
  MAX44000_U8               = 0x0000,
  MAX44000_U16              = 0x0200,
  MAX44000_TYPE_MSK         = 0x0F00,
  MAX44000_RO               = 0x1000,
  MAX44000_RW_MSK           = 0xF000,
}

/* MAX44000 Register Addresses */
const                                                                    // R/W  | Reset State | Description
{                                                                        // --------------------------------
  MAX44000_STATUS_REG           = 0x00 | MAX44000_U8 | MAX44000_RO,      // R    | 0x04        | Interrupt Status
  MAX44000_CONFIG_REG           = 0x01 | MAX44000_U8,                    // R/W  | 0x24        | Main Configuration
  MAX44000_RXCONFIG_REG         = 0x02 | MAX44000_U8,                    // R/W  | 0x00        | Receive Configuration
  MAX44000_TXCONFIG_REG         = 0x03 | MAX44000_U8,                    // R/W  | 0x00        | Transmit Configuration
  MAX44000_ALS_REG              = 0x04 | MAX44000_U16 | MAX44000_RO,     // R    | 0x00        | ALS Register
  MAX44000_ALS_REG_H            = 0x04                | MAX44000_RO,     // R    | 0x00        | ALS Register, HIGH Byte
  MAX44000_ALS_REG_L            = 0x05                | MAX44000_RO,     // R    | 0x00        | ALS Register, LOW Byte
  MAX44000_ALS_UPTHR_REG        = 0x06 | MAX44000_U16,                   // R/W  | 0x00        | ALS  Upper Threshold
  MAX44000_ALS_UPTHR_REG_H      = 0x06,                                  // R/W  | 0x00        | ALS  Upper Threshold, HIGH Byte
  MAX44000_ALS_UPTHR_REG_L      = 0x07,                                  // R/W  | 0x00        | ALS  Upper Threshold, LOW Byte
  MAX44000_ALS_LOTHR_REG        = 0x08 | MAX44000_U16,                   // R/W  | 0x00        | ALS  Lower Threshold
  MAX44000_ALS_LOTHR_REG_H      = 0x08,                                  // R/W  | 0x00        | ALS  Lower Threshold, HIGH Byte
  MAX44000_ALS_LOTHR_REG_L      = 0x09,                                  // R/W  | 0x00        | ALS  Lower Threshold, LOW Byte
  MAX44000_THR_TIMER_REG        = 0x0A | MAX44000_U8,                    // R/W  | 0x00        | Threshold Persist Timer
  MAX44000_PROX_THR_REG         = 0x0B | MAX44000_U16,                   // R/W  | 0x00        | PROX Threshold Registers
  MAX44000_PROX_THR_REG_I       = 0x0B,                                  // R/W  | 0x00        | PROX Threshold Indicator
  MAX44000_PROX_THR_REG_T       = 0x0C,                                  // R/W  | 0x00        | PROX Threshold
  MAX44000_TRIM_REG             = 0x0F | MAX44000_U16,                   // R/TW | 0x80        | Digital Gain Trim Registers
  MAX44000_TRIM_REG_GREEN       = 0x0F,                                  // R/TW | 0x80        | Digital Gain Trim of Green Channel
  MAX44000_TRIM_REG_INFRARED    = 0x10,                                  // R/TW | 0x80        | Digital Gain Trim of Infrared Channel
  MAX44000_PROX_REG             = 0x15 | MAX44000_U16 | MAX44000_RO,     // R    | 0x00        | PROX Data Register
}

/* LED Drive Current Settings */
const
{
  MAX44000_LED_000MA     = 0x00,  // LED driver disabled
  MAX44000_LED_010MA     = 0x01,
  MAX44000_LED_020MA     = 0x02,
  MAX44000_LED_030MA     = 0x03,
  MAX44000_LED_040MA     = 0x04,
  MAX44000_LED_040MA     = 0x04,
  MAX44000_LED_050MA     = 0x05,
  MAX44000_LED_060MA     = 0x06,
  MAX44000_LED_070MA     = 0x07,
  MAX44000_LED_080MA     = 0x0C,
  MAX44000_LED_090MA     = 0x0D,
  MAX44000_LED_100MA     = 0x0E,
  MAX44000_LED_110MA     = 0x0F,
}  

/* Ambient ADC Conversion Time */
const
{
  MAX44000_ADC_100MS     = 0x00 << 2,   // 100ms      14bit
  MAX44000_ADC_025MS     = 0x01 << 2,   // 25ms       12bit
  MAX44000_ADC_006MS     = 0x02 << 2,   // 6.25ms     10bit
  MAX44000_ADC_001MS     = 0x03 << 2,   // 1.5625ms   8bit
}

/* Ambient Light Measurement Gain */
const
{
  MAX44000_ALS_GAIN_01   = 0x00,        // 0.03125 LUX/LSB
  MAX44000_ALS_GAIN_04   = 0x01,        // 0.125   LUX/LSB
  MAX44000_ALS_GAIN_16   = 0x02,        // 0.5     LUX/LSB
  MAX44000_ALS_GAIN_64   = 0x03,        // 4       LUX/LSB
}


/**
* Structure for MAX44000 interface configuration
*
* i2c:s32      - I2C Port used for communication
* addr:s32     - Address of the I2C device
* state:s32    - Reserved
*/
#define TMAX44000_Handle[.i2c, .addr, .state]

/**
 * Initialisation of I2C communication with MAX44000
 *
 * This function copies the I2C port and address to the given structure and initialises the
 * common MAX44000 registers to allow usage of ALS and proximity sensor.
 * 
 * @param handle:TMAX44000_Handle - Structure that contains MAX44000 interface configuration
 * @param i2c:s32                 - I2C port used for communication
 * @param addr:s32                - Address of the I2C device
 * @return s32                    - OK:  If successful
 *                                - <OK: An ERROR occurred. See the manual.
 */
stock MAX44000_Init(handle[TMAX44000_Handle], i2c, addr)
{
  /* Temporary memory */
  new iStatus;
  
  handle.i2c = i2c;
  handle.addr = addr;
  
  /* Proximity (14 Bit, 100ms, gain = 1)
     ALS (100ms, 0.5lux/LSB) */
  if(MAX44000_Write(handle, MAX44000_RXCONFIG_REG, 0x32) < OK) return (ERROR);
  
  /* Sets the LED drive current to 110mA */
  if(MAX44000_Write(handle, MAX44000_TXCONFIG_REG, 0x0F) < OK) return (ERROR);
  
  /* Sets the ALS interrupt window limits */
  if(MAX44000_Write(handle, MAX44000_ALS_UPTHR_REG, 0x0000) < OK) return (ERROR);
  if(MAX44000_Write(handle, MAX44000_ALS_LOTHR_REG, 0x3FFF) < OK) return (ERROR);
  
  /* Enables proximity interrupt */
  if(MAX44000_Write(handle, MAX44000_PROX_THR_REG, 0x4000) < OK) return (ERROR);

  /* Reads status register (clears INT) */
  if(MAX44000_Read(handle, MAX44000_STATUS_REG, iStatus) < OK) return (ERROR);

  /* Uses factory-programmed trim value
     ALS & proximity are interleaved continuously */
  if(MAX44000_Write(handle, MAX44000_CONFIG_REG, 0x30) < OK) return (ERROR);

  /* Returns OK if successful */
  return (OK);
}

/**
 * Writes data to the MAX44000
 * 
 * This function checks the given register and sends 1 to 2 bytes of data to the MAX44000.
 * The register address is incremented when writing two bytes of data. The I2C communication
 * result is returned.
 * 
 * @param handle:TMAX44000_Handle - Structure that contains MAX44000 interface configuration
 * @param iReg:s32                - MAX44000 register address
 * @param iValue:s32              - Data to be written to the MAX44000
 * @return s32                    - OK:  If successful
 *                                - ERROR: Register address not valid
 *                                - <OK: An ERROR occurred. See the manual.
 */
stock MAX44000_Write(handle[TMAX44000_Handle], iRegInfo, iValue)
{
  /* Temporary memory */
  new aData{2};
  new iSize;
  new iAddr, iType, iAccess;

  /* Gets address and data type */
  iAddr   = iRegInfo & MAX44000_ADDR_MSK;
  iType   = iRegInfo & MAX44000_TYPE_MSK;
  iAccess = iRegInfo & MAX44000_RW_MSK;

  // If the register is read only -> Returns "ERROR"
  if(iAccess == MAX44000_RO)
    return (ERROR);

  /* Sets number of bytes to be read depending on the data type */
  switch(iType)
  {
    case MAX44000_U8:
      iSize = 1;
    case MAX44000_U16:
      iSize = 2;
    default:
      return (ERROR);
  }

  /* Checks size of data to be written */
  if(iSize >= 2)
  {
    /* Sets register address and increment afterwards */
    aData{0} = iAddr++;
    /* Sets first byte of data */
    aData{1} = iValue >> 8;
    /* Writes data & return result if failed */
    if(rM2M_I2cCom(0, handle.addr, aData, 2, 0) < OK) return(ERROR);
  }
  if(iSize >= 1)
  {
    /* Sets (next) register address */
    aData{0} = iAddr;
    /* Sets data */
    aData{1} = iValue;
    /* Writes data & returns result if failed */
    if(rM2M_I2cCom(0, handle.addr, aData, 2, 0) < OK) return(ERROR);
  }
  /* Returns OK if successfull */
  return(OK);
}

/**
 * Reads data from the MAX44000
 * 
 * This function checks the given register and reads 1 to 2 bytes of data from the MAX44000.
 * The register address is incremented when reading two bytes of data. The I2C communication
 * result and the data to be read are returned.
 * 
 * @param handle:TMAX44000_Handle - Structure that contains MAX44000 interface configuration
 * @param iReg:s32                - MAX44000 register address
 * @param iValue:s32              - Data to be read from the MAX44000
 * @return s32                    - OK:    If successful
 *                                - ERROR: Register address not valid
 *                                - <OK:   An ERROR occurred. See the manual.
 */
stock MAX44000_Read(handle[TMAX44000_Handle], iRegInfo, &iValue)
{
  /* Temporary memory */
  new aData{1};
  new iSize;
  new iAddr, iType;
  new iReadData;

  /* Gets address and data type */
  iAddr   = iRegInfo & MAX44000_ADDR_MSK;
  iType   = iRegInfo & MAX44000_TYPE_MSK;

  /* Checks register size */
  switch(iType)
  {
    case MAX44000_U8:
      iSize = 1;
    case MAX44000_U16:
      iSize = 2;
    default:
      return (ERROR);
  }

  /* Reads data */
  if(iSize >= 2)
  {
    /* Sets register address and increments afterwards */
    aData{0} = iAddr++;
    /* Reads data & returns result if failed */
    if(rM2M_I2cCom(0, handle.addr, aData, 1, 1) < OK) return(ERROR);
    /* Copies the first byte of the read data to the s32 cell variable */
    iReadData |= aData{0} << 8;
  }
  if(iSize >= 1)
  {
    /* Sets (next) register address */
    aData{0} = iAddr;
    /* Reads data & returns result if failed */
    if(rM2M_I2cCom(0, handle.addr, aData, 1, 1) < OK) return(ERROR);

    /* Copies the byte of the read data to the s32 cell variable */
    iReadData |= aData{0};
  }
  /* Returns data */
  iValue = iReadData;
  /* Returns OK if successful */
  return(OK);
}
