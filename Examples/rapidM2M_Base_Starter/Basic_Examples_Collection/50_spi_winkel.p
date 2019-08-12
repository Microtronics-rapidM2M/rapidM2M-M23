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
 * Extended "SPI, g-forces (LIS3DSH)" example
 *
 * Uses the SPI interface to communicate with a LIS3DSH accelerometer.
 * The LIS3DSH is first configured to periodically perform a measurement with an output data rate (ODR) of 6.25 Hz.
 * A 100ms timer is then used to read out the measurement values and add up these sample measurements of the g-forces [mg]. 
 * Another 1 sec. timer is used to average the sample measurements and calculate the angle of the axes in relation to 
 * the surface of the earth. The calculated angle of the 3 axes are then issued via the console. 
 *
 * Only compatible with rapidM2M Base Starter
 * 
 *
 * @version 20190730  
 */

 /* Path for hardware-specific include file */
#include ".\rapidM2M M2\mx.inc"
#include "lis3dsh.inc"

/* Forward declarations of public functions */
forward public TimerMain();                      // Called up 1x per sec. to average the sample measurements and 
                                                 // Calculates the angle of the axes in relation to the surface of the earth 
forward public TimerFast();                      // Called up every 100ms to read the g-forces from the LIS3DSH and add up these sample measurements
forward public TimerInit();                      // Single shot timer to initialise the SPI interface and the accelerometer
                                                 // Called up in different intervals until the initialisation is done    
							 
const
{
  PIN_LED1_R = 1,                                // RGB LED 1: Red 	(GPIO1 is used)
  PIN_LED1_G = 2,                                // RGB LED 1: Green(GPIO2 is used)
  PIN_LED1_B = 3,                                // RGB LED 1: Blue	(GPIO3 is used)
  PIN_LED2   = 0,                                // LED 2: Green	(GPIO0 is used)
  PIN_LED3   = 4,                                // LED 3: Green	(GPIO4 is used)
  PIN_CS_ACC = 5,                                // CS signal for accelerometer
  
  LED_ENABLE  = RM2M_GPIO_HIGH,                  // By setting the GPIO to "high" the LED is turned on
  LED_DISABLE = RM2M_GPIO_LOW,				     // By setting the GPIO to "low" the LED is turned off
  
  PORT_SPI = 0,                                  // The fist SPI interface should be used
  
  /* State of the init process for the accelerometer */
  INIT_START = 0,                                // Initialisation started
  INIT_LIS3DSH,                                  // Config accelerometer
  INIT_FINISHED,                                 // Accelerometer ready 
  
  VALUE_MAX=1030,                                // G-force equivalent to an angle of 90° [mg]  
}

/* Handles for available sensors */
static hAcc[TLIS3DSH_Handle];                    // Handle for the LIS3DSH (accelerometer)
static iStateInit;                               // State of the init process for the accelerometer 
static iXsum, iYsum, iZsum;                      // Variables to add up the sample measurements for the g-forces of the 3 axes
static iCount;									 // Number of sample measurements performed

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
    printf("[MAIN] rM2M_TimerAdd(%d) = %d\r\n", iIdx, iResult);

  /* You could also refrain from setting the iStateInit to 0 (INIT_START), as all variables in 
	 PAWN are initialised with 0 at the moment of their creation. However, it was completed at this 
	 point for the  purpose of a better understanding.                                        */
  iStateInit = INIT_START;                       // Sets the state of the init process to "Initialization started"
	
  InitHandler();                                 // Starts the initialisation handler  
}

/* Single shot ms timer to initialise the SPI interface and the accelerometer
   The "InitHandler" function restarts the timer again until the initialisation is done         */
public TimerInit()
{
  InitHandler();                                 // Initialises the SPI interface and the accelerometer
}

/* Initialises the SPI interface and the accelerometer
   While the initialisation is not finished, this function starts a single shot timer which 
   calls it up again.                                                                           */ 
