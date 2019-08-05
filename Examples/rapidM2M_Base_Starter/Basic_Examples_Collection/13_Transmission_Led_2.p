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
 * Initiates a connection to the server and uses LED 2 to indicate the current connection state
 * LED 2 is off                      if the device is offline (not connected)
 * LED 2 is on                       if the device is connected to the server 
 * LED 2 flickers                    if the connection establishment has started or the device is waiting for the next automatic retry
 * LED 2 blinks 1x every 3 sec.      if the device is in "Wakeup" mode
 * LED 2 blinks 2x every 3 sec.      if the connection establishment has failed
 *
 *
 * Only compatible with rapidM2M M2xx
 * Special hardware circuit necessary
 * Recommendation: use rapidM2M Base Starter
 *
 * @version 20190508  
 */

/*
 * TX_STATE STARTED     Flicker green
 * TX_STATE RETRY       Flicker green
 * TX_STATE ACTIVE      Constantly green on
 * TX_STATE WAKEUPABLE  1x green every 3 seconds
 * TX_STATE NONE        Off
 * TX_STATE FAILED      2x red every 3 seconds
 */

/* Forward declarations of public functions */
forward public TimerMain();                 // Called up 1x per sec. for the program sequence
forward public Timer100ms();                // Called up every 100ms to display the current state of the connection via LED 2 

/* Pin configuration */
const
{  
  PIN_LED1_R  = 1,                          // RGB LED 1: Red 	(GPIO1 is used)
  PIN_LED1_G  = 2,                          // RGB LED 1: Green	(GPIO2 is used)
  PIN_LED1_B  = 3,                          // RGB LED 1: Blue	(GPIO3 is used)
  PIN_LED2    = 0,                          // LED 2: Green		(GPIO0 is used)
  PIN_LED3    = 4,                          // LED 3: Green		(GPIO4 is used)
  
  LED_ENABLE  = RM2M_GPIO_HIGH,             // By setting the GPIO to "high", the LED is turned on
  LED_DISABLE = RM2M_GPIO_LOW,              // By setting the GPIO to "low", the LED is turned off
  
  //State that can be displayed via LED 2
  STATE_OFF   = 0,                          // Device is offline (not connected)       (LED 2 off)	
  STATE_ON,                                 // Device is connected to the server       (LED 2 on)
  STATE_SLOW,                               // Device is in "Wakeup" mode              (LED 2 blinking 1x every 3 seconds)
  STATE_FAST,                               // Connection establishment has started or 
                                            // is waiting for the next automatic retry (LED 2 flickering)
  STATE_ERROR,                              // Connection establishment failed         (RGB LED 1 (red) blinking 2x every 3 seconds)                                            
};


/* Global variables declarations */
static iState          = STATE_OFF;              // Current state that should be displayed via LED 2
static iTimer          = 0;                      // Counter used to generate the blink interval for LED 2 
static iStateGpioGREEN = 0;                      // Current state of the GPIO used to control LED 2 (green)
static iStateGpioRED   = 0;                      // Current state of the GPIO used to control RGB LED 1 (red)

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

  /* Initialisation of a 100ms timer used to display current state of the connection via LED 2
     - Determining the function index that should be executed every 100ms
     - Transferring the index to the system and informing it that the timer should be restarted 
	   following expiry of the interval
	 - In case of a problem the index and return value of the init function are issued by the console  */	
  iIdx = funcidx("Timer100ms");
  iResult = rM2M_TimerAddExt(iIdx, true, 100);
  if(iResult < OK)
    printf("rM2M_TimerAddExt(%d) = %d\r\n", iIdx, iResult);
  
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
  new iTxState;                                // Temporary memory for the current connection status
  static iTxStateAkt;                          // Last determined connection status 
  
  iTxState = rM2M_TxGetStatus();               // Gets the current connection status
  
  if(iTxState != iTxStateAkt)                  // If the connection status has changed ->  
  {
    if(iTxState == 0)                          // If the connection is not active, has not started and is not waiting for next automatic retry 
    {
      print("iTxState: ---\r\n");
      ChangeState(STATE_OFF);                  // LED 2 should be off 
    }
    else                                       // Otherwise ->
    {
      print("iTxState: ");
      if(iTxState & RM2M_TX_FAILED)            // If the connection establishment failed -> 
      {
        print("FAILED | ");
        ChangeState(STATE_ERROR);              // LED should blink 2x slowly every 3 seconds
      }
      if(iTxState & RM2M_TX_ACTIVE)            // If the GPRS connection is established -> 
      {
        print("ACTIVE | ");
        ChangeState(STATE_ON);                 // LED 2 should be on
      }
      if(iTxState & RM2M_TX_STARTED)           // If the connection establishment has started ->
      {
        print("STARTED | ");
        ChangeState(STATE_FAST);               // LED 2 should flicker
      }
      if(iTxState & RM2M_TX_RETRY)             // If the system is waiting for the next automatic retry
      {
        print("RETRY | ");
        ChangeState(STATE_FAST);               // LED 2 should flicker
      }
	  // If the modem is logged into the GSM network AND (the connection is not active, has not started and is not waiting for next automatic retry) ->
      if((iTxState & RM2M_TX_WAKEUPABLE) && !(iTxState & (RM2M_TX_RETRY + RM2M_TX_STARTED + RM2M_TX_ACTIVE)))
      {
        print("WAKEUPABLE | ");
        ChangeState(STATE_SLOW);               // LED should blink 1x slowly every 3 seconds
      }
      print("\r\n");
    }
    iTxStateAkt = iTxState;                    // Copies current connection status to the variable for the last determined connection status 
  }
}

