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
 * Simple "Status LED and interrupt input (button)" Example
 *
 * As long as the button connected to INT0 is pressed, an external LED connected to "LED status" 
 * lights up. If the button is released, the LED is turned off.
 *
 * Note: The external LED and a resistor must be placed between "3V3" and "LED status".
 * 
 * Only compatible with rapidM2M M2xx
 * Special hardware circuit necessary 
 * 
 * @version 20190624
 */
 
/* Path for hardware-specific include file */
#include ".\rapidM2M M2\mx.inc"

/* Forward declarations of public functions */
forward public KeyChanged(iState);       // Called up when the button is pressed or released

/* Pin configuration */
const
{
  IRQ_KEY    = 0                         // IRQ for the key 
}

/* Application entry point */
main()
{
  /* Temporary memory for the index of a public function and the return value of a function               */
  new iIdx, iResult;

  printf("Led Demo\r\n");                // Issues the name of the example via the console 

  /* Initialisation of the interrupt input used for the button
     - Determining the function index that should be called up when pressing the button
     - Transferring the index to the system and informing it that an interrupt should
       accrue on a falling edge (i.e. button pressed)
     - The index and return value of the init function are issued by the console  	                      */
  iIdx = funcidx("KeyChanged");                        
  iResult = rM2M_IrqInit(IRQ_KEY, RM2M_IRQ_FALLING, iIdx);  
  printf("rM2M_IrqInit(%d) = %d\r\n", iIdx, iResult);

  /* Initialisation of the LED -> Control via script activated */ 
  iResult = MxLed_Init(MX_LED_MODE_SCRIPT);
  printf("MxLed_Init() = %d\r\n", iResult);
  
  MxLed_Off(true);                       // Switches off LED
}

/**
 * Function that should be called up when the button is pressed or released
 *
 * Note: An interruptible pin can only be configured to initiate the interrupt on the rising OR falling 
 *       edges. If both pressing and releasing the button should be detected when an interrupt 
 *       occurs, it is necessary to first deactivate the interrupt functionality and then activate the 
 *       interrupt functionality again, but that time for the other edge.
 *
 * @param iState:s32 - Signal level at the interruptible pin after the edge that triggered the 
 *                     interrupt occurred 
 *					   0: "low" signal level
 *                     1: "high" signal level
 */
public KeyChanged(iState)
{
  /* Temporary memory for the index of a public function and the return value of a function               */  
  new iIdx,iResult;
  
  /* Deactivates the interrupt functionality of the input used for the button and issues the result of  
     the close function via the console                                                                   */
  iResult = rM2M_IrqClose(IRQ_KEY);
  if(iResult < OK)
    printf("[KEY] rM2M_IrqClose(0)=%d\r\n", iResult);
  
  iState = !iState; 					 // Button has an inverted logic (0: pressed, 1: released)

  if(iState)                             // If the button has been pressed ->
  {
    /* Activates the interrupt functionality of the input used for the button again and issues the result of  
     the init function via the console. An interrupt should occur on a rising edge (i.e. button released) */ 
    iIdx = funcidx("KeyChanged");
    iResult = rM2M_IrqInit(IRQ_KEY, RM2M_IRQ_RISING,iIdx);
    if(iResult < OK)
      printf("[KEY] rM2M_IrqInit(%d)=%d\r\n", iIdx, iResult);
    
    MxLed_On(true);                      // Switches on LED
  }
  else                                   // Otherwise -> If the button was released ->
  {
    /* Activates the interrupt functionality of the input used for the button again and issues the result of  
     the init function via the console. An interrupt should occur on a falling edge (i.e. button pressed) */   
    iIdx=funcidx("KeyChanged");
    iResult = rM2M_IrqInit(IRQ_KEY, RM2M_IRQ_FALLING,iIdx);
    if(iResult < OK)
      printf("[KEY] rM2M_IrqInit(%d)=%d\r\n", iIdx, iResult);
    
	MxLed_Off(true);                     // Switches off LED
  } 
}
