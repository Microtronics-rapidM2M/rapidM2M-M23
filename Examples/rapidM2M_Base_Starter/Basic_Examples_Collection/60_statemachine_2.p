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
 * Extended "State machine" example
 *
 * The state machine has seven different states that are indicated by the RGB LED 1:
 * - STATE_OFF               RGB LED 1 is off for 1 sec.
 * - STATE_RED               RGB LED 1 lights up red for 5 sec.
 * - STATE_RED_YELLOW        RGB LED 1 lights up yellow for 2 sec. 
 * - STATE_GREEN             RGB LED 1 lights up green for 5 sec.
 * - TIMEOUT_GREEN_BLINKING  RGB LED 1 blinks green for 3 sec. (ton=1sec. toff=1sec.)
 * - STATE_YELLOW            RGB LED 1 lights up yellow for 2 sec.
 *
 * When turned on, the state machine is in state "STATE_OFF" for one sec. 
 * Then it switches to state "STATE_RED" for 5 seconds.
 * Then it switches to state "STATE_RED_YELLOW" for 2 seconds.
 * Then it switches to state "STATE_GREEN" for 5 seconds.
 * Then it switches to state "TIMEOUT_GREEN_BLINKING" for 3 seconds.
 * Then it switches to state "STATE_YELLOW" for 2 seconds.
 * After that, the sequence just described is continuously repeated starting with the state "STATE_RED".
 * 
 * Note: The state "STATE_OFF" is only used once at the PowerOn.
 *
 
 * Only compatible with rapidM2M M2xx and rapidM2M M3
 * Special hardware circuit necessary
 * Recommendation: use rapidM2M Base Starter
 *
 * @version 20190508  
 */

/* Forward declarations of public functions */
forward public TimerMain();                 // Called up 1x per sec. for the program sequence

const
{
  PIN_LED1_R = 1,                           // RGB LED 1: Red 	(GPIO1 is used)
  PIN_LED1_G = 2,                           // RGB LED 1: Green	(GPIO2 is used)
  PIN_LED1_B = 3,                           // RGB LED 1: Blue	(GPIO3 is used)
  PIN_LED2   = 0,                           // LED 2: Green		(GPIO0 is used)
  PIN_LED3   = 4,                           // LED 3: Green		(GPIO4 is used)
  
  LED_ENABLE  = RM2M_GPIO_HIGH,				// By setting the GPIO to "high" the LED is turned on
  LED_DISABLE = RM2M_GPIO_LOW,				// By setting the GPIO to "low" the LED is turned off  
}

// Possible states for the application 
const
{
  STATE_OFF = 0,
  STATE_RED,
  STATE_RED_YELLOW,
  STATE_GREEN,
  STATE_GREEN_BLINKING,
  STATE_YELLOW,
}

// Delay before switching to the next state [sec.]
const
{
  TIMEOUT_OFF              = 1,
  TIMEOUT_RED              = 5,
  TIMEOUT_RED_YELLOW       = 2,
  TIMEOUT_GREEN            = 5,
  TIMEOUT_GREEN_BLINKING   = 3,
  TIMEOUT_YELLOW           = 2,
}

static iAppState;                           // Current state of the application

static iStateRed;                           // Current state of the GPIO used to control the red part of the RGB LED
static iStateGreen;                         // Current state of the GPIO used to control the green part of the RGB LED
static iStateBlue;                          // Current state of the GPIO used to control the blue part of the RGB LED

static iTimerTimeout;                       // Remaining time until switching to the next state  

