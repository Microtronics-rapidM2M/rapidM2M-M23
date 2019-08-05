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
 * Simple "LED" Example
 *
 * Toggles LED2 every second and turns off all other LEDs
 *
 * Only compatible with rapidM2M M2xx and rapidM2M M3
 * Special hardware circuit necessary
 * Recommendation: use rapidM2M Base Starter
 *
 * @version 20190508  
 */

/* Pin configuration */
const
{
  PIN_LED1_R = 1,                           // RGB LED 1: Red 	(GPIO1 is used)
  PIN_LED1_G = 2,                           // RGB LED 1: Green	(GPIO2 is used)
  PIN_LED1_B = 3,                           // RGB LED 1: Blue	(GPIO3 is used)
  PIN_LED2   = 0,                           // LED 2: Green		(GPIO0 is used)
  PIN_LED3   = 4,                           // LED 3: Green		(GPIO4 is used)
  
  LED_ENABLE  = RM2M_GPIO_HIGH,				// By setting the GPIO to "high" the LED is turned on
  LED_DISABLE = RM2M_GPIO_LOW,				// By setting the GPIO to "low" the LED is turned off
};


/* Forward declarations of public functions */
forward public MainTimer();                 // Called up 1x per sec. for the program sequence

/* Global variables declarations */
static iLedState;                           // Current state of LED 2 (1=^ On; x=^Off)

/* Application entry point */
main()
{
 /* Temporary memory for the index of a public function and the return value of a function  */
  new iIdx, iResult;
  
  /* Initialisation of a 1 sec. timer used for the general program sequence
     - Determining the function index that should be executed 1x per sec.
     - Transferring the index to the system
     - In case of a problem the index and return value of the init function are issued by the console  */
  iIdx = funcidx("MainTimer");
  iResult = rM2M_TimerAdd(iIdx);
  if(iResult < OK)
    printf("rM2M_TimerAddExt(%d) = %d\r\n", iIdx, iResult);

	
  /* Sets signal direction for GPIOs used to control LEDs to "Output" and turns off all LEDs */
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
}

/* 1 sec. timer is used for the general program sequence */
public MainTimer()
{  
  if(iLedState)								// If LED 2 is currently "On" ->
  { 
    rM2M_GpioSet(PIN_LED2, LED_DISABLE);    // Turns off LED 2
    printf("[LED] off\r\n");
  }
  else                                      // Otherwise (i.e. LED 2 is off)
  { 
    rM2M_GpioSet(PIN_LED2, LED_ENABLE);     // Turns on LED 2
    printf("[LED] on\r\n");
  }
  
  // Change state of LED
  iLedState = !iLedState;                   // Toggles the variable which holds the current state of LED 2
}

