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
 * Simple "State machine" example
 *
  * The state machine has four different states that are indicated by the external LED
 * - 0  LED flashes continuously
 * - 1  LED blinks continuously
 * - 2  LED flashes 3x every 5sec.
 * - 3  LED blinks 3x every 5sec.
 *
 * When turned on, the state machine is in state "0". Every 5 seconds the system 
 * switches to the next state. When state "3" is reached the sequence repeats,
 * starting with the state "0"
 *
 * Note: The external LED must be connected between 3V3(<=250mA) and LED status.
 *
 * Only compatible with rapidM2M M2xx
 *
 * @version 20190723
 */
 
/* Path for hardware-specific include file */
#include ".\rapidM2M M2\mx.inc"

/* Forward declarations of public functions */
forward public Timer5s();                   // Called every 5 sec. for the program sequence

static iState = 0;                          // Current state of the application

/* Application entry point */
main()
{
 /* Temporary memory for the index of a public function and the return value of a function  */
  new iIdx, iResult;

  /* Output module information */
  printf("Led Demo\r\n");                   // Issues the name of the example via the console
  
  /* Initialisation of a 5sec. timer used for the general program sequence
     - Determining the function index that should be executed every 5sec.
     - Transferring the index to the system and informing it that the timer should be restarted 
	   following expiry of the interval
	 - In case of a problem the index and return value of the init function are issued by the console  */	  
  iIdx = funcidx("Timer5s");
  iResult = rM2M_TimerAddExt(iIdx, true, 5000);
  if(iResult < OK)
    printf("rM2M_TimerAddExt(%d) = %d\r\n", iIdx, iResult);  

  /* Initialisation of the LED -> control via script activated */ 
  iResult = MxLed_Init(MX_LED_MODE_SCRIPT);
  printf("MxLed_Init() = %d\r\n", iResult);
  
  Timer5s();                                // Calls the timer callback function to display the initial state of the application using the LEDs
}

/* 5 sec. timer is used for the general program sequence */
public Timer5s()
{
  printf("State: %d: ", iState);            // Issues the current state of the application via the console

  if(iState == 0)
  {
    // LED should flash continuously
    print("flashing cont.\r\n");
    MxLed_Off(true);
    MxLed_Flash(0); 
  }
  else if(iState == 1)
  {
    // LED should blink continuously
	  print("blinking cont.\r\n");
    MxLed_Off(true);
    MxLed_Blink(0);
  }
  else if(iState == 2)
  {
    // LED should flash 3x every 5sec.
    print("3x flash\r\n");
    MxLed_Off(true);
    MxLed_Flash(3);
  }
  else if(iState == 3)
  {
    // LED should blink 3x every 5sec.
    print("3x blink\r\n");
    MxLed_Off(true);
    MxLed_Blink(3);
  }
  else                                      // Otherwise (none of the specified states; should not occur ) ->
  {
    // LED should be switched off
    print("off\r\n");
    MxLed_Off(true);
  }
  
  iState = (iState + 1) % 4;                // Increases the state of the application. It is ensured that the state does not exceed 3.
}
