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
 * SHT21 interface functions
 * 
 * The SHT21 is a fully calibrated temperature and humidity sensor with accuracy of
 * +/-2%RH and +/-0.3°C. It has a digital output and can be accessed via I2C interface.
 * Furthermore, the device has low power consumption and long term stability.
 * 
 * Interfaces: 1x I2C
 *
 * @version 20190802
 * 
 * revision history:
 * 20190802
 * - Added SHT21_GetSerialNumber() function
 * 20190730
 * - Added CRC Check
 * 20190716
 * - Added JSDoc comments
 * - Data is handed over via SHT21 I2C handle
 */

#define SHT21_DEBUG

/* Available SHT21 I2C addresses */
#define SHT21_I2C_ADR1 (0x80)


const
{ // Error Flags
  SHT21_RH_ERROR_FLAG             = 0x01
  SHT21_TEMP_ERROR_FLAG           = 0x02
  SHT21_PLAUSIBILITY_ERROR_FLAG   = 0x04
}

const
{ // Commands
  SHT2x_TRIG_T_MEASUREMENT_HM     = 0xE3,   // Command trig. temp meas. hold master
  SHT2x_TRIG_RH_MEASUREMENT_HM    = 0xE5,   // Command trig. humidity meas. hold master
  SHT2x_TRIG_T_MEASUREMENT_POLL   = 0xF3,   // Command trig. temp meas. no hold master
  SHT2x_TRIG_RH_MEASUREMENT_POLL  = 0xF5,   // Command trig. humidity meas. no hold master
  SHT2x_USER_REG_W                = 0xE6,   // Command writing user register
  SHT2x_USER_REG_R                = 0xE7,   // Command reading user register
  SHT2x_SOFT_RESET                = 0xFE,   // Command soft reset
}

const
{ // SHT21 Resolution
  SHT2x_RES_12_14BIT  = 0x00,               // RH=12bit, T=14bit
  SHT2x_RES_8_12BIT   = 0x01,               // RH= 8bit, T=12bit
  SHT2x_RES_10_13BIT  = 0x80,               // RH=10bit, T=13bit
  SHT2x_RES_11_11BIT  = 0x81,               // RH=11bit, T=11bit
  SHT2x_RES_MASK      = 0x81,               // Mask for res. bits (7,0) in user reg.
}

const
{ // End of Battery
  SHT2x_EOB_ON    = 0x40,                   // End of battery
  SHT2x_EOB_MASK  = 0x40,                   // Mask for EOB bit(6) in user reg.
}

const
{ // Functionality Diagnosis
  SHT2x_HEATER_ON   = 0x04,                 // Heater on
  SHT2x_HEATER_OFF  = 0x00,                 // Heater off
  SHT2x_HEATER_MASK = 0x04,                 // Mask for Heater bit(2) in user reg.
}

const
{ // Possible states of the measurement process
  STATE_NONE = 0,
  STATE_RESET,
  STATE_CONFIG,
  STATE_TRIGGER_TEMPERATURE,
  STATE_READ_TEMPERATURE,
  STATE_TRIGGER_HUMIDITY,
  STATE_READ_HUMIDITY,
}

const
{ // Measurement data available indicators
  SHT21_TEMPERATURE = 0x01,
  SHT21_HUMIDITY    = 0x02,
}

/**
* Structure for SHT21 interface configuration
*
* i2c:s32      - I2C port used for communication
* addr:s32     - Address of the I2C device
* state:s32    - Current measurement state
* temp:s32     - Temperature value [0.01 °C]
* hum:s32      - Humidity value [0.1 %rH]
* valstate:s32 - Measurement data available indicator
*/
#define TSHT21_Handle[.i2c, .addr, .state, .temp , .hum, .valstate]

/**
 * Initialisation of I2C communication with SHT21
 *
 * This function copies the I2C port and address to the given structure and resets the state
 * machine.
 * 
 * @param handle:TSHT21_Handle - Structure that contains SHT21 interface configuration
 * @param i2c:s32              - I2C port used for communication
 * @param addr:s32             - Address of the I2C device
 * @return s32                 - OK: If successful
 */
stock SHT21_Init(handle[TSHT21_Handle], i2c, addr)
{
  /* Currently only 1 device supported */
  handle.i2c = i2c;
  handle.addr = addr;
  
  /* Switches to reset state */
  handle.state = STATE_RESET;
  
  return(OK);
}

