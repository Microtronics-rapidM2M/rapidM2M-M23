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
 */

// #define SHT31_DEBUG

/* Available SHT31 I2C addresses */
#define SHT31_I2C_ADR_A (0x88)  // ADDR pin=0
#define SHT31_I2C_ADR_B (0x8A)  // ADDR pin=1

/*  Measurement duration (affecting power consumption) according to SHT31 datasheet:
 *  low repeatability: max. 4ms
 *  medium repeatability: max. 6ms
 *  high repeatability: max. 15ms
 */

/* SHT31 commands */
const
{
  SHT31_CMD_READ_SERIALNBR  = 0x3780, // read serial number
  SHT31_CMD_READ_STATUS     = 0xF32D, // read status register
  SHT31_CMD_CLEAR_STATUS    = 0x3041, // clear status register
  SHT31_CMD_HEATER_ENABLE   = 0x306D, // enabled heater
  SHT31_CMD_HEATER_DISABLE  = 0x3066, // disable heater
  SHT31_CMD_SOFT_RESET      = 0x30A2, // soft reset
  SHT31_CMD_MEAS_CLOCKSTR_H = 0x2C06, // measurement: clock stretching, high repeatability
  SHT31_CMD_MEAS_CLOCKSTR_M = 0x2C0D, // measurement: clock stretching, medium repeatability
  SHT31_CMD_MEAS_CLOCKSTR_L = 0x2C10, // measurement: clock stretching, low repeatability
  SHT31_CMD_MEAS_POLLING_H  = 0x2400, // measurement: polling, high repeatability
  SHT31_CMD_MEAS_POLLING_M  = 0x240B, // measurement: polling, medium repeatability
  SHT31_CMD_MEAS_POLLING_L  = 0x2416, // measurement: polling, low repeatability
  SHT31_CMD_MEAS_PERI_05_H  = 0x2032, // measurement: periodic 0.5 mps, high repeatability
  SHT31_CMD_MEAS_PERI_05_M  = 0x2024, // measurement: periodic 0.5 mps, medium repeatability
  SHT31_CMD_MEAS_PERI_05_L  = 0x202F, // measurement: periodic 0.5 mps, low repeatability
  SHT31_CMD_MEAS_PERI_1_H   = 0x2130, // measurement: periodic 1 mps, high repeatability
  SHT31_CMD_MEAS_PERI_1_M   = 0x2126, // measurement: periodic 1 mps, medium repeatability
  SHT31_CMD_MEAS_PERI_1_L   = 0x212D, // measurement: periodic 1 mps, low repeatability
  SHT31_CMD_MEAS_PERI_2_H   = 0x2236, // measurement: periodic 2 mps, high repeatability
  SHT31_CMD_MEAS_PERI_2_M   = 0x2220, // measurement: periodic 2 mps, medium repeatability
  SHT31_CMD_MEAS_PERI_2_L   = 0x222B, // measurement: periodic 2 mps, low repeatability
  SHT31_CMD_MEAS_PERI_4_H   = 0x2334, // measurement: periodic 4 mps, high repeatability
  SHT31_CMD_MEAS_PERI_4_M   = 0x2322, // measurement: periodic 4 mps, medium repeatability
  SHT31_CMD_MEAS_PERI_4_L   = 0x2329, // measurement: periodic 4 mps, low repeatability
  SHT31_CMD_MEAS_PERI_10_H  = 0x2737, // measurement: periodic 10 mps, high repeatability
  SHT31_CMD_MEAS_PERI_10_M  = 0x2721, // measurement: periodic 10 mps, medium repeatability
  SHT31_CMD_MEAS_PERI_10_L  = 0x272A, // measurement: periodic 10 mps, low repeatability
  SHT31_CMD_FETCH_DATA      = 0xE000, // readout measurements for periodic mode
  SHT31_CMD_R_AL_LIM_LS     = 0xE102, // read alert limits, low set
  SHT31_CMD_R_AL_LIM_LC     = 0xE109, // read alert limits, low clear
  SHT31_CMD_R_AL_LIM_HS     = 0xE11F, // read alert limits, high set
  SHT31_CMD_R_AL_LIM_HC     = 0xE114, // read alert limits, high clear
  SHT31_CMD_W_AL_LIM_HS     = 0x611D, // write alert limits, high set
  SHT31_CMD_W_AL_LIM_HC     = 0x6116, // write alert limits, high clear
  SHT31_CMD_W_AL_LIM_LC     = 0x610B, // write alert limits, low clear
  SHT31_CMD_W_AL_LIM_LS     = 0x6100, // write alert limits, low set
  SHT31_CMD_NO_SLEEP        = 0x303E,
  SHT31_CMD_BREAK           = 0x3093, // break: stop periodic data acquisition mode
}

