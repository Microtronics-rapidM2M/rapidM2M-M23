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
 * Simple "I2C SHT21" Example
 * 
 * Reads temperature and humidity measurement from the SHT21 every second and prints the
 * converted result to the console
 * 
 * Only compatible with rapidM2M M3 and rapidM2M M2xx
 * Special hardware circuit necessary
 * 
 * @version 20190716
 */

#include "sht21.inc"

/* Handle for available sensors */
static hTempRh[TSHT21_Handle];

/* Forward declarations of public functions */
forward public Timer1s();

const
{
  PORT_I2C = 0,                             // I2C port used for communication with SHT21
}

/* Application entry point */
main()
{
  /* Temporary memory for the index of a public function and the return value of a function */
  new iIdx, iResult;

  /* Initialisation of a 1 sec. timer used for the general program sequence
     - Determining the function index that should be executed 1x per sec.
     - Transferring the index to the system
     - Index and function return value used to generate the timer are issued via the console */
  iIdx = funcidx("Timer1s");
  iResult = rM2M_TimerAdd(iIdx);
  printf("rM2M_TimerAdd(%d) = %d\r\n", iIdx, iResult);

  /* Initialisation of I2C #0 with 400kHz clock */
  iResult = rM2M_I2cInit(PORT_I2C, 400000, 0);
  printf("rM2M_I2cInit() = %d\r\n", iResult);

  /* Initialisation of SHT21 sensor with address 1 (0x80) */
  if(SHT21_Init(hTempRh, PORT_I2C, SHT21_I2C_ADR1) < OK)
  {
    printf("SHT21 Error\r\n");
  }
  else 
  {
    printf("SHT21 OK\r\n");
  }
}

/* 1 sec. timer is used for the general program sequence */
public Timer1s()
{
  /* Handles SHT21 measurement process */
  SHT21_HandleTimer(hTempRh);
  
  /* Gets the measurement data and prints it to the console */
  Handle_Record();
}

Handle_Record()
{
  new iResult;                              // Temporary memory for the return value of a function
  new iTemperature;                         // Temporary memory for the temperature
  new iHumidity;                            // Temporary memory for the humidity

  /* Reads measurements */
  iResult = SHT21_GetTemperature(hTempRh, iTemperature); // iTemperature with 0.01 scale
  if(iResult < OK)
  {
    iTemperature = 0x7FFF;                  // Set to NaN
    printf("ReadTemp(%d)\r\n", iResult);
  }
    
  iResult = SHT21_GetHumidity(hTempRh, iHumidity);       // iHumidity with 0.1 scale
  if(iResult < OK)
  {
    iHumidity = 0x7FFF;                     // Set to NaN
    printf("ReadRH(%d)\r\n", iResult);
  }
  
  /* Prints the temperature and humidity to the console */
  printf("T:%g rH:%g\r\n", float(iTemperature) / 100.0, float(iHumidity) / 10.0);
}

