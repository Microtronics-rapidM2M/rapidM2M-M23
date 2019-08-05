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
 * Extended "Online Mode Transmission" Example
 *
 * Establishes and tries to maintain an online connection to the server.
 * As long as the device is in online mode, a synchronisation of the configuration, registration and
 * measurement data is initiated every 2 hours. If the online connection is interrupted, the 
 * device tries up to 10 times to reestablish the connection. If the re-establishment of the 
 * connection was not possible within these 10 attempts, the device waits for 1 hour before
 * starting another 10 attempts to establish the connection. 
 * 
 * Compatible with every rapidM2M hardware.
 *
 * @version 20190508
 */

/* Forward declarations of public functions */
forward public Timer1s();                  // Called up 1x per sec. for the program sequence

const 
{
  TX_INTERVAL       = 2 * 60 * 60,          // Transmission interval [sec.], default 120min
                                            // Note: For transmission intervals < 10 min the "Online" mode is recommended  
  TX_RETRY_MAX      = 10,					// Maximum retries for establishing the online connection 	
  TX_RETRY_INTERVAL = 60 * 60,				// Waiting time before restarting the connection attempts [sec.], default 60min
}

static iTxTimer;                            // Sec. until the next transmission

/* Application entry point */
main()
{
  /* Temporary memory for the index of a public function and the return value of a function             */
  new iIdx, iResult;

  /* Initialisation of a cyclic 1 sec. timer
     - Determining the function index that should be executed 1x per sec.
     - Transferring the index to the system
	 - In case of a problem the index and return value of the init function are issued by the console  */
  iIdx = funcidx("Timer1s");
  iResult = rM2M_TimerAdd(iIdx);
  if(iResult < OK)
    printf("rM2M_TimerAdd(%d) = %d\r\n", iIdx, iResult);
  
  /* Setting the counter to 0 immediately triggers a transmission.
     You could also refrain from setting it to 0, as all variables in PAWN are initialised with 0
     at the time of their creation. However, it was completed at this point for the purpose 
	 of a better understanding.    		                                                               */
  iTxTimer  = 0;	
    
  rM2M_TxSetMode(RM2M_TXMODE_ONLINE);       // Setting the connection type to "Online" mode
}

/* Cyclic 1s Timer */
public Timer1s()
{
  new iResult;								// Temporary memory for the return value of a function 
  new iTxCurrentState;                      // Temporary memory for the current connection status
  static iForceOnlineTry;                   // Retry counter for establishing the online connection 
  static iTxRetriggerTimer;					// Counter for restarting the connection tries (sec. until the next retry 
                                            // to establish the online connection if the max retries are exceeded)
  
  iTxCurrentState = rM2M_TxGetStatus();     // Gets the current connection status 
  
  /* Temporary memory indicating if the device is currently connecting to the server or already connected to the server.
     Is 0 if the connection is not active, has not started, is not waiting for next automatic retry and is not disabled */
  new iIsConnected = iTxCurrentState & (RM2M_TX_ACTIVE|RM2M_TX_STARTED|RM2M_TX_RETRY|RM2M_TX_DISABLED);

  if((iIsConnected == 0) && (iTxTimer > 5) && ((TX_INTERVAL - iTxTimer) > 10) && (iForceOnlineTry < TX_RETRY_MAX))
  { 
    /* Force reconnection to get back into "Online" mode by setting the counter counting down the sec. to the next 
	   transmission to 0 if 
	   - the connection is not active, has not started, is not waiting for next automatic retry and is not disabled
       - AND connection is not starting within the next 5 seconds
       - AND connection was not triggered 10 seconds ago
	   - AND max retries were not reached
       Note: Max retry recommended!								                                                       */
    iTxTimer = 0;
    
    iForceOnlineTry++;                      // Increases counter for the retries to establishing the online connection
    if(iForceOnlineTry >= TX_RETRY_MAX)     // If max retries exceeded ->
    { 
      iTxRetriggerTimer = TX_RETRY_INTERVAL;// Sets Counter for restarting the connection tries to TX_RETRY_INTERVAL seconds
    }
  }
  
  /* Wait until iTxRetriggerTimer is 0 to restart connection tries */
  if(iTxRetriggerTimer > 0)                 // If the counter for restarting the connection tries has not expired -> 
  { 
    iTxRetriggerTimer--;					// Counting down the sec. until the connection attempts are restarted
    if(iTxRetriggerTimer <= 0)				// When the counter has expired -> 
    {
      iForceOnlineTry = 0;                  // Resets the retry counter for establishing the online connection
    }
  }
  
  iTxTimer--;								// Counter counting down the sec. to the next transmission
  if(iTxTimer <= 0)                         // When the counter has expired -> 
  {
    /* If the device is currently in "Online" mode, the rM2M_TxStart() function only initiates a 
	   synchronisation of the configuration, registration and measurement data with the server.
       If the device is not online, the function initiates a connection to the server and the system 
	   (Firmware) automatically synchronises the configuration, registration and measurement data 
	   with the server. In any case the return value of the function is issued via the console.         */  
	iResult = rM2M_TxStart();
    if(iResult < OK)
      printf("rM2M_TxStart() = %d\r\n", iResult);
	  
    iTxTimer = TX_INTERVAL;                 // Resets counter var. to current transmission interval [sec.] 
  }
}
