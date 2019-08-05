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
 * Simple "WiFi Transmission" Example
 *
 * Selects the WiFi module as the communication interface and initiates a connection to the server 
 * The synchronisation of the configuration, registration and measurement data is 
 * automatically done by the firmware.
 *
 * Only compatible with rapidM2M M2xx
 *
 * @version 20190508  
 */



/* Application entry point */
main()
{
  new iResult;                              // Temporary memory for the return value of a function 

  
  /* In order to use the WiFi module for communication with the server, at least the SSID and 
     password for the network to be used must be written into the registration memory block 
	 "REG_APP_FLASH". Thereby, the following code block or an external tool can be used.	 
	 Note: It is only once necessary to write the following settings.                       */
  
  // iResult = rM2M_RegSetString(RM2M_REG_APP_FLASH, "wifiSSID", "TRAINING");         // Writes SSID only
  // iResult = rM2M_RegSetString(RM2M_REG_APP_FLASH, "wifiSSID", "WPA_PSK;TRAINING"); // Writes security type and SSID (Alternative to the line above)
  // if(iResult < OK)
  //   printf("Set WiFi SSID (%d)\r\n",iResult);
  // iResult = rM2M_RegSetString(RM2M_REG_APP_FLASH, "wifiPwd", "Password-training"); // Writes password
  // if(iResult < OK)
  //   printf("Set WiFi PWD (%d)\r\n",iResult);


  
  /* Selects the WiFi module as the communication interface used for the communication with 
     server and issues the return value of the function via the console. 
	 For using the mobile network modem replace "RM2M_TXITF_WIFI" with "RM2M_TXITF_MODEM"    */
  iResult = rM2M_TxSelectItf(RM2M_TXITF_WIFI);
  if(iResult < OK)
    printf("rM2M_TxSelectItf() = %d\r\n", iResult);
  
  /* Initiates a connection to the server and issues the return value of the function via the 
     console. The system (Firmware) automatically synchronises the configuration, registration 
	 and measurement data with the server.                                                   */
  iResult = rM2M_TxStart();
  if(iResult < OK)
    printf("rM2M_TxStart() = %d\r\n", iResult);
}
