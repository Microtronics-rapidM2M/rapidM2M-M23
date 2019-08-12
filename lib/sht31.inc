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
 * SHT31 interface functions
 * 
 * The SHT31 is a fully calibrated, linearised and temperature compensated temperature and humidity sensor.
 * It has a digital output and can be accessed via I2C interface with communication speed up to 1 MHz.
 * Furthermore, the device has two user selectable addresses and an accuracy of +-2%RH and +-0.3°C.
 * 
 * Interfaces: 1x I2C
 * 
 * @version 20190730
 *
 * Revision history:
 * 20190730
 * - Changed functions to use the rM2M_Pack() function
 */

// #define SHT31_DEBUG

/* Available SHT31 I2C addresses */
#define SHT31_I2C_ADR_A (0x88)  // ADDR pin=0
#define SHT31_I2C_ADR_B (0x8A)  // ADDR pin=1

/*  Measurement duration (affecting power consumption) according to SHT31 datasheet:
 *  Low repeatability: max. 4ms
 *  Medium repeatability: max. 6ms
 *  High repeatability: max. 15ms
 */

/* SHT31 commands */
const
{
  SHT31_CMD_READ_SERIALNBR  = 0x3780, // Reads serial number
  SHT31_CMD_READ_STATUS     = 0xF32D, // Reads status register
  SHT31_CMD_CLEAR_STATUS    = 0x3041, // Clears status register
  SHT31_CMD_HEATER_ENABLE   = 0x306D, // Enables heater
  SHT31_CMD_HEATER_DISABLE  = 0x3066, // Disables heater
  SHT31_CMD_SOFT_RESET      = 0x30A2, // Soft reset
  SHT31_CMD_MEAS_CLOCKSTR_H = 0x2C06, // Measurement: clock stretching, high repeatability
  SHT31_CMD_MEAS_CLOCKSTR_M = 0x2C0D, // Measurement: clock stretching, medium repeatability
  SHT31_CMD_MEAS_CLOCKSTR_L = 0x2C10, // Measurement: clock stretching, low repeatability
  SHT31_CMD_MEAS_POLLING_H  = 0x2400, // Measurement: polling, high repeatability
  SHT31_CMD_MEAS_POLLING_M  = 0x240B, // Measurement: polling, medium repeatability
  SHT31_CMD_MEAS_POLLING_L  = 0x2416, // Measurement: polling, low repeatability
  SHT31_CMD_MEAS_PERI_05_H  = 0x2032, // Measurement: periodic 0.5 mps, high repeatability
  SHT31_CMD_MEAS_PERI_05_M  = 0x2024, // Measurement: periodic 0.5 mps, medium repeatability
  SHT31_CMD_MEAS_PERI_05_L  = 0x202F, // Measurement: periodic 0.5 mps, low repeatability
  SHT31_CMD_MEAS_PERI_1_H   = 0x2130, // Measurement: periodic 1 mps, high repeatability
  SHT31_CMD_MEAS_PERI_1_M   = 0x2126, // Measurement: periodic 1 mps, medium repeatability
  SHT31_CMD_MEAS_PERI_1_L   = 0x212D, // Measurement: periodic 1 mps, low repeatability
  SHT31_CMD_MEAS_PERI_2_H   = 0x2236, // Measurement: periodic 2 mps, high repeatability
  SHT31_CMD_MEAS_PERI_2_M   = 0x2220, // Measurement: periodic 2 mps, medium repeatability
  SHT31_CMD_MEAS_PERI_2_L   = 0x222B, // Measurement: periodic 2 mps, low repeatability
  SHT31_CMD_MEAS_PERI_4_H   = 0x2334, // Measurement: periodic 4 mps, high repeatability
  SHT31_CMD_MEAS_PERI_4_M   = 0x2322, // Measurement: periodic 4 mps, medium repeatability
  SHT31_CMD_MEAS_PERI_4_L   = 0x2329, // Measurement: periodic 4 mps, low repeatability
  SHT31_CMD_MEAS_PERI_10_H  = 0x2737, // Measurement: periodic 10 mps, high repeatability
  SHT31_CMD_MEAS_PERI_10_M  = 0x2721, // Measurement: periodic 10 mps, medium repeatability
  SHT31_CMD_MEAS_PERI_10_L  = 0x272A, // Measurement: periodic 10 mps, low repeatability
  SHT31_CMD_FETCH_DATA      = 0xE000, // Reads out measurements for periodic mode
  SHT31_CMD_R_AL_LIM_LS     = 0xE102, // Reads alert limits, low set
  SHT31_CMD_R_AL_LIM_LC     = 0xE109, // Reads alert limits, low clear
  SHT31_CMD_R_AL_LIM_HS     = 0xE11F, // Reads alert limits, high set
  SHT31_CMD_R_AL_LIM_HC     = 0xE114, // Reads alert limits, high clear
  SHT31_CMD_W_AL_LIM_HS     = 0x611D, // Writes alert limits, high set
  SHT31_CMD_W_AL_LIM_HC     = 0x6116, // Writes alert limits, high clear
  SHT31_CMD_W_AL_LIM_LC     = 0x610B, // Writes alert limits, low clear
  SHT31_CMD_W_AL_LIM_LS     = 0x6100, // Writes alert limits, low set
  SHT31_CMD_NO_SLEEP        = 0x303E,
  SHT31_CMD_BREAK           = 0x3093, // Break: stops periodic data acquisition mode
}