/**
 * SHT21 state handling
 * 
 * This function checks the current state of the SHT21 interface and switches to the respective
 * communication step. The SHT21 must be initialised before calling this function. When this is
 * done, a soft reset is performed firstly, followed by the configuration (10/13 bit). After that,
 * a measurement command is sent to the sensor indicating "No Hold master mode", so the master
 * needs to poll for the termination of the internal sensor processing. Please refer to the
 * SHT21 datasheet about the "No Hold master communication sequence" for further information.
 * 
 * @param handle:TSHT21_Handle - Structure that contains SHT21 interface configuration
 */
stock SHT21_HandleTimer(handle[TSHT21_Handle])
{
  /* Temporary memory for read data and the return value of a function */
  new iValue;
  new iResult;

  switch(handle.state)
  {
    case STATE_NONE:
    {
      /* SHT21 not initialised */
    }
    case STATE_RESET:
    {
      /* Performs a software reset and wait until finished */
      iResult = SHT21_SoftReset(handle);
      if(iResult < OK)
      {
    #if defined SHT21_DEBUG
        printf("[SHT21] SHT21_SoftReset: %d\r\n", iResult);
    #endif
      }
      else
      {
        /* Switches to configuration state */
        handle.state = STATE_CONFIG;
      }
    }
    case STATE_CONFIG:
    {
      /* Reads configuration data from SHT21 */
      iResult = SHT21_ReadConfig(handle, iValue)

      if(iResult < OK)
      {
    #if defined SHT21_DEBUG
        printf("[SHT21] SHT21_ReadConfig: %d\r\n", iResult);
    #endif
      }
      else
      {
        /* Sets the temperature and humidity resolution */
        iValue = (iValue & SHT2x_RES_MASK) | SHT2x_RES_10_13BIT;
        
        /* Sends configuration to the SHT21 */
        iResult = SHT21_WriteConfig(handle, iValue)
        if(iResult < OK)
        {
      #if defined SHT21_DEBUG
          printf("[SHT21] SHT21_WriteConfig: %d\r\n", iResult);
      #endif
        }
        else
        {
          /* Switches to trigger temperature state */
          handle.state = STATE_TRIGGER_TEMPERATURE;
        }
      }
    }
    case STATE_TRIGGER_TEMPERATURE:
    {
      /* Informs the SHT21 that No Hold master mode (temperature) should be used */
      iResult = SHT21_TriggerMeasurement(handle, SHT2x_TRIG_T_MEASUREMENT_POLL);
      
      if(iResult < OK)
      {
    #if defined SHT21_DEBUG
        printf("[SHT21] SHT21_TriggerMeasurement: %d\r\n", iResult);
    #endif
      }
      else
      {
        /* Switches to read temperature state */
        handle.state = STATE_READ_TEMPERATURE;
      }
    }
    case STATE_READ_TEMPERATURE:
    {
      /* Reads temperature measurement from SHT21 */
      iResult = SHT21_ReadMeasurement(handle, iValue);
      
      if(iResult < OK)
      {
    #if defined SHT21_DEBUG
        printf("[SHT21] SHT21_TriggerMeasurement: %d\r\n", iResult);
    #endif
        /* Temperature not available */
        handle.valstate &= ~(SHT21_TEMPERATURE);
      }
      else
      {
        /* Clears bits [1..0] (status bits) */
        iValue &= ~0x0003;
        
        /* Calculates temperature [°C] */
        handle.temp = -4685 + 17572 * iValue / 65536; //T= -46.85 + 175.72 * ST/2^16
        
        /* Temperature available */
        handle.valstate |= SHT21_TEMPERATURE;
      }
      /* Switches to trigger humidity state */
      handle.state = STATE_TRIGGER_HUMIDITY;
    }
    case STATE_TRIGGER_HUMIDITY:
    {
      /* Informs the SHT21 that No Hold master mode (humidity) should be used */
      iResult = SHT21_TriggerMeasurement(handle, SHT2x_TRIG_RH_MEASUREMENT_POLL);
      
      if(iResult < OK)
      {
    #if defined SHT21_DEBUG
        printf("[SHT21] SHT21_TriggerMeasurement: %d\r\n", iResult);
    #endif
      }
      else
      {
        /* Switches to read humidity state */
        handle.state = STATE_READ_HUMIDITY;
      }
    }
    case STATE_READ_HUMIDITY:
    {
      /* Reads humidity measurement from SHT21 */
      iResult = SHT21_ReadMeasurement(handle, iValue);
      
      if(iResult < OK)
      {
    #if defined SHT21_DEBUG
        printf("[SHT21] SHT21_TriggerMeasurement: %d\r\n", iResult);
    #endif
        /* Humidity not available */
        handle.valstate &= ~(SHT21_HUMIDITY);
      }
      else
      {
        /* Clears bits [1..0] (status bits) */
        iValue &= ~0x0003;

        /* Calculates relative humidity [%RH] */
        handle.hum = -60 + 1250 * iValue / 65536; // RH= -6 + 125 * SRH/2^16
        
        /* Humidity available */
        handle.valstate |= SHT21_HUMIDITY;
      }
      /* Switches to trigger temperature state */
      handle.state = STATE_TRIGGER_TEMPERATURE;
    }
  }
}

