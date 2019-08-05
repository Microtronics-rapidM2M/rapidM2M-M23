/**
 *                  _     _  __  __ ___  __  __
 *                 (_)   | ||  \/  |__ \|  \/  |
 *  _ __ __ _ _ __  _  __| || \  / |  ) | \  / |
 * | '__/ _` | '_ \| |/ _` || |\/| | / /| |\/| |
 * | | | (_| | |_) | | (_| || |  | |/ /_| |  | |
 * |_|  \__,_| .__/|_|\__,_||_|  |_|____|_|  |_|
 *           | |
 *           |_|
 *
 * Simple "Transmission" Example
 *
 * Initiates a connection to the server 
 * The synchronisation of the configuration, registration and measurement data is 
 * automatically done by the firmware.
 *
 * Compatible with every rapidM2M hardware
 *
 * @version 20190508
 */



/* Application entry point */
main()
{
  new iResult;                              // Temporary memory for the return value of a function 

  /* Initiates a connection to the server and issues the return value of the function via the 
     console. The system (Firmware) automatically synchronises the configuration, registration 
	 and measurement data with the server.                                                   */
  iResult = rM2M_TxStart();                  
  if(iResult < OK)                          
    printf("rM2M_TxStart() = %d\r\n", iResult);
}