/**
* Structure for SHT31 interface configuration
* 
* i2c:s32      - I2C port used for communication
* addr:s32     - Address of the I2C device
* state:s32    - Reserved
*/
#define TSHT31_Handle[.i2c, .addr, .state]

/**
 * Initialisation of I2C communication with SHT31
 *
 * This function copies the I2C port and address to the given structure.
 * A soft reset is then performed. The return value indicates whether the sensor is busy
 * and proceeds with an additional BREAK command if so.
 * 
 * @author: Microtronics
 * @param handle:TSHT31_Handle 	- Structure that contains SHT31 interface configuration
 * @param i2c:s32               - I2C port used for communication 
 * @param addr:s32              - Address of the I2C device
 * @return s32                 	- OK: 		If successful
 *                              - ERROR:    Number of data to be sent >255 or number of data to be received >255
 *                              - < OK:     Another ERROR occured. See the manual.
 */
stock SHT31_Init(handle[TSHT31_Handle], i2c, addr)
{
  new iResult;

  handle.i2c = i2c;
  handle.addr = addr;
  iResult = SHT31_SoftReset(handle);

  if(iResult < OK)
  {
    /* SHT31 probably busy (e.g. periodic measurement) -> Sends BREAK command */
    iResult = SHT31_StopMeasurement(handle);
    if(iResult >= OK)
    {
      /* Retries triggering SoftReset */
      iResult = SHT31_SoftReset(handle);
    }
  }
  
  /* NOTE:
   * Soft reset time max. 1.5ms according to SHT31 data sheet
   * (Time between ACK of soft reset command and sensor entering idle state).
   */
  return(iResult);
}

/**
 * Calculates a 8-bit CRC checksum and compares it with the received one
 *
 * The 0x131 Polynomial and 0xFF initialisation are referenced from the SHT31 datasheet.
 * In general a SHT31 CRC only covers the contents of two transmitted bytes of data.
 *
 * The first loop in this function performs a XOR operation on the current byte and the second loop
 * then applies the polynomial to the data bit by bit. After calculation has finished, the 8-bit CRC
 * checksums are compared and the result is returned.
 * 
 * @author: Microtronics
 * @param data[]:u8			 	- Array that contains the data
 * @param nBytes:s32            - Number of bytes for checksum calculation
 * @param crcRead:s32           - Received 8-bit CRC checksum to be compared with
 *								  the calculated one
 * @return s32                 	- OK: 		Received and calculated CRC checksums are equal
 *								- ERROR:	Received and calculated CRC checksums are NOT equal
 */
stock _SHT31_CheckCrc(data{}, nBytes, crcRead)
{
  new POLYNOMIAL = 0x131; //P(x)=x^8+x^5+x^4+1 = 100110001
  new crc = 0xFF;
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
 * Performs a softreset on the SHT31
 * 
 * This function buffers the 16-bit softreset command and then sends it to the sensor. In addition, a
 * message is printed to the console if the macro SHT31_DEBUG is defined.
 * 
 * @author: Microtronics
 * @param handle:TSHT31_Handle 	- Structure that contains SHT31 interface configuration
 * @return s32                 	- OK: 		If successful
 *							   	- ERROR:	Number of data to be sent >255 or number of data to be received >255
 *								- <OK:		Another ERROR occurred. See the manual.
 */
stock SHT31_SoftReset(handle[TSHT31_Handle])
{
  new aData{2};
  new iResult;

#if defined SHT31_DEBUG
  print("Softreset!\r\n");
#endif
  aData{0} = SHT31_CMD_SOFT_RESET >> 8;
  aData{1} = SHT31_CMD_SOFT_RESET & 0xFF;
  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 2, 0);

  /* NOTE:
   * Soft reset time max. 1.5ms according to SHT31 data sheet
   * (Time between ACK of soft reset command and sensor entering idle state).
   */
  return(iResult);
}

/**
 * Calculates relative humidity value with a resolution of [0.1%]
 *
 * This function calculates and returns relative humidity according to the formula in the SHT31 datasheet.
 * 
 * @author: Microtronics
 * @param rhRaw:s32 			- Raw rH measurement value
 * @return s32                  - Relative humidity in %
 */
stock _SHT31_CalcRH(rhRaw)
{
  // Calculates relative humidity [%RH]
  // RH = rawValue / (2^16-1) * 100
  return((rhRaw * 1000) / 65535);
}

/**
 * Calculates temperature value with a resolution of [0.01°C]
 *
 * This function calculates and returns temperature according to the formula in the SHT31 datasheet.
 * 
 * @author: Microtronics
 * @param tempRaw:s32 			- Raw temperature measurement value
 * @return s32                  - Temperature in °C
 */
stock _SHT31_CalcTemperatureC(tempRaw)
{
  // Calculates temperature [°C]
  // T = -45 + 175 * rawValue / (2^16-1)
  return(17500 * tempRaw / 65535 - 4500);
}

