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
 * Simple "I2C, Temperature and humidity (SHT31)" example
 *
 * Uses the I2C interface to communicate with a SHT31 temperature and humidity sensor.
 * The SH31 is first configured to periodically perform a measurement every second. A 1sec. timer is 
 * then used to read out the measurement values and issue the temperature and humidity via the console.
 *
 * Only compatible with rapidM2M Base Starter
 * 
 *
 * @version 20190508  
 */

 /* Path for hardware-specific include file */
#include ".\rapidM2M M2\mx.inc"
#include "sht31.inc"

/* Forward declarations of public functions */
forward public TimerMain();                      // Called up 1x per sec. to read the temperature and humidity from the SHT31
forward public TimerInit();                      // Single shot timer to initialise the I2C interface and the Temp. & RH sensor
                                                 // Called up in different intervals until the initialisation is done     

const
{
  PIN_LED1_R = 1,                                // RGB LED 1: Red      (GPIO1 is used)
  PIN_LED1_G = 2,                                // RGB LED 1: Green    (GPIO2 is used)
  PIN_LED1_B = 3,                                // RGB LED 1: Blue     (GPIO3 is used)
  PIN_LED2   = 0,                                // LED 2: Green        (GPIO0 is used)
  PIN_LED3   = 4,                                // LED 3: Green        (GPIO4 is used)
  
  LED_ENABLE  = RM2M_GPIO_HIGH,                  // By setting the GPIO to "high" the LED is turned on
  LED_DISABLE = RM2M_GPIO_LOW,                   // By setting the GPIO to "low" the LED is turned off
  
  PORT_I2C = 0,                                  // The fist I2C interface should be used.
  
  /* State of the Init process for the Temp. & RH sensor */
  INIT_START = 0,                                // Initialisation started
  INIT_SHT31_RESET,                              // Performs soft reset of the Temp. & RH sensor
  INIT_SHT31_CONFIG,                             // Configures sensor and starts measuring
  INIT_FINISHED,                                 // Temp. & RH sensor ready
}

/* Handles for available sensors */
static hTempRh[TSHT31_Handle];                   // Handle for the SHT31 (Temp. & RH sensor)
static iStateInit;                               // State of the Init process for the Temp. & RH sensor       

/* Application entry point */
main()
{
 /* Temporary memory for the index of a public function and the return value of a function      */
  new iIdx, iResult;
  
  /* Initialisation of a 1 sec. timer used for the general program sequence
     - Determining the function index that should be executed 1x per sec.
     - Transferring the index to the system
     - In case of a problem the index and return value of the init function are issued by the console*/
  iIdx = funcidx("TimerMain");
  iResult = rM2M_TimerAdd(iIdx);
  if(iResult < OK)
    printf("rM2M_TimerAdd(%d) = %d\r\n", iIdx, iResult);
  
  /* You could also refrain from setting the iStateInit to 0 (INIT_START), as all variables in 
	 PAWN are initialised with 0 at the time of their creation. However, it was completed at this 
	 point for the  purpose of a better understanding.                                        */
  iStateInit = INIT_START;                       // Sets the state of the init process to "Initialization started"
  
  InitHandler();                                 // Starts the initialisation handler
}

/* Single shot ms timer to initialise the I2C interface and the Temp. & RH sensor.
   The "InitHandler" function restarts the timer again until the initialisation is done.        */
public TimerInit()
{
  InitHandler();                                 // Initialises the I2C interface and the Temp. & RH sensor 
}


/* Initialises the I2C interface and the Temp. & RH sensor
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
    iStateInit = INIT_SHT31_RESET;               // Sets the state of the init process to "Perform soft reset"
  }
  else if(iStateInit == INIT_SHT31_RESET)        // Otherwise if the state of the Init process is "Perform soft reset" ->  
  {
    iResult = rM2M_I2cInit(PORT_I2C, 400000, 0); // Inits the I2C interface with 400kHz clock 
    
    /* Inits SHT31 (Temp. and rh measurement) and issues an error message if the sensor could not be initialised */
    iResult = SHT31_Init(hTempRh, PORT_I2C, SHT31_I2C_ADR_A);
    if(iResult < OK)
      printf("[INIT] SHT31_Init() = %d\r\n", iResult);

    rM2M_I2cClose(PORT_I2C);                     // Closes the I2C interface  
                                                 // Note: 50ms delay without I2C communication is required after SHT31_Init (SoftReset) 
    
    iTimeout = 100;                              // Sets the init timer to 100ms so that this 50ms delay is observed in any case.  
    iStateInit = INIT_SHT31_CONFIG;              // Sets the state of the init process to "Configures sensor and starts measuring"
  }
  else if(iStateInit == INIT_SHT31_CONFIG)       // Otherwise if the state of the init process is "Configures sensor and starts measuring"
  {
    iResult = rM2M_I2cInit(PORT_I2C, 400000, 0); // Inits the I2C interface with 400kHz clock 
    
    /* Starts measurement (periodic 1 measurement per second, medium repeatability) and 
	   issues an error message if the sensor could not be initialised                                     */
    iResult = SHT31_TriggerMeasurement(hTempRh, SHT31_CMD_MEAS_PERI_1_H);
    if(iResult < OK)
      printf("[INIT] SHT31_TriggerMeasurement() = %d\r\n", iResult);

    rM2M_I2cClose(PORT_I2C);                     // Closes the I2C interface 

    iStateInit = INIT_FINISHED;                  // Sets the state of the init process to "Temp. & RH sensor ready"
  }

  if(iTimeout > 0)                               //If the init timer should be started ->
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

/* 1 sec. timer is used to read the temperature and humidity from the SHT31 (Temp. and RH sensor) */
public TimerMain()
{  
  new iResult;                                   // Temporary memory for the return value of a function   
  new iTemperature;                              // Temporary memory for the temperature read from the SHT31 (Temp. and RH sensor) 
  new iHumidity;                                 // Temporary memory for the humidity read from the SHT31 (Temp. and RH sensor)   

  if(iStateInit == INIT_FINISHED)                // If the state of the init process is "Temp. & RH sensor ready" 
  {
    rM2M_I2cInit(PORT_I2C, 400000, 0);           // Inits the I2C interface with 400kHz clock
   
    //Reads the temperature and humidity from the SHT31 (Temp. and RH sensor) 
    iResult = SHT31_ReadMeasurement(hTempRh, iTemperature, iHumidity);

    if(iResult >= OK)                            // If the values could be read -> 
    {
      // Issues the read temperature and humidity via the console
	  printf("[SHT31] Temperature:%d RH:%d\r\n", iTemperature, iHumidity);
    }
    else                                         // Otherwise (error while reading the values) ->
    {
      /* Issues an error message via the console 
	     Note: probably NACK received indicating that no measurement data is present                   */
      printf("[SHT31] ReadMeasurement failed with %d\r\n", iResult);
    }
    rM2M_I2cClose(PORT_I2C);                     // Closes the I2C interface 
  }
}
