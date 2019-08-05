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
 * Extended "Indicating the transmission state" Example
 *
 * Initiates a connection to the server and uses status LED to indicate the current connection state
 * Status LED is off                     if the device is offline (not connected)
 * Status LED is on                      if the device is connected to the server 
 * Status LED flickers 	                 if the connection establishment has started or the device is waiting for the next automatic retry
 * Status LED blinks 1x every 3 sec.     if the device is in "Wakeup" mode
 * Status LED blinks 2x every 3 sec.     if the connection establishment has failed
 * 
 * Note: The external LED must be connected between 3V3(<= 250mA) and LED status.
 * 
 * Only compatible with rapidM2M M2xx
 * Special hardware circuit necessary
 * Recommendation: use rapidM2M Base Starter
 *
 * @version 20190703  
 */

/*
 * TX_STATE STARTED     Flicker
 * TX_STATE RETRY       Flicker
 * TX_STATE ACTIVE      Constantly on
 * TX_STATE WAKEUPABLE  1x every 3 seconds
 * TX_STATE NONE        Off
 * TX_STATE FAILED      2x every 3 seconds
 */

 /* Path for hardware-specific include file */
#include ".\rapidM2M M2\mx.inc"

/* Forward declarations of public functions */
forward public TimerMain();                 // Called up 1x per sec. for the program sequence

/* Pin configuration */
const
{  
  PIN_LED1_R  = 1,                          // RGB LED 1: Red 	(GPIO1 is used)
  PIN_LED1_G  = 2,                          // RGB LED 1: Green	(GPIO2 is used)
  PIN_LED1_B  = 3,                          // RGB LED 1: Blue	(GPIO3 is used)
  PIN_LED2    = 0,                          // LED 2: Green		(GPIO0 is used)
  PIN_LED3    = 4,                          // LED 3: Green		(GPIO4 is used)
  
  LED_DISABLE = RM2M_GPIO_LOW,              // By setting the GPIO to "low", the LED is turned off.
  
  //State that can be displayed via status LED
  STATE_OFF   = 0,                          // Device is offline (not connected)       (Status LED off)	
  STATE_ON,                                 // Device is connected to the server       (Status LED on)
  STATE_SLOW,                               // Device is in "Wakeup" mode              (Status LED blinking 1x every 3 seconds)
  STATE_FAST,                               // Connection establishment has started or 
                                            // is waiting for the next automatic retry (Status LED flickering)
  STATE_ERROR,                              // Connection establishment failed         (Status LED blinking 2x every 3 seconds)                                            
};

/* Global variables declarations */
static iState     = STATE_OFF;              // Current state that should be displayed via the status LED
static iTimerLed  = -1;                     // Counter used to control the status LED

/* Application entry point */
main()
{
 /* Temporary memory for the index of a public function and the return value of a function  */
  new iIdx, iResult;
  
  /* Initialisation of a 1 sec. timer used for the general program sequence
     - Determining the function index that should be executed 1x per sec.
     - Transferring the index to the system
	 - In case of a problem the index and return value of the init function are issued by the console  */
  iIdx = funcidx("TimerMain");
  iResult = rM2M_TimerAdd(iIdx);
  if(iResult < OK)
    printf("rM2M_TimerAdd(%d) = %d\r\n", iIdx, iResult);
  
  /* Initialisation of the status LED -> control via script activated */ 
  iResult = MxLed_Init(MX_LED_MODE_SCRIPT);
  printf("MxLed_Init() = %d\r\n", iResult);
  
  MxLed_Off(true);                             // Switches off status LED
  
  /* Sets signal direction for GPIOs used to control LEDs to "Output" and turn off all LEDs */
  /* Note: It is recommended to set the desired output level of a GPIO before setting the 
           signal direction for the GPIO.                                                     */
  rM2M_GpioSet(PIN_LED1_R, LED_DISABLE);       // Sets the output level of the GPIO to "low" (LED 1: Red is off)
  rM2M_GpioDir(PIN_LED1_R, RM2M_GPIO_OUTPUT);  // Sets the signal direction for the GPIO used to control LED 1: Red 
  rM2M_GpioSet(PIN_LED1_G, LED_DISABLE);
  rM2M_GpioDir(PIN_LED1_G, RM2M_GPIO_OUTPUT);
  rM2M_GpioSet(PIN_LED1_B, LED_DISABLE);
  rM2M_GpioDir(PIN_LED1_B, RM2M_GPIO_OUTPUT);
  rM2M_GpioSet(PIN_LED2, LED_DISABLE);
  rM2M_GpioDir(PIN_LED2, RM2M_GPIO_OUTPUT);
  rM2M_GpioSet(PIN_LED3, LED_DISABLE);
  rM2M_GpioDir(PIN_LED3, RM2M_GPIO_OUTPUT);
  
  // rM2M_TxSelectItf(RM2M_TXITF_MODEM);       // (Alternative) Selects the mobile network modem as the communication interface
  // rM2M_TxSetMode(RM2M_TXMODE_WAKEUP);       // (Alternative) Sets the connection type to "Wakeup" mode
  
  rM2M_TxSelectItf(RM2M_TXITF_WIFI);           // Selects the WiFi module as the communication interface
  rM2M_TxSetMode(RM2M_TXMODE_TRIG);            // Sets the connection type to "Interval" mode  

  rM2M_TxStart();                              // Initiates a connection to the server
}

