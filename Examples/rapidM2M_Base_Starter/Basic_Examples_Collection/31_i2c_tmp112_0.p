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
 * Simple "I2C TMP112" Example
 * 
 * Reads temperature measurement from the TMP112 every second and prints the converted result
 * to the console
 * 
 * Only compatible with rapidM2M M3 and rapidM2M M2xx
 * Special hardware circuit necessary
 * 
 * @version 20190715
 */

#include "tmp112.inc"

/* Handle for available sensors */
static hTemp[TTMP112_Handle];

/* Forward declarations of public functions */
forward public Timer1s();

const
{
  PORT_I2C = 0,                           // I2C port used for communication with temperature sensor
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

  /* Initialisation of TMP112 sensor with address 1 (0x90). The address is determined by the ADD0 pin.
     In this case the ADD0 pin is connected to GND. The result is printed to the console. */
  if(TMP112_Init(hTemp, PORT_I2C, TMP112_I2C_ADR1) < OK)
  {
    print("TMP112 Error\r\n");
  }
  else 
    print("TMP112 OK\r\n");
}

/* 1 sec. timer is used for the general program sequence */
public Timer1s()
{
  /* Temporary memory for the raw measurement value and the converted measurement result */
  new iTemp;
  
  /* Reads actual temperature from TMP112 sensor */
  printf("Result: %d\r\n", TMP112_GetTemp(hTemp, iTemp));

  /* Prints the converted measurement result in °C to the console */
  printf("Temp = %.2f degree C\r\n", float(iTemp)/100);
}
