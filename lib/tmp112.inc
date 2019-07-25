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
 * TMP112 interface functions
 * 
 * The TMP112 is a high-accuracy digital temperature sensor. It is highly linear and provides
 * easy temperature calculation. The on-chip 12-bit ADC offers resolutions down to 0.0625°C.
 * 
 * Revision history:
 * 20190723
 * - Edited configuration register initialisation value in TMP112_Init()
 * - Added data packing to TMP112_Write() and TMP112_Read()
 * - Added TMP112_GetTemp()
 * 20190715
 * - Added JSDoc comments
 * - Data is handed over via TMP112 I2C handle
 */

/* TMP112 addresses determined by the ADD0 pin. The ADD0 pin can be connected to either
   GND, V+, SDA or SCL pin.  */
const
{
  TMP112_I2C_ADR1 = (0x90),
  TMP112_I2C_ADR2 = (0x92),
  TMP112_I2C_ADR3 = (0x94),
  TMP112_I2C_ADR4 = (0x96),
}

/* Pointer addresses of the registers available in the TMP112 family. */
const
{
  TMP112_TEMP_REG      = (0x00),
  TMP112_CONFIG_REG    = (0x01),
  TMP112_TEMP_LOW_REG  = (0x02),
  TMP112_TEMP_HIGH_REG = (0x03),
}

/**
* Structure for TMP112 interface configuration
*
* i2c:s32      - I2C port used for communication
* addr:s32     - Address of the I2C device
* state:s32    - Reserved
*/
#define TTMP112_Handle[.i2c, .addr, .state]

/**
 * Initialisation of the TMP112
 * 
 * This function copies configuration data into the TMP112 handler and writes the
 * configuration Register of the TMP112.
 * 
 * @param handle:TTMP112_Handle - Structure that contains TMP112 interface configuration
 * @param i2c:s32               - I2C Port used for communication 
 * @param addr:s32              - Address of the I2C device
 * @return s32                 	- OK:    If successful
 *							   	            - <OK:   An ERROR occurred. See the manual.
 */
stock TMP112_Init(handle[TTMP112_Handle], i2c, addr)
{
  /* Initialises TMP112 I2C handle */
  handle.i2c = i2c;
  handle.addr = addr;
  
  /* Initialises TMP112 with 4 Hz conversion rate (typ. conversion time = 26ms)*/
  return(TMP112_Write(handle, TMP112_CONFIG_REG, 0x6080));
}

/**
 * Writes a value to a TMP112 register
 * 
 * This function checks the given register and converts the value to be written
 * into an array of bytes. The first byte after slave address contains pointer address
 * information, followed by the actual data to be written. See the write word format in the
 * datasheet for the timing diagrams.
 * 
 * @param handle:TTMP112_Handle - Structure that contains TMP112 interface configuration
 * @param Reg:s32               - TMP112 register (Pointer Address)
 * @param iValue:s32            - Value to be written to the TMP112 register
 * @return s32                 	- OK:    If successful
 *							   	            - <OK:   An ERROR occurred. See the manual.
 */
stock TMP112_Write(handle[TTMP112_Handle], Reg, iValue)
{
  /* Temporary memory */
  new Data{3};
  new iLen;
  
  /* Sets data */
  switch(Reg)
  {
    case TMP112_TEMP_REG:
      return(ERROR);
    case TMP112_CONFIG_REG:
    {
      rM2M_Pack(Data, 1, iValue, RM2M_PACK_U16 + RM2M_PACK_BE);
      iLen = 3;
    }
    case TMP112_TEMP_LOW_REG:
    {
      rM2M_Pack(Data, 1, iValue, RM2M_PACK_S16 + RM2M_PACK_BE);
      iLen = 3;
    }
    case TMP112_TEMP_HIGH_REG:
    {
      rM2M_Pack(Data, 1, iValue, RM2M_PACK_S16 + RM2M_PACK_BE);
      iLen = 3;
    }
    default:
      return(ERROR);
  }
  
  /* Sets register pointer */
  Data{0} = Reg;
  
  /* Writes data & returns result */
  return(rM2M_I2cCom(handle.i2c, handle.addr, Data, iLen, 0));
}

/**
 * Reads a value from a TMP112 register
 * 
 * This function checks the given register and converts the byte array to be read
 * into a s32 cell. The first byte after slave address containing pointer address
 * information is sent to the TMP112. After that, the TMP112 responds with the requested data
 * frames after the second slave address byte. See the read word format in the
 * datasheet for the timing diagrams.
 * 
 * @param handle:TTMP112_Handle - Structure that contains TMP112 interface configuration
 * @param Reg:s32               - TMP112 register (Pointer Address)
 * @param iValue:s32            - Value to be read from the TMP112 register
 * @return s32                 	- OK:    If successful
 *							   	            - <OK:   An ERROR occurred. See the manual.
 */
stock TMP112_Read(handle[TTMP112_Handle], Reg, &iValue)
{
  /* Temporary memory */
  new Data{2};
  new Size;

  /* Checks register size */
  switch(Reg)
  {
    case TMP112_CONFIG_REG, TMP112_TEMP_REG, TMP112_TEMP_LOW_REG, TMP112_TEMP_HIGH_REG:
      Size = 2;
    default:
      return(ERROR);
  }

  /* Sets register pointer & reads data */
  Data{0} = Reg;
  if(rM2M_I2cCom(handle.i2c, handle.addr, Data, 1, Size) < OK) return(ERROR);

  switch(Reg)
  {
    case TMP112_TEMP_REG:
      rM2M_Pack(Data, 0, iValue, RM2M_PACK_S16 + RM2M_PACK_GET + RM2M_PACK_BE);
    case TMP112_CONFIG_REG:
      rM2M_Pack(Data, 0, iValue, RM2M_PACK_U16 + RM2M_PACK_GET + RM2M_PACK_BE);
    case TMP112_TEMP_LOW_REG:
      rM2M_Pack(Data, 0, iValue, RM2M_PACK_S16 + RM2M_PACK_GET + RM2M_PACK_BE);
    case TMP112_TEMP_HIGH_REG:
      rM2M_Pack(Data, 0, iValue, RM2M_PACK_S16 + RM2M_PACK_GET + RM2M_PACK_BE);
    default:
      return(ERROR);
  }
  
  return(OK);
}

/**
 * Gets TMP112 temperature
 * 
 * This function reads the TMP112 temperature register and converts the data. The
 * measurement is returned in [0.01°C].
 * 
 * @param handle:TTMP112_Handle - Structure that contains TMP112 interface configuration
 * @param iTemp:s32             - Temperature in [0.01°C]
 * @return s32                 	- OK:    If successful
 *							   	            - <OK:   An ERROR occurred. See the manual.
 */
stock TMP112_GetTemp(handle[TTMP112_Handle], &iTemp)
{
  /* Temporary memory for the return value of a function */
  new iResult;
  
  /* Reads TMP112 temperature register */
  iResult = TMP112_Read(handle, TMP112_TEMP_REG, iTemp);
  
  /* The data in the TMP112 temperature register is left-justified. By dividing by 16, the lower
     4 bits are cut off. The resolution of 12-bit results in 0.0625°C/LSB. This multiplication 
     factor equates to a division by 16. So all things considered, the measurement value has to be divided by
     256. The returned value is an integer value [0.01°C]. */
  iTemp = 100 * iTemp / 256;
  
  /* Returns result */
  return iResult;
}