/* Application entry point */
main()
{
  /* Temporary memory for the index of a public function and the return value of a function  */
  new iIdx, iResult;

  /* Sets signal direction for GPIOs used to control LEDs to "Output" and turn off all LEDs */
  /* Note: It is recommended to set the desired output level of a GPIO before setting the 
           signal direction for the GPIO.                                                     */
  rM2M_GpioSet(PIN_LED1_R, LED_DISABLE);	   // Sets the output level of the GPIO to "low"( LED 1: Red is off)
  rM2M_GpioDir(PIN_LED1_R, RM2M_GPIO_OUTPUT);  // Sets the signal direction for the GPIO used to control LED 1: Red 
  rM2M_GpioSet(PIN_LED1_G, LED_DISABLE);
  rM2M_GpioDir(PIN_LED1_G, RM2M_GPIO_OUTPUT);
  rM2M_GpioSet(PIN_LED1_B, LED_DISABLE);
  rM2M_GpioDir(PIN_LED1_B, RM2M_GPIO_OUTPUT);
  rM2M_GpioSet(PIN_LED2, LED_DISABLE);
  rM2M_GpioDir(PIN_LED2, RM2M_GPIO_OUTPUT);
  rM2M_GpioSet(PIN_LED3, LED_DISABLE);
  rM2M_GpioDir(PIN_LED3, RM2M_GPIO_OUTPUT); 

  /* Initialisation of a 1 sec. timer used for the general program sequence
     - Determining the function index that should be executed 1x per sec.
     - Transferring the index to the system
     - In case of a problem the index and return value of the init function are issued by the console  */
  iIdx = funcidx("TimerMain");
  iResult = rM2M_TimerAdd(iIdx);
  printf("rM2M_TimerAdd(%d) = %d\r\n", iIdx, iResult);
  
  // Init state
  StateChange(STATE_OFF);                      // Calls function to set the state of the application to "STATE_OFF"
}

 /**
 * Changes the state of the application
 *
 * @param iNewState:s32    - New state for the application 
 */
StateChange(iNewState)
{
  switch(iNewState)                            // Switch new state for the application
  {
    case STATE_OFF:
    {
      // Sets the state of the GPIOs used to control RGB LED in such a way that the LED is turned off
      iStateRed     = LED_DISABLE;
      iStateGreen   = LED_DISABLE;
      iStateBlue    = LED_DISABLE;
      
      // Sets the counter for the remaining time until switching to the next state to "TIMEOUT_OFF"
      Handle_CheckWait(iTimerTimeout, TIMEOUT_OFF);
    }
    case STATE_RED:
    {
      // Sets the state of the GPIOs used to control RGB LED in such a way that the LED lights up red
      iStateRed     = LED_ENABLE;
      iStateGreen   = LED_DISABLE;
      iStateBlue    = LED_DISABLE;

      // Sets the counter for the remaining time until switching to the next state to "TIMEOUT_RED"
      Handle_CheckWait(iTimerTimeout, TIMEOUT_RED);
    }
    case STATE_RED_YELLOW:
    {
      // Sets the state of the GPIOs used to control RGB LED in such a way that the LED lights up yellow	
      iStateRed     = LED_ENABLE;
      iStateGreen   = LED_ENABLE;
      iStateBlue    = LED_DISABLE;

      // Sets the counter for the remaining time until switching to the next state to "TIMEOUT_RED_YELLOW"
      Handle_CheckWait(iTimerTimeout, TIMEOUT_RED_YELLOW);
    }
    case STATE_GREEN:
    {
      // Sets the state of the GPIOs used to control RGB LED in such a way that the LED lights up green
      iStateRed     = LED_DISABLE;
      iStateGreen   = LED_ENABLE;
      iStateBlue    = LED_DISABLE;

      // Sets the counter for the remaining time until switching to the next state to "TIMEOUT_GREEN"
      Handle_CheckWait(iTimerTimeout, TIMEOUT_GREEN);
    }
    case STATE_GREEN_BLINKING:
    {
      // Sets the state of the GPIOs used to control RGB LED in such a way that the LED is turned off (blinking starts with LED off) 
      iStateRed     = LED_DISABLE;
      iStateGreen   = LED_DISABLE;
      iStateBlue    = LED_DISABLE;

	  //Sets the counter for the remaining time until switching to the next state to "TIMEOUT_GREEN_BLINKING"  
      Handle_CheckWait(iTimerTimeout, TIMEOUT_GREEN_BLINKING);
    }
    case STATE_YELLOW:
    {
      // Sets the state of the GPIOs used to control RGB LED in such a way that the LED lights up yellow		
      iStateRed     = LED_ENABLE;
      iStateGreen   = LED_ENABLE;
      iStateBlue    = LED_DISABLE;
      
      // Sets the counter for the remaining time until switching to the next state to "TIMEOUT_YELLOW"
      Handle_CheckWait(iTimerTimeout, TIMEOUT_YELLOW);
    }
  }
  
  iAppState = iNewState;                       // Copies the new state to the variable for the current state of the application 
}

