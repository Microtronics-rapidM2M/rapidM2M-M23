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
 * 
 * Simple "System Values" Example
 *
 * Reads the last valid values for Vin and Vaux from the system and issues them every second via the console 
 *
 * Only compatible with rapidM2M M2xx
 * 
 *
 * @version 20190508  
 */

#include ".\rapidM2M M2\mx.inc"

/* Forward declarations of public functions */
forward public Timer1s();                   // Called up 1x per sec. to read the last valid values for Vin and Vaux

/* Application entry point */
main()
{
  /* Temporary memory for the index of a public function and the return value of a function  */
  new iIdx, iResult;

  /* Initialisation of a 1 sec. timer used for the general program sequence
     - Determining the function index that should be executed 1x per sec.
     - Transferring the index to the system
     - in case of a problem the index and return value of the init function are issued by the console  */
  iIdx = funcidx("Timer1s");
  iResult = rM2M_TimerAdd(iIdx);
  if(iResult < OK)
    printf("rM2M_TimerAdd(%d) = %d\r\n", iIdx, iResult);
}

/* 1s Timer used to read the last valid values for Vin and Vaux from the system and issue them via the console   */
public Timer1s()
{
  new aSysValues[TMx_SysValue];             // Temporary memory for the internal measurement values
  
  Mx_GetSysValues(aSysValues);              // Reads the last valid values for Vin and Vaux from the system 
                                            // The interval for determining these values is 1sec. and cannot be changed.        
  
  // Issues the last valid values for Vin and Vaux via the console
  printf("Vin = %dmV Vaux = %dmV\r\n", aSysValues.VIn, aSysValues.VAux);
}