/* Structure for SHT31 interface configuration */
#define TSHT31_Handle[.i2c, .addr, .stat]

stock SHT31_Init(handle[TSHT31_Handle], i2c, addr)
{
  new iResult;

  handle.i2c = i2c;
  handle.addr = addr;
  iResult = SHT31_SoftReset(handle);

  if(iResult < OK)
  {
    /* SHT31 probably busy (e.g. periodic measurement) -> send BREAK command */
    iResult = SHT31_StopMeasurement(handle);
    if(iResult >= OK)
    {
      /* retry triggering SoftReset */
      iResult = SHT31_SoftReset(handle);
    }
  }
  
  /* NOTE:
   * Soft reset time max. 1.5ms according to SHT31 data sheet
   * (time between ACK of soft reset command and sensor entering idle state).
   */
  return(iResult);
}

stock SHT31_CheckCrc(data{}, nBytes, crcRead)
{
  new POLYNOMIAL = 0x131; //P(x)=x^8+x^5+x^4+1 = 100110001
  new crc = 0xFF;
  new byteCtr;

  //calculates 8-Bit checksum with given polynomial
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
   * (time between ACK of soft reset command and sensor entering idle state).
   */
  return(iResult);
}

/* calc RH value with a resolution of [0.1%] */
stock SHT31_CalcRH(rhRaw)
{
  // calculate relative humidity [%RH]
  // RH = rawValue / (2^16-1) * 100
  return((rhRaw*1000)/65535);
}

/* calc RH value with a resolution of [0.01°C] */
stock SHT31_CalcTemperatureC(tempRaw)
{
  // calculate temperature [°C]
  // T = -45 + 175 * rawValue / (2^16-1)
  return(17500*tempRaw/65535 - 4500);
}

stock SHT31_StopMeasurement(handle[TSHT31_Handle])
{
  new aData{2};
  new iResult;

  /* Information from datasheet concerning BREAK command (chapter 4.8):
   * It is recommended to stop the periodic data acquisition prior to sending another
   * command (except Fetch Data command) using the break command.
   */
  
  /* send BREAK command (stop periodic data acquisition mode) */
  aData{0} = SHT31_CMD_BREAK >> 8;
  aData{1} = SHT31_CMD_BREAK & 0xFF;
  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 2, 0);
  return(iResult);
}

stock SHT31_TriggerMeasurement(handle[TSHT31_Handle], cmd=SHT31_CMD_MEAS_POLLING_M)
{
  new aData{2};
  new iResult;

  if((cmd & 0xFF00) == 0x2C00)
  {
    /* Clock stretching command is not supported! */
    return(ERROR);
  }

  /* send measurement command: no clock stretching (polling) with
   * repeatability (measurement time) MEDIUM */
  aData{0} = cmd >> 8;
  aData{1} = cmd & 0xFF;
  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 2, 0);
  return(iResult);
}

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

  /* check CRC for temperature */
  iResult = SHT31_CheckCrc(aData, 2, aData{2});
  if(iResult < OK)
  {
    printf("SHT31 [I2C %x]: TEMP CRC Error!\r\n", handle.addr);
    return(ERROR);
  }
  
  /* calc temperature value */
  raw = (aData{0} << 8) | aData{1};
  temp = SHT31_CalcTemperatureC(raw);
  
  /* check CRC for RH value */
  aData{0} = aData{3};
  aData{1} = aData{4};
  iResult = SHT31_CheckCrc(aData, 2, aData{5});
  if(iResult < OK)
  {
    printf("SHT31 [I2C %x]: RH CRC Error!\r\n", handle.addr);
    return(ERROR);
  }

  /* calc RH value */
  raw = (aData{0} << 8) | aData{1};
  rh = SHT31_CalcRH(raw);
  return(OK);
}