/**
 * Changes the state that should be displayed via LED 2
 *
 * @param iNewState:s32 - New state that should be displayed via LED 2
 */
ChangeState(iNewState)
{
  if(iNewState != iState)                      // If the status to be set differs from the current status ->
  {
    // Update on next tick
    iTimer = 0;                                // Sets the counter used to generate the blink interval to 0 so that LED2 can be controlled 
                                               // on the next expiry of the 100ms
    iState = iNewState;                        // Copies new status to the variable for the current state that should be displayed via LED 2 
  }
}

/* 100ms timer used to display current state of the connection via LED 2 */
public Timer100ms()
{
  iTimer--;                                    // Decrements the counter used to generate the blink interval for LED 2  
  if(iTimer <= 0)                              // When the counter has expired ->
  {
    if(iState == STATE_OFF)                    // If LED 2 should be off ->	
    {
      iStateGpioRED = LED_DISABLE;             // Disables red LED
      // LED off
      iStateGpioGREEN = LED_DISABLE;           // Sets current state of the GPIO used to control LED 2 to "LED_DISABLE"  
      iTimer = 0;                              // Sets the counter used to generate the blink interval to 0 	  
    }
    else if (iState == STATE_ON)               // If LED 2 should light up ->
    {
      iStateGpioRED = LED_DISABLE;             // Disables red LED
      // LED on
      iStateGpioGREEN = LED_ENABLE;            // Sets current state of the GPIO used to control LED 2 to "LED_ENABLE" 
      iTimer = 0                               // Sets the counter used to generate the blink interval to 0 
    }
    else if (iState == STATE_FAST)             // If LED 2 should flicker ->
    {
      iStateGpioRED = LED_DISABLE;             // Disables red LED
      // 100ms on, 100ms off
      if(iStateGpioGREEN)                      // If the current state of the GPIO used to control LED 2 is "LED_ENABLE" -> sets it to "LED_DISABLE" 
        iStateGpioGREEN = LED_DISABLE;
      else                                     // Otherwise -> sets it to "LED_ENABLE"
        iStateGpioGREEN = LED_ENABLE;                
        
      iTimer = 0;                              // Sets the counter used to generate the blink interval to 0 
    }
    else if (iState == STATE_SLOW)             // If LED 2 should blink 1x every 3 seconds ->
    {
      iStateGpioRED = LED_DISABLE;             // Disables red LED
      // 1 second on, 1 second off
      if(iStateGpioGREEN)                      // If the current state of the GPIO used to control LED 2 is "LED_ENABLE"  -> sets it to "LED_DISABLE"
      {
        iStateGpioGREEN = LED_DISABLE;
        iTimer = 30;                           // Sets the counter used to generate the blink interval to 3 seconds
      }
      else                                     // Otherwise -> sets it to "LED_ENABLE"
      {
        iStateGpioGREEN = LED_ENABLE;
        iTimer = 5;                            // LED should be on for 1 second 
      }
    }
    else if (iState == STATE_ERROR)            // If RGB LED 1 (red) should blink 2x every 3 seconds ->
    {
      static iIdle = 3;                        // Counter decrements each time the LED is switched on/off
      iStateGpioGREEN = LED_DISABLE;           // Disables green LED
      if(iIdle < 1)                            // If counter reaches 0 ->
      {
        iStateGpioRED = LED_DISABLE;           // Disables red LED
        iIdle = 3;                             // Resets counter
        iTimer = 30;                           // Sets the counter used to generate the blink interval to 3 seconds
      }
      else if(iStateGpioRED)                   // If counter did not reach 0 yet and red LED is on
      {
        iStateGpioRED = LED_DISABLE;           // Disables red LED
        iIdle--;                               // Decrements counter
        iTimer = 5;                            // LED should be off for 500ms
      }
      else
      {
        iStateGpioRED = LED_ENABLE;            // Enables red LED
        iIdle--;                               // Decrements counter
        iTimer = 5;                            // LED should be on for 500ms 
      }
    }
    rM2M_GpioSet(PIN_LED2, iStateGpioGREEN);   // Sets the GPIO which controls the LED 2 to the detected level
    rM2M_GpioSet(PIN_LED1_R, iStateGpioRED);   // Sets the GPIO which controls the RGB LED 1 (red) to the detected level
  }
}