/**
 * Performs a soft reset on the SHT21
 *
 * The function sends one byte of data containing the Soft reset command to the sensor.
 * 
 * @param handle:TSHT21_Handle - Structure that contains SHT21 interface configuration
 * @return s32                 - OK:  If successful
 *                             - <OK: An ERROR occurred. See the manual.
 */
stock SHT21_SoftReset(handle[TSHT21_Handle])
{
  /* Temporary memory for the data and the return value of a function */
  new aData{1};
  new iResult;
  
  /* Sets SHT21 command */
  aData{0} = SHT2x_SOFT_RESET;
  
  /* Writes data and returns result */
  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 1, 0);
  
  return iResult;
}

/**
 * Writes configuration data to the SHT21
 *
 * The function sends two bytes of data containing the Write user register command and the
 * configuration data to the sensor.
 * 
 * @param handle:TSHT21_Handle - Structure that contains SHT21 interface configuration
 * @param iValue:s32           - Configuration to be written to the SHT21
 * @return s32                 - OK:  If successful
 *                             - <OK: An ERROR occurred. See the manual.
 */
stock SHT21_WriteConfig(handle[TSHT21_Handle], iValue)
{
  /* Temporary memory for the data and the return value of a function */
  new aData{2};
  new iResult;
  
  /* Sets SHT21 command */
  aData{0} = SHT2x_USER_REG_W;
  
  /* Sets data to be written */
  aData{1} = iValue;
  
  /* Writes data and returns result */
  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 2, 0);
    
  return iResult;
}

/**
 * Reads configuration data from the SHT21
 *
 * The function sends one byte of data containing the Read user register command to the sensor
 * and reads one byte of configuration data.
 * 
 * @param handle:TSHT21_Handle - Structure that contains SHT21 interface configuration
 * @param iValue:s32           - Configuration to be read from the SHT21
 * @return s32                 - OK:  If successful
 *                             - <OK: An ERROR occurred. See the manual.
 */
stock SHT21_ReadConfig(handle[TSHT21_Handle], &iValue)
{
  /* Temporary memory for the data and the return value of a function */
  new aData{1};
  new iResult;
  
  /* Sets SHT21 command */
  aData{0} = SHT2x_USER_REG_R;
  
  /* Writes and reads data */
  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 1, 1);
  if(iResult < OK)
    return(iResult);
  
  /* Returns data */
  iValue = aData{0};

  return OK;
}

/**
 * Triggers measurement for the SHT21
 * 
 * The function sends one byte of data containing the Trigger measurement command to the
 * sensor.
 * 
 * @param handle:TSHT21_Handle - Structure that contains SHT21 interface configuration
 * @param iCommand:s32         - Trigger measurement command
 * @return s32                 - OK:  If successful
 *                             - <OK: An ERROR occurred. See the manual.
 */
stock SHT21_TriggerMeasurement(handle[TSHT21_Handle], iCommand)
{
  /* Temporary memory for the data and the return value of a function */
  new aData{1};
  new iResult;
  
  /* Sets SHT21 command */
  aData{0} = iCommand;
  
  /* Writes data and returns result */
  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 1, 0);

  return iResult;
}

/**
 * Reads measurement data from the SHT21
 *
 * The function reads two bytes of measurement data from the SHT21.
 * 
 * @param handle:TSHT21_Handle - Structure that contains SHT21 interface configuration
 * @param iValue:s32           - Measurement value
 * @return s32                 - OK:  If successful
 *                             - <OK: An ERROR occurred. See the manual.
 */
stock SHT21_ReadMeasurement(handle[TSHT21_Handle], &iValue)
{
  /* Temporary memory for the data and the return value of a function */
  new aData{3};
  new iResult;
  
  /* Reads data */
  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 0, 3);
  if(iResult < OK)
    return(iResult);

  /* Checks CRC */
  iResult = _SHT21_CheckCrc(aData, 2, aData{2});
  if(iResult < OK)
  {
    printf("SHT21 [I2C %x]: CRC Error!\r\n", handle.addr);
    return(iResult);
  }

  /* Returns data */
  iResult = rM2M_Pack(aData, 0, iValue, RM2M_PACK_U16 + RM2M_PACK_GET + RM2M_PACK_BE);

  return iResult;
}

