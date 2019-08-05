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
 * Extended "Transmission" Example
 *
 * Initiates a connection to the server every 2 hours
 * The synchronisation of the configuration, registration and measurement data is 
 * automatically done by the firmware.
 * 
 * Compatible with every rapidM2M hardware.
 *
 * @version 20190508
 */

 
 
/* Forward declarations of public functions */
forward public Timer1s();

const 
{
  TX_INTERVAL = 2 * 60 * 60,				// Transmission interval [sec.], default 120 min
}                                           // Note: For transmission intervals < 10 min the "Online" mode is recommended

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
}

/* Cyclic 1s timer */
public Timer1s()
{
  new iResult;                              // Temporary memory for the return value of a function 
  
  iTxTimer--;                               // Counter counting down the sec. to the next transmission
  if(iTxTimer <= 0)                         // When the counter has expired -> 
  {
    /* Initiates a connection to the server and issues the return value of the function via the 
       console. The system (Firmware) automatically synchronises the configuration, registration 
	   and measurement data with the server.                                                            */  
    iResult = rM2M_TxStart();
    if(iResult < OK)
      printf("rM2M_TxStart() = %d\r\n", iResult);
    
	iTxTimer = TX_INTERVAL;                 // Resets counter var. to current transmission interval [sec.] 
  }
  
  printf("Transmission in %d sec.\r\n", iTxTimer); // Prints the seconds until the next transmission to the console
}