/* 1 sec. timer is used for the general program sequence */
public TimerMain()
{
  HandleTxState();                             // Checks the connection state and controls the state that should be displayed via the status LED 
  HandleLed();                                 // Control of the status LED
}


/* Determines the current connection state and changes the state that should be displayed via the status LED if necessary. */
HandleTxState()
{
  new iTxState;                                // Temporary memory for the current connection status
  static iTxStateAkt;                          // Last determined connection status 
 
  iTxState = rM2M_TxGetStatus();               // Gets the current connection status
  
  if(iTxState != iTxStateAkt)                  // If the connection status has changed ->  
  {
    if(iTxState == 0)                          // If the connection is not active, has not started and is not waiting for next automatic retry 
    {
      print("iTxState: ---\r\n");
      ChangeState(STATE_OFF);                  // Status LED should be off 
    }
    else                                       // Otherwise ->
    {
      print("iTxState: ");
      if(iTxState & RM2M_TX_FAILED)            // If the connection establishment failed -> 
      {
        print("FAILED | ");
        ChangeState(STATE_ERROR);              // Status LED should blink 2x slowly every 3 seconds
      }
      if(iTxState & RM2M_TX_ACTIVE)            // If the GPRS connection is established -> 
      {
        print("ACTIVE | ");
        ChangeState(STATE_ON);                 // Status LED should be on
      }
      if(iTxState & RM2M_TX_STARTED)           // If the connection establishment has started ->
      {
        print("STARTED | ");
        ChangeState(STATE_FAST);               // Status LED should flicker
      }
      if(iTxState & RM2M_TX_RETRY)             // If the system is waiting for the next automatic retry
      {
        print("RETRY | ");
        ChangeState(STATE_FAST);               // Status LED should flicker
      }
	  // If the modem is logged into the GSM network AND (the connection is not active, has not started and is not waiting for next automatic retry) ->
      if((iTxState & RM2M_TX_WAKEUPABLE) && !(iTxState & (RM2M_TX_RETRY + RM2M_TX_STARTED + RM2M_TX_ACTIVE)))
      {
        print("WAKEUPABLE | ");
        ChangeState(STATE_SLOW);               // Status LED should blink 1x slowly every 3 seconds
      }
      print("\r\n");
    }
    iTxStateAkt = iTxState;                    // Copies current connection status to the variable for the last determined connection status 
  }
}

/**
 * Changes the state that should be displayed via the status LED 
 *
 * @param iNewState:s32 - New state that should be displayed via the status LED
 */
ChangeState(iNewState)
{
  if(iNewState != iState)                      // If the status to be set differs from the current status ->
  {
    iTimerLed = 1;                             // Sets the counter used to control the status LED to "1" so that the "HandleLed" function 
                                               // can change the mode of the status LED
    
    iState = iNewState;                        // Copies new status to the variable for the current state that should be displayed via the status LED 
  }
}

/* Function to control the status LED */
HandleLed()
{
  if(iTimerLed > 0)                            // If the counter used to control the status LED has not expired ->
  {
    iTimerLed--;                               // Decrements the counter used to control the status LED
    if(iTimerLed == 0)                         // If the counter used to control the status LED has expired ->
    {
      switch(iState)                           // Switches current state that should be displayed via the status LED 
      {
        case STATE_OFF:
        {
          MxLed_Off(true);                     // Switches off status LED
        }
        case STATE_ON:
        {
          MxLed_On(true);                      // Switches on status LED
        }
        case STATE_SLOW:
        {
          MxLed_Off(true);
          MxLed_Blink(1);                      // Makes the status LED blink 1x
          iTimerLed = 3;                       // Sets the counter used to control the status LED to "3" so that the "HandleLed" function is called up again after 3 seconds
        }
        case STATE_FAST:
        {
          MxLed_Off(true);
          MxLed_Flicker(0);                    // Makes the status LED flicker until another MxLed_xxx function is called
        }
        case STATE_ERROR:
        {
          MxLed_Off(true);
          MxLed_Blink(2);                      // Makes the status LED blink 2x
          iTimerLed = 3;                       // Sets the counter used to control the status LED to "3" so that the "HandleLed" function is called up again after 3 seconds
        }
      }
    }
  }
}
