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
 * Simple "Button" Example
 *
 * Evaluates the state of a button connected to the "INT0" pin.
 * If the button was pressed ("low" signal level) "[KEY] Key pressed" is issued via the console.
 * If the button was released ("high" signal level) "[KEY] Key released" is issued via the console. 
 *
 * Only compatible with rapidM2M M2xx
 * Special hardware circuit necessary
 * Recommendation: use rapidM2M Base Starter
 *
 * @version 20190508  
 */

 /* Path for hardware-specific include file */

/* Forward declarations of public functions */
forward public KeyChanged(iKeyState);       // Called up when the button is pressed or released

const
{
  IRQ_KEY = 0,                              // Interruptible pin "INT0" is used for the button 
}

/* Application entry point */
main()
{
  /* Temporary memory for the index of a public function and the return value of a function             */
  new iIdx;
  new iResult;
  	 
  /* Initialisation of the interrupt input used for the button
     - Determining the function index that should be called up when pressing the button
     - Transferring the index to the system and informing it that an interrupt should
       accrue on a falling edge (i.e. button pressed)
     - In case of a problem the index and return value of the init function are issued by the console  	*/
  iIdx = funcidx("KeyChanged");                        
  iResult = rM2M_IrqInit(IRQ_KEY, RM2M_IRQ_FALLING, iIdx);  
  if(iResult < OK)
    printf("[MAIN] rM2M_IrqInit(%d) = %d\r\n", iIdx, iResult);
  
}

/**
 * Function that should be called up when the button is pressed or released
 *
 * Note: An interruptible pin can only be configured to initiate the interrupt on the rising OR falling 
 *       edges. If both pressing and releasing the button should be detected when an interrupt 
 *       occurs, it is necessary to first deactivate the interrupt functionality and then activate the 
 *       interrupt functionality again, but this time for the other edge.
 *
 * @param iKeyState:s32 - Signal level at the interruptible pin after the edge that triggered the 
 *                        interrupt occurred 
 *						  0: "low" signal level
 *                        1: "high" signal level
 */
public KeyChanged(iKeyState)
{
  /* Temporary memory for the index of a public function and the return value of a function             */  
  new iIdx,iResult;
  
  /* Deactivates the interrupt functionality of the input used for the button and issues the result of  
     the close function via the console                                                                */
  iResult = rM2M_IrqClose(IRQ_KEY);
  if(iResult < OK)
    printf("[KEY] rM2M_IrqClose(0)=%d\r\n", iResult);

  if(!iKeyState)                            // If the button was pressed ->
  {
    /* Activates the interrupt functionality of the input used for the button again and issues the result of  
     the init function via the console. An interrupt should occur on a rising edge (i.e. button released) */ 
    iIdx = funcidx("KeyChanged");
    iResult = rM2M_IrqInit(IRQ_KEY, RM2M_IRQ_RISING,iIdx);
    if(iResult < OK)
      printf("[KEY] rM2M_IrqInit(%d)=%d\r\n", iIdx, iResult);
    
    printf("[KEY] Key pressed\r\n");		// Prints "[KEY] Key pressed" to the console
  }
  else                                      // Otherwise -> If the button was released ->
  {
    /* Activates the interrupt functionality of the input used for the button again and issues the result of  
     the init function via the console. An interrupt should occur on a falling edge (i.e. button pressed) */   
    iIdx=funcidx("KeyChanged");
    iResult = rM2M_IrqInit(IRQ_KEY, RM2M_IRQ_FALLING,iIdx);
    if(iResult < OK)
      printf("[KEY] rM2M_IrqInit(%d)=%d\r\n", iIdx, iResult);
      
   
    printf("[KEY] Key released\r\n");       // Prints "[KEY] Key released" to the console
  } 
}


