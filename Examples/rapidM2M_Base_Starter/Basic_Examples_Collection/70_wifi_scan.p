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
 * Simple "WiFi" Example
 * 
 * Scans every 5 seconds for WiFi network in the receiving range and issues security type, WiFi RF channel and Service Set Identifier 
 * of the found WiFi networks via the console
 *
 * Only compatible with rapidM2M M2xx
 *
 * @version 20190508  
 */
 
#include <rapidM2M M2\WiFi>

/* Forward declarations of public functions */
forward public BasicTimer();                                        // Called up 1x per sec. for the program sequence
forward public WiFi_Callback(event, connhandle, const data{}, len); // Called up when a WiFi event occurs

/* Global variables */
static iCount=0;                                                    // Counter used for the program sequence 

/* Application entry point */
main()
{
  /* Temporary memory for the index of a public function and the return value of a function             */
  new iIdx;
  new iResult;
  
  /* Initialisation of a 1 sec. timer used for the general program sequence
     - Determining the function index that should be executed 1x per sec.
     - Transferring the index to the system
     - In case of a problem the index and return value of the init function are issued by the console  */
  iIdx = funcidx("BasicTimer");
  iResult = rM2M_TimerAdd(iIdx);
  if(iResult < OK)
    printf("Basic timer(%d) = %d\r\n", iIdx, iResult);
  
  rM2M_TxSelectItf(RM2M_TXITF_MODEM);          // Selects the mobile network modem as the communication interface
  rM2M_TxSetMode(RM2M_TXMODE_TRIG);            // Sets the connection type to "Interval" mode    
}

/* 1 sec. timer is used for the general program sequence */
public BasicTimer()
{
  /* Temporary memory for the index of a public function and the return value of a function             */
  new iIdx;
  new iResult;
  
  new iWiFi_State;                             // Temporary memory for the current state of the WiFi interface 
  
  iCount++;                                    // Increases counter for the program sequence

  iWiFi_State = WiFi_GetState();               // Reads the current state of the WiFi interface from the system
  printf("WiFi State:%d ", iWiFi_State);       // Issues the current state of the WiFi interface as numeric number via the console 
  
  switch(iWiFi_State)                          // Switch the current state of the WiFi interface -> 
  {
    // Issues the current state of the WiFi interface as readable text via the console
	case WIFI_STATE_OFF:
    {
      print("OFF");
    }
    case WIFI_STATE_INIT:
    {
      print("INIT");
    }
    case WIFI_STATE_READY:
    {
      print("READY");
    }
    case WIFI_STATE_BUSY:
    {
      print("BUSY");
    }
  }
  print("\r\n");
  
  if (iCount == 5)                             // Waits 5sec. after PowerOn to give the WiFi interface time to get into the ready state   
  {											   // Note: This is especially necessary if the WiFi interface was initially used to communicate 
                                               //       with the server and should now be used for communication with another device. 
    /* Initialisation of the WiFi interface.
     - Determining the function index that should be called up when a WiFi event occurs
     - Transferring the index to the system 
     - In case of an problem the return value of the init function is issued by the console             */	  	 
    iIdx = funcidx("WiFi_Callback");
    iResult = WiFi_Init(iIdx);
    if(iResult < OK)
      printf("WiFi Init:%d\r\n", iResult);
  }
  
  //If counter for the program sequence is greater than 10 and the WiFi interface is ready ->
  if ((iCount > 10) && (iWiFi_State == WIFI_STATE_READY)) 
  {
    iCount = 5;                                // Sets the counter for the program sequence to 5 to restart the scan in 5sec.
	                                           // Note: As the counter for the program sequence is increased before the 
											   //      "if (iCount == 5)" line, the code block that initialises the WiFi 
											   //      interface is only executed once after PowerOn.
	
	/* Starts the search for available WiFi networks on all channels supported by the built-in WiFi
	   module and issues an error message if an error occurs                                             */
    iResult = WiFi_Scan();
    if(iResult < OK)
      printf("WiFi Scan:%d\r\n",iResult);
  }
}
 
/**
 * Function that should be called up when a WiFi event occurs
 *
 * @param event:s32      - WiFi event that has occurred   
 * @param connhandle:s32 - Connection Handle (only relevant for "WIFI_EVENT_SOCKET_xxx" events)
 * @param data[]:u8      - Array that contains the data. The structure depends on which WiFi event has occurred. 
 * @param len:s32        - Size of the data block in bytes
 */
public WiFi_Callback(event, connhandle, const data{}, len) 
{
  new aWiFi_Scan[TWiFi_Scan];                  // Temporary memory for information about a WiFi network in the receiving range
  
  // Issues the occurred event, the connection handle and the size of the data block via the console
  printf("WiFi Callback: E:%d C:%d L:%d\r\n",event, connhandle, len);
  
  if (event == WIFI_EVENT_SCAN)                // If an access point was found during scan
  {
    /* Copies the information from the data block to the temporary memory with a specific structure.
	   This is necessary as the information in the data block is packed and therefore cannot be processed directly. */
	rM2M_Pack(data, 0, aWiFi_Scan.sec, RM2M_PACK_S8 | RM2M_PACK_GET);
    rM2M_Pack(data, 1, aWiFi_Scan.ch, RM2M_PACK_S8 | RM2M_PACK_GET);
    rM2M_GetPackedB(data, 2, aWiFi_Scan.bssid, 6);
    rM2M_GetPackedB(data, 8, aWiFi_Scan.ssid, 33);
    rM2M_Pack(data, 41, aWiFi_Scan.rssi, RM2M_PACK_S8 | RM2M_PACK_GET);
	
    Event_WifiScan(aWiFi_Scan);                // Calls up the function to process the received information about a WiFi network
  }
}

/**
 * Function to process the received information about a WiFi network
 *
 * @param aWiFi_Scan:TWiFi_Scan - Information about a WiFi network in the receiving range
 */
Event_WifiScan(aWiFi_Scan[TWiFi_Scan])
{
  // Issues the security type, WiFi RF channel and Service Set Identifier via the console 
  printf("WiFi Scan: %d %d %s\r\n", aWiFi_Scan.sec, aWiFi_Scan.ch, aWiFi_Scan.ssid);
}