/**
 * Gets SHT21 temperature
 *
 * The function checks if any valid SHT21 temperature measurement is available and returns it
 * if so.
 * 
 * @param handle:TSHT21_Handle - Structure that contains SHT21 interface configuration
 * @param iValue:s32           - Temperature [0.01 °C]
 * @return s32                 - OK:    Data available
 *                             - ERROR: No data available
 */
stock SHT21_GetTemperature(handle[TSHT21_Handle], &iValue)
{
  /* Checks if the temperature is available */
  if(handle.valstate & SHT21_TEMPERATURE)
  {
    /* Gets value from SHT21 interface structure */
    iValue = handle.temp;
    return OK;
  }
  else
  {
    return ERROR;
  }
}

/**
 * Gets SHT21 humidity
 *
 * The function checks if any valid SHT21 humidity measurement is available and returns it
 * if so.
 * 
 * @param handle:TSHT21_Handle - Structure that contains SHT21 interface configuration
 * @param iValue:s32           - Humidity [0.1 %rH]
 * @return s32                 - OK:    Data available
 *                             - ERROR: No data available
 */
stock SHT21_GetHumidity(handle[TSHT21_Handle], &iValue)
{
  /* Checks if the humidity is available */
  if(handle.valstate & SHT21_HUMIDITY)
  {
    /* Gets value from SHT21 interface structure */
    iValue = handle.hum;
    return OK;
  }
  else
  {
    return ERROR;
  }
}


/**
 * Calculates a 8-bit CRC checksum and compares it with the received one
 *
 * The 0x131 Polynomial and 0x00 Initialization are referenced from the SHT21 datasheet.
 * In general, the SHT21 CRC covers the contents of two transmitted bytes of data.
 *
 * The first loop in this function performs a XOR operation on the current byte and the second loop
 * then applies the polynomial to the data bit by bit. After calculation has finished, the 8-bit CRC
 * checksums are compared and the result is returned.
 * 
 * @param data[]:u8             - Array that contains the data
 * @param nBytes:s32            - Number of bytes for checksum calculation
 * @param crcRead:s32           - Received 8-bit CRC checksum to be compared with
 *                                the calculated one
 * @return s32                 	- OK: 		Received and calculated CRC checksums are equal
 *                              -  ERROR:   Received and calculated CRC checksums are NOT equal
 */
stock _SHT21_CheckCrc(data{}, nBytes, crcRead)
{
  new POLYNOMIAL = 0x131; //P(x)=x^8+x^5+x^4+1 = 100110001
  new crc = 0x00;
  new byteCtr;

  // Calculates 8-Bit checksum with given polynomial
  for(byteCtr = 0; byteCtr < nBytes; ++byteCtr)
  {
    new bit;
    crc ^= (data{byteCtr});
    for(bit = 8; bit > 0; --bit)
    {
      if (crc & 0x80) crc = (crc << 1) ^ POLYNOMIAL;
      else crc = (crc << 1);
    }
  }

  if (crc != crcRead) return ERROR;
  else return OK;
}

/**
 * Gets SHT21 serial number
 *
 * The function reads data from memory location 1 & 2 and returns
 * the serial number for the given SHT21 sensor.
 * 
 * @param handle:TSHT21_Handle - Structure that contains SHT21 interface configuration
 * @param serial[]:s32         - Serial to be returned
 * @return s32                 - OK:    Data available
 *                             - ERROR: No data available
 */
stock SHT21_GetSerialNumber(handle[TSHT21_Handle], serial[])
{
  new aData{8};

  // Reads from memory location 1
  aData{0} = 0xFA; // Command for readout on-chip memory
  aData{1} = 0x0F; // On-chip memory address
  if(rM2M_I2cCom(handle.i2c, handle.addr, aData, 2, 8) < OK) return(ERROR);

  // Reads CRC & SNB (CRC is not analysed)
  serial[5] = aData{0}; // SNB_3
  serial[4] = aData{2}; // SNB_2
  serial[3] = aData{4}; // SNB_1
  serial[2] = aData{6}; // SNB_0

  // Reads from memory location 2
  aData{0} = 0xFC; // Command for readout on-chip memory
  aData{1} = 0xC9; // On-chip memory address
  if(rM2M_I2cCom(handle.i2c, handle.addr, aData, 2, 6) < OK) return(ERROR);

  // Reads CRC, SNC & SNA (CRC is not analysed)
  serial[1] = aData{0}; // SNC_1
  serial[0] = aData{1}; // SNC_0
  serial[7] = aData{3}; // SNA_1
  serial[6] = aData{4}; // SNA_0

  return(OK);
}