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
 * Toggles external LED between 3V3(<= 250mA) and LED status every second
 * 
 * Note: The external LED must be connected between 3V3(<= 250mA) and LED status.
 * 
 * Only compatible with rapidM2M M2xx
 *
 * @version 20190701
 */

/* Path for hardware-specific include file */
#include ".\rapidM2M M2\mx.inc"

/* Forward declarations of public functions */
forward public MainTimer();                 // Called up 1x per sec. for the program sequence

/* Global variables declarations */
static iLedState;                           // Current state of LED (1=^ On; x=^Off)

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
  
  /* Initialisation of the LED -> control via script activated */ 
  iResult = MxLed_Init(MX_LED_MODE_SCRIPT);
  printf("MxLed_Init() = %d\r\n", iResult);
  
  MxLed_Off(true);                          // Switches off LED
}

/* 1 sec. timer is used for the general program sequence */
public MainTimer()
{  
  if(iLedState)								// If LED is currently "On" ->
  { 
    MxLed_Off(true);                        // Turns off LED
    printf("[LED] off\r\n");
  }
  else                                      // Otherwise (i.e. LED is off)
  { 
    MxLed_On(true);                         // Turns on LED
    printf("[LED] on\r\n");
  }
  
  // Changes state of LED
  iLedState = !iLedState;                   // Toggles the variable which holds the current state of LED
}