/**
 * Forces the SHT31 to stop concerning the periodic data acquisition
 * 
 * This function buffers the 16-bit break command and then sends it to the sensor.
 * 
 * @author: Microtronics
 * @param handle:TSHT31_Handle 	- Structure that contains SHT31 interface configuration
 * @return s32                 	- OK: 		If successful
 *							   	- ERROR:	Number of data to be sent >255 or number of data to be received >255
 *								- <OK:		Another ERROR occurred. See the manual.
 */
stock SHT31_StopMeasurement(handle[TSHT31_Handle])
{
  new aData{2};
  new iResult;

  /* Information from datasheet concerning BREAK command (chapter 4.8):
   * It is recommended to stop the periodic data acquisition prior to sending another
   * command (except Fetch Data command) using the break command.
   */
  
  /* Sends BREAK command (stops periodic data acquisition mode) */
  aData{0} = SHT31_CMD_BREAK >> 8;
  aData{1} = SHT31_CMD_BREAK & 0xFF;
  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 2, 0);
  return(iResult);
}

/**
 * Sends a command to the SHT31
 * 
 * This function buffers the given 16-bit command and then sends it to the sensor. Clock stretching is
 * not supported so if any corresponding command is detected, ERROR will be returned.
 * 
 * @author: Microtronics
 * @param handle:TSHT31_Handle 	- Structure that contains SHT31 interface configuration
 * @param cmd:s32				- Command that should be sent to the SHT31
 * @return s32                 	- OK: 		If successful
 *							   	- ERROR:	Number of data to be sent >255 or
 *											number of data to be received >255 or
 *											a clock stretching command is detected
 *								- <OK:		Another ERROR occurred. See the manual.
 */
stock SHT31_TriggerMeasurement(handle[TSHT31_Handle], cmd=SHT31_CMD_MEAS_POLLING_M)
{
  new aData{2};
  new iResult;

  if((cmd & 0xFF00) == 0x2C00)
  {
    /* Clock stretching command is not supported! */
    return(ERROR);
  }

  /* Sends measurement command: no clock stretching (polling) with
   * repeatability (measurement time) MEDIUM */
  iResult = rM2M_Pack(aData, 0, cmd, RM2M_PACK_U16 + RM2M_PACK_BE);
  if(iResult < OK)
    return(iResult);

  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 2, 0);
  return(iResult);
}

/**
 * Reads measurement output of the SHT31
 * 
 * This function buffers the 16-bit fetch data command and then sends it to the sensor. The structure of a
 * measurement consists of two bytes temperature data followed by an 8-bit CRC checksum and two bytes
 * humidity data followed by a second 8-bit CRC checksum. These 6 bytes are altogether buffered in an
 * array as well as the command. After assuring that the received data is correct, it is converted
 * accordingly. If an error occurs during I2C communication or CRC control, the corresponding error code
 * will be returned immediately.
 * 
 * @author: Microtronics
 * @param handle:TSHT31_Handle 	- Structure that contains SHT31 interface configuration 
 * @param temp:s32				- Buffer for the temperature value in °C (resolution of 0.01°C)
 * @param rh:s32				- Buffer for the relative humidity value in % (resolution of 0.1%)
 * @return s32                 	- OK: 		If successful
 *							   	- ERROR:	Number of data to be sent >255 or
 *											number of data to be received >255 or
 *											received and calculated CRC checksums are NOT equal
 *								- <OK:		Another ERROR occurred. See the manual.
 */
stock SHT31_ReadMeasurement(handle[TSHT31_Handle], &temp, &rh)
{
  new aData{6};
  new iResult;
  new raw;

  aData{0} = SHT31_CMD_FETCH_DATA >> 8;
  aData{1} = SHT31_CMD_FETCH_DATA & 0xFF;
  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 2, 6);
  if(iResult < OK)
    return(iResult);

  /* Checks CRC for temperature */
  iResult = _SHT31_CheckCrc(aData, 2, aData{2});
  if(iResult < OK)
  {
    printf("SHT31 [I2C %x]: TEMP CRC Error!\r\n", handle.addr);
    return(ERROR);
  }
  
  /* Calculates temperature value */
  iResult = rM2M_Pack(aData, 0, raw, RM2M_PACK_U16 + RM2M_PACK_GET + RM2M_PACK_BE);
  if(iResult < OK)
    return(iResult);

  temp = _SHT31_CalcTemperatureC(raw);
  
  /* Checks CRC for RH value */
  aData{0} = aData{3};
  aData{1} = aData{4};
  iResult = _SHT31_CheckCrc(aData, 2, aData{5});
  if(iResult < OK)
  {
    printf("SHT31 [I2C %x]: RH CRC Error!\r\n", handle.addr);
    return(ERROR);
  }

  /* Calculates RH value */
  iResult = rM2M_Pack(aData, 0, raw, RM2M_PACK_U16 + RM2M_PACK_GET + RM2M_PACK_BE);
  if(iResult < OK)
    return(iResult);

  rh = _SHT31_CalcRH(raw);
  
  return(OK);
}
