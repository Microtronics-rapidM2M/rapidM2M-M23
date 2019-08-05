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
 * Extended "Button" Example
 *
 * Evaluates the state of a button connected to the "INT0" pin and also detects if the button was 
 * pressed only briefly or for a longer time
 *
 * If the button was pressed ("low" signal level) "[KEY] Key pressed" is issued via the console.
 * If the button was pressed and held for a longer time "[KEY] Long Push Detected" is issued via the console.
 * If the button was released after it had been pressed and held for a longer time "[KEY] Long key detected 
 * and key released" is issued via the console.
 * If the button was released after it had been pressed briefly ("high" signal level) "[KEY] Key released" 
 * is issued via the console.
 * 
 * Only compatible with rapidM2M M2xx
 * Special hardware circuit necessary
 * Recommendation: use rapidM2M Base Starter
 *
 * @version 20190508  
 */


/* Forward declarations of public functions */
forward public KeyChanged(iKeyState);       // Called up when the button is pressed or released
forward public KeyDetectLong();             // Called up when the timer to detect a long key press has
                                            // expired

const
{
  IRQ_KEY            = 0,                   // Interruptible pin "INT0" is used for the button 
  CNT_LONG_PUSH_TIME = 5000,                // Time (ms) for which the button must be pressed to detect
                                            // a long button press
}

/* Global variable declarations */
static iLongPushDetected;                   // Long key press detected

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
     - In case of a problem the index and return value of the init function are issued by the console  */ 
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
     the close function via the console                                                                 */
  iResult = rM2M_IrqClose(IRQ_KEY);
  if(iResult < OK)
    printf("[KEY] rM2M_IrqClose(0)=%d\r\n", iResult);

  if(!iKeyState)                            // If the button was pressed ->
  {
	/* Activates the interrupt functionality of the input used for the button again and issues the result of  
     the init function via the console. An interrupt should occur on a rising edge (i.e. button released) */ 
    iIdx=funcidx("KeyChanged");
    iResult = rM2M_IrqInit(IRQ_KEY, RM2M_IRQ_RISING,iIdx);
    if(iResult < OK)
      printf("[KEY] rM2M_IrqInit(%d)=%d\r\n", iIdx, iResult);
    
    /* Starts a timer for the purpose of detecting a long key press. If the timer expires, a
     flag is set by the function transferred.
    - Determining the function index that should be called up when the timer expires and informing it 
	  that the timer should be stopped following expiry of the interval (single shot timer)
    - Transferring the index to the system                                                              */    
    iIdx = funcidx("KeyDetectLong");
    rM2M_TimerAddExt(iIdx, false, CNT_LONG_PUSH_TIME);
    
    printf("[KEY] Key pressed\r\n");        // Prints "[KEY] Key pressed" to the console
  }
  else                                      // Otherwise -> if the button was released ->
  {
    /* Activates the interrupt functionality of the input used for the button again and issues the result of  
     the init function via the console. An interrupt should occur on a falling edge (i.e. button pressed) */   
    iIdx=funcidx("KeyChanged");
    iResult = rM2M_IrqInit(IRQ_KEY, RM2M_IRQ_FALLING,iIdx);
    if(iResult < OK)
      printf("[KEY] rM2M_IrqInit(%d)=%d\r\n", iIdx, iResult);
      
    if(iLongPushDetected)                   // If the button had been pressed for a long time ->
    {
      // Prints "[KEY] Long key detected and key released" to the console
      printf("[KEY] Long key detected and key released\r\n");  
	  
      iLongPushDetected = 0;                // Delete "long key press detected" flag
    }
    else                                    // Otherwise (i.e. the button was pressed only briefly) ->
    {
     /* Deletes the timer for long keystroke detection if the button has been pressed only for a short
         time, so that it cannot expire and therefore the "long key press detected" flag is not set.
         - Determining the function index that should be called up when the timer expires
         - Transferring the index to the system so that the timer can be deleted
         - If it was not possible to delete the timer -> Index and TimerRemove function return values
           are issued via the console                                                                   */
      iIdx = funcidx("KeyDetectLong");
      iResult = rM2M_TimerRemoveExt(iIdx);
      if(iResult != OK) 
        printf("[KEY] rM2M_TimerRemove(%d) = %d\r\n", iIdx, iResult);
        
      printf("[KEY] Key released\r\n");     // Prints "[KEY] Key released" to the console
    }
  } 
}

/* Function that is called when the timer to detect a long key press has expired  */
public KeyDetectLong()
{
  print("[KEY] Long Push Detected\r\n");    // Issue the detection of a long key press via the console
  iLongPushDetected = 1;                    // Set "long key press detected" flag
}

