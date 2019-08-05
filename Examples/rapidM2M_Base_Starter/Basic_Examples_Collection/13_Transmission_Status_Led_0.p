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
 * Simple "Indicating the transmission state" Example
 *
 * Initiates a connection to the server and uses external LED to indicate the current connection state
 * LED is off 	if the device is offline (not connected) or in "Wakeup" mode
 * LED is on    if the device is connected to the server 
 * LED flickers if the connection establishment has started or the device is waiting for the next automatic retry
 * LED blinks 	if the connection establishment has failed
 *
 * Note: The external LED must be connected between 3V3(<=250mA) and LED status.
 *
 * Only compatible with rapidM2M M2xx
 *
 * @version 20190723
 */

/* Path for hardware-specific include file */
#include ".\rapidM2M M2\mx.inc"

/*
 * TX_STATE STARTED     Flicker
 * TX_STATE RETRY       Flicker
 * TX_STATE ACTIVE      Constantly on
 * TX_STATE WAKEUPABLE  Off
 * TX_STATE NONE        Off
 * TX_STATE FAILED      Blink
 */

/* Forward declarations of public functions */
forward public Timer1s();                   // Called up 1x per sec. for the program sequence


/* Application entry point */
main()
{
 /* Temporary memory for the index of a public function and the return value of a function  */
  new iIdx, iResult;

  printf("Led Demo\r\n");                   // Issues the name of the example via the console

  /* Initialisation of a 1 sec. timer used for the general program sequence
     - Determining the function index that should be executed 1x per sec.
     - Transferring the index to the system
	   - In case of a problem the index and return value of the init function are issued by the console  */
  iIdx = funcidx("Timer1s");
  iResult = rM2M_TimerAdd(iIdx);
  if(iResult < OK)
    printf("rM2M_TimerAdd(%d) = %d\r\n", iIdx, iResult);
  
  /* Initialisation of the LED -> control via script activated */ 
  iResult = MxLed_Init(MX_LED_MODE_SCRIPT);
  printf("MxLed_Init() = %d\r\n", iResult);
  
  MxLed_Off(true);                          // Switches off LED

  rM2M_TxSetMode(RM2M_TXMODE_TRIG);         // Sets the connection type to "Interval" mode
  rM2M_TxStart();                           // Initiates a connection to the server
}

/* 1 sec. timer is used for the general program sequence */
public Timer1s()
{
  new iTxState;                             // Temporary memory for the current connection status
  static iTxStateAkt;                       // Last determined connection status 
  
  iTxState = rM2M_TxGetStatus();            // Gets the current connection status
  if(iTxState != iTxStateAkt)               // If the connection status has changed ->  
  {
    if(iTxState == 0)                       // If the connection is not active, has not started and is not waiting for next automatic retry 
    {
      print("iTxState: ---\r\n");
      MxLed_Off(true);                      // Switches off LED
    }
    else                                    // Otherwise ->
    {
      print("iTxState: ");
      if(iTxState & RM2M_TX_FAILED)         // If the connection establishment failed ->
      {
        print("FAILED | ");
        // LED should blink
        MxLed_Off(true);
        MxLed_Blink(0);
      }
      if(iTxState & RM2M_TX_ACTIVE)         // If the GPRS connection is established ->
      {
        print("ACTIVE | ");
        // LED should be on
        MxLed_Off(true);
        MxLed_On(true);
      }
      if(iTxState & RM2M_TX_STARTED)        // If the connection establishment has started ->
      {
        print("STARTED | ");
        // LED should flicker	
        MxLed_Off(true);
        MxLed_Flicker(0);
      }
      if(iTxState & RM2M_TX_RETRY)          // If the system is waiting for the next automatic retry
      {
        print("RETRY | ");
        // LED should flicker	
        MxLed_Off(true);
        MxLed_Flicker(0);
      }
      // If the modem is logged into the GSM network AND (the connection is not active, has not started and is not waiting for next automatic retry) ->
      if((iTxState & RM2M_TX_WAKEUPABLE) && !(iTxState & (RM2M_TX_RETRY + RM2M_TX_STARTED + RM2M_TX_ACTIVE)))
      {
        print("WAKEUPABLE | ");
        MxLed_Off(true);                    // Switches off LED		
      }
      print("\r\n");
    }
    iTxStateAkt = iTxState;                 // Copies current connection status to the variable for the last determined connection status 
  }
}

