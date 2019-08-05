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
 * "RGB LED" Example V1
 *
 * Changes the color of the RGB LED 1 each second. To do this, a counter is repeatedly increased 
 * from 0 to 7. Each bit of the counter is assigned a color of the RGB LED1.
 *
 * Bit0 = Red
 * Bit1 = Green
 * Bit2 = Blue
 *
 * This results in the following color sequence: Off - Red - Green - Yellow - Blue - Magenta - Cyan - White
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

/* Bit mask for the LED state */
const
{
  LED1_R_MSK = 0x01,                        //Bit indicating the state of the red part of the RGB LED   (0^=Off; 1=^On)
  LED1_G_MSK = 0x02,                        //Bit indicating the state of the green part of the RGB LED (0^=Off; 1=^On)
  LED1_B_MSK = 0x04,                        //Bit indicating the state of the blue part of the RGB LED  (0^=Off; 1=^On)
}

/* Forward declarations of public functions */
forward public MainTimer();                 // Called up 1x per sec. for the program sequence

/* Global variables declarations */
static iLedState;                           // Current LED state (counts from 0 to 7)

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
  rM2M_GpioSet(PIN_LED1_R, LED_DISABLE);       // Sets the output level of the GPIO to "low"( LED 1: Red is off)
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
  printf("[LED] State: %d\r\n", iLedState);    // Issues the current LED state via the console
  
  if(iLedState & LED1_R_MSK)                   // If the bit for the red part of the RGB LED is set ->
  {
    rM2M_GpioSet(PIN_LED1_R, LED_ENABLE);	   // Turns on red part of the RGB LED	
  }
  else                                         // Otherwise (bit not set) ->
  {
    rM2M_GpioSet(PIN_LED1_R, LED_DISABLE);     // Turns off red part of the RGB LED	
  }

  if(iLedState & LED1_G_MSK)                   // If the bit for the green part of the RGB LED is set ->
  {
    rM2M_GpioSet(PIN_LED1_G, LED_ENABLE);      // Turns on green part of the RGB LED	
  }
  else                                         // Otherwise (bit not set) ->
  {
    rM2M_GpioSet(PIN_LED1_G, LED_DISABLE);     // Turns off green part of the RGB LED	
  }

  if(iLedState & LED1_B_MSK)                   // If the bit for the blue part of the RGB LED is set ->
  {
    rM2M_GpioSet(PIN_LED1_B, LED_ENABLE);      // Turns on blue part of the RGB LED	
  }
  else                                         // Otherwise (bit not set) ->
  {
    rM2M_GpioSet(PIN_LED1_B, LED_DISABLE);     // Turns off blue part of the RGB LED
  }  
  
  // Increases counter for the LED state. It is ensured that the counter reading does not exceed 7.
  iLedState = (iLedState + 1) & (LED1_R_MSK|LED1_G_MSK|LED1_B_MSK);
}