/* Function that manages the switching between the different application states */ 
StateHandle()
{
  switch(iAppState)                            // Switch current state of the application  
  {
    case STATE_OFF:
    {
      if(Handle_CheckWait(iTimerTimeout))      // If the remaining time until switching to the next state has expired ->  
        StateChange(STATE_RED);                // Calls function to set the state of the application to "STATE_RED"   
    }
    case STATE_RED:
    {
      if(Handle_CheckWait(iTimerTimeout))      // If the remaining time until switching to the next state has expired ->
        StateChange(STATE_RED_YELLOW);         // Calls function to set the state of the application to "STATE_RED_YELLOW"
    }
    case STATE_RED_YELLOW:
    {
      if(Handle_CheckWait(iTimerTimeout))      // If the remaining time until switching to the next state has expired ->
        StateChange(STATE_GREEN);              // Calls function to set the state of the application to "STATE_GREEN"   
    }
    case STATE_GREEN:
    {
      if(Handle_CheckWait(iTimerTimeout))      // If the remaining time until switching to the next state has expired ->
        StateChange(STATE_GREEN_BLINKING);     // Calls function to set the state of the application to "STATE_GREEN_BLINKING" 
    }
    case STATE_GREEN_BLINKING:
    {
      if(Handle_CheckWait(iTimerTimeout))      // If the remaining time until switching to the next state has expired ->
        StateChange(STATE_YELLOW);             // Calls function to set the state of the application to "STATE_YELLOW" 
      else                                     // Otherwise -> Toggles the state of the GPIO used to control the green part of the RGB LED (blinking) 
        iStateGreen = !iStateGreen; 
    }
    case STATE_YELLOW:
    {
      if(Handle_CheckWait(iTimerTimeout))      // If the remaining time until switching to the next state has expired ->
        StateChange(STATE_RED);                // Calls function to set the state of the application to "STATE_RED"   
    }
  }
}

 /**
 * Checks whether the remaining time until switching to the next state has expired.
 *
 * If the value for "iReset" (new remaining time until switching to the next state) is not 0, the transferred counter containing 
 * the remaining time until switching to the next state is set to "iReset". The return value is set to "False"
 *
 * If the value for "iReset" is 0, the transferred counter containing the remaining time until switching to the   
 * next state is decremented. If the time has expired, the return value is set to "True", otherwise it is set to "False"  
 * 
 * Note: Parameter "iTimer" is transferred as a reference. This means that changes to the value within  
 *       the function have an impact on the variable outside the function.
 *
 *
 * @param iTimer:s32 - Counter containing the remaining time until switching to the next state
 * @param iReset:s32 - New remaining time until switching to the next state (- OPTIONAL)
 * @return s32       - False: Time has NOT expired
 *                     True:  Time has expired       
 */
Handle_CheckWait(&iTimer, iReset=0)
{
  new iResult = false;                         // Temporary memory for the return value of the function is set to "False"
  
  if(iReset)                                   // If the transferred counter containing the remaining time should be set to a new value ->
  {
    iTimer = iReset;                           // Sets the counter containing the remaining time to the new value   
  }
  else if(iTimer > 0)                          // If the counter containing the remaining time has not expired ->
  {
    iTimer--;                                  // Decrements the counter containing the remaining time until switching to the next state
    if(iTimer == 0)                            // If the counter containing the remaining time has expired ->
    {
      iResult = true;                          // Sets the return value of the function to "True" 
    }
  }
  return iResult;                              // Return  
}

/* 1 sec. timer is used for the general program sequence */
public TimerMain()
{
  StateHandle();                               // Manages the switching between the different application states
  
  // Issues the current state of the application and the current states of the GPIOs used to control the RGB LED
  printf("%d: %d/%d/%d\r\n", iAppState, iStateRed, iStateGreen, iStateBlue);
  
  // Sets the GPIOs which controls the RGB LED to the determined level
  rM2M_GpioSet(PIN_LED1_R,  iStateRed);
  rM2M_GpioSet(PIN_LED1_G,  iStateGreen);
  rM2M_GpioSet(PIN_LED1_B,  iStateBlue);
  
}


