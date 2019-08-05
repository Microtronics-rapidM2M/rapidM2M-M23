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
 * Extended rapidM2M "Hello World" Example
 * Prints "Hello World" every second to the development console
 * 
 * Compatible with every rapidM2M hardware
 *
 * @version 20190508 
 */

/* Forward declarations of public functions */
forward public Timer1s();					// Called up 1x per sec. to issue "Hello World"

/* Application entry point */
main()
{
  /* Temporary memory for the index of a public function and the return value of a function             */
  new iIdx, iResult;

  /* Initialisation of a cyclic 1 sec. timer
     - Determining the function index that should be executed 1x per sec.
     - Transferring the index to the system
     - In case of a problem the index and return value of the init function are issued by the console  */
  iIdx = funcidx("Timer1s");
  iResult = rM2M_TimerAdd(iIdx);
  if(iResult < OK)
    printf("rM2M_TimerAddExt(%d) = %d\r\n", iIdx, iResult);
}

/* Cyclic 1s timer used to issue "Hello World" every second via the console */
public Timer1s()
{
  static iCounter = 0;                      // Number of times "Hello World" was issued via the console 
  
  iCounter++;                               // Increase Counter   
  printf("Hello World %d\r\n", iCounter);   // Prints "Hello World" and the current counter reading to the console
}