InitHandler()
{
  new iTimeout = 0;                              // Interval of the timer used for the initialisation [ms]
  
  /* Temporary memory for the index of a public function and the return value of a function     */
  new iIdx;
  new iResult;

  if(iStateInit == INIT_START)                   // If the state of the init process is "Initialization started" ->
  {
    /* Sets signal direction for GPIOs used to control LEDs to "Output" and turns off all LEDs
       Note: It is recommended to set the desired output level of a GPIO before setting the 
             signal direction for the GPIO.                                                     */
    rM2M_GpioSet(PIN_LED1_R, LED_DISABLE);	     // Sets the output level of the GPIO to "low"( LED 1: Red is off)
    rM2M_GpioDir(PIN_LED1_R, RM2M_GPIO_OUTPUT);  // Sets the signal direction for the GPIO used to control LED 1: Red
    rM2M_GpioSet(PIN_LED1_G, LED_DISABLE);
    rM2M_GpioDir(PIN_LED1_G, RM2M_GPIO_OUTPUT);
    rM2M_GpioSet(PIN_LED1_B, LED_DISABLE);
    rM2M_GpioDir(PIN_LED1_B, RM2M_GPIO_OUTPUT);
    rM2M_GpioSet(PIN_LED2, LED_DISABLE);
    rM2M_GpioDir(PIN_LED2, RM2M_GPIO_OUTPUT);
    rM2M_GpioSet(PIN_LED3, LED_DISABLE);
    rM2M_GpioDir(PIN_LED3, RM2M_GPIO_OUTPUT); 

    iTimeout = 1;                                // Sets the init timer to 1ms so that the InitHandler is called up again immediately
    iStateInit = INIT_LIS3DSH;                   // Sets the state of the init process to "Config accelerometer"
  }
  else if(iStateInit == INIT_LIS3DSH)            // Otherwise if the state of the init process is "Config accelerometer" ->
  {
    // Init SPI interface for communication with the LIS3DSH (accelerometer) and issues an error message if interface could not be initialised 
    iResult = rM2M_SpiInit(PORT_SPI, LIS3DSH_SPICLK, LIS3DSH_SPIMODE);
    if(iResult < OK)
      printf("[INIT] rM2M_SpiInit() = %d\r\n", iResult);
      
    iResult = LIS3DSH_Init(hAcc, PIN_CS_ACC, PORT_SPI);// Inits LIS3DSH accelerometer
    if(iResult >= OK)                                  // If the initialisation was successful ->
    {
      // Enables X, Y, Z axes and sets output data rate (ODR) to 6.25 Hz and check if it was sucessful
      iResult = LIS3DSH_Write(hAcc, LIS3DSH_REG_CTRL4, LIS3DSH_ODR_6HZ25|0x7);
      if(iResult >= OK)
      {
        printf("[INIT] LIS3DSH OK\r\n");
      } 
      else 
      {
        printf("[INIT] LIS3DSH_Init failed %d\r\n", iResult);
      }
    }
    else                                               // Otherwise ->  Issues an error message via the console
    {
      printf("[INIT] LIS3DSH_Init failed  %d\r\n", iResult);
    }
    
    rM2M_SpiClose(hAcc.spi);                           // Closes the SPI interface 
    
	/* Initialisation of a 100ms timer used to perform the sample measurements and add them up 
     - Determining the function index that should be executed every 100ms
     - Transferring the index to the system and informing it that the timer should be restarted 
	   following expiry of the interval
     - In case of a problem the index and return value of the init function are issued by the console  */
    iIdx = funcidx("TimerFast");
    iResult = rM2M_TimerAddExt(iIdx, true, 100);
    if(iResult < OK)
      printf("[INIT] rM2M_TimerAddExt(%d) = %d\r\n", iIdx, iResult);

    iStateInit = INIT_FINISHED;                        // Sets the state of the init process to "Accelerometer ready"
  }

  if(iTimeout > 0)                                     // If the init timer should be started ->
  {
  	/* Starts the init timer. If the timer expires, the "InitHandler" function is called up again by the 
       function transferred.
    - Determining the function index that should be called up when the timer expires and informing it 
	  that the timer should be stopped following expiry of the interval (single shot timer)
    - Transferring the index to the system                                                              
	- If the timer could not be started, the error message is issued via the console                     */   
    iIdx = funcidx("TimerInit");
    iResult = rM2M_TimerAddExt(iIdx, false, iTimeout);
    if(iResult < OK)
      printf("[INIT] rM2M_TimerAddExt failed with %d\r\n", iResult);
  }
}

/* 1 sec. timer is used to average the sample measurements and calculate the angle of the axes in relation to the surface of the earth */
public TimerMain()
{  
  if(iStateInit == INIT_FINISHED)                      // If the state of the init process is "Accelerometer ready" 
  {
    new iAngleX, iAngleY, iAngleZ;                     // Temporary memory for the calculated angle of the axes in relation to the surface of the earth
    new iX, iY, iZ;                                    // Temporary memory for the averaged values of the g-forces of the axes
    
    if(iCount > 0)                                     // If at least 1 sample measurement was performed -> 
    {
      //Calculates the average of the sample measurements
      iX = iXsum / iCount;
      iY = iYsum / iCount;
      iZ = iZsum / iCount;
      
	  //Calculates the angles of the axes in relation to the surface of the earth
      iAngleX = CalcAngle(iX, VALUE_MAX);
      iAngleY = CalcAngle(iY, VALUE_MAX);
      iAngleZ = CalcAngle(iZ, VALUE_MAX);
      
      //Resets the variables for adding up the sample measurements and the counter for the number of sample measurements performed
	  iXsum = 0;
      iYsum = 0;
      iZsum = 0;
      iCount = 0;
      
	  // Issues the averaged g-forces and calculated angles via the console
      printf("[LIS3DSH] X:%d (%d) Y:%d (%d) Z:%d (%d)\r\n", iX, iAngleX, iY, iAngleY, iZ, iAngleZ);
    }
  }
}

/* 100ms timer is used to read the g-forces from the LIS3DSH and add up these sample measurements */
public TimerFast()
{
  new iResult;                                         // Temporary memory for the return value of a function
  
  new iX, iY, iZ;                                      // Temporary memory for g-forces read from the LIS3DSH (accelerometer)
  
  // Inits SPI interface for communication with the LIS3DSH (accelerometer) and issues an error message if interface could not be initialised
  iResult = rM2M_SpiInit(hAcc.spi, LIS3DSH_SPICLK, LIS3DSH_SPIMODE);
  if(iResult < OK)
    printf("[MAIN] rM2M_SpiInit() = %d\r\n", iResult);
  
  iResult = LIS3DSH_ReadMeasurement(hAcc, iX, iY, iZ)            // Reads the g-forces [mg] for all 3 axes from the LIS3DSH (accelerometer)
  if(iResult < OK)
    printf("[MAIN] LIS3DSH_ReadMeasurement() = %d\r\n", iResult);

  iXsum += iX;                                         // Adds up the read g-force of the x axis 
  iYsum += iY;                                         // Adds up the read g-force of the y axis 
  iZsum += iZ;                                         // Adds up the read g-force of the z axis 
  iCount++;                                            // Increases the number of sample measurements performed
  
  rM2M_SpiClose(hAcc.spi);                             // Closes the SPI interface
}

 /**
 * Calculates the angle of the axis in relation to the surface of the earth
 *
 * @param iValue:s32    - g-force [mg] measured for an axis  
 * @param iMaxValue:s32 - g-force equivalent to an angle of 90° [mg]
 * @return s32          - angle of the axis in relation to the surface of the earth
 */                        
stock CalcAngle(iValue, iMaxValue)
{
  new Float:fValue;
  new iResult;
  
  if(iValue > iMaxValue)                               // If the measured g-force for the axis is higher than the g-force equivalent to an angle of 90° 
    return 90;                                         // Return 90°  														
  
  if(iValue < -iMaxValue)                              // If the measured g-force for the axis is lower than the g-force equivalent to an angle of -90° 
    return -90;                                        // Return -90°
  
  // Calculates the angle of the axis in relation to the surface of the earth and return it
  fValue = 90-(acos(float(iValue) / float(iMaxValue)) * 180.0 / M_PI);
  iResult = fValue;
  return iResult;
}