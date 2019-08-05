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
 * Prints "Hello World" every 5 seconds to the development console
 * 
 * Compatible with every rapidM2M hardware
 *
 * @version 20190619 
 */

/* Forward declarations of public functions */
forward public Timer5s();					// Called up every 5 seconds to issue "Hello World"

/* Application entry point */
main()
{
  /* Temporary memory for the index of a public function and the return value of a function             */
  new iIdx, iResult;

  /* Initialisation of a cyclic 5 sec. timer
     - Determining the function index that should be executed every 5 seconds.
     - Transferring the index to the system and informing it that the timer should be restarted 
	   following expiry of the interval
     - In case of a problem the index and return value of the init function are issued by the console  */
  iIdx = funcidx("Timer5s");
  iResult = rM2M_TimerAddExt(iIdx, true, 5000);
  if(iResult < OK)
    printf("rM2M_TimerAddExt(%d) = %d\r\n", iIdx, iResult);
}

/* Cyclic 5s timer used to issue "Hello World" every 5 seconds via the console */
public Timer5s()
{
  static iCounter = 0;                      // Number of times "Hello World" was issued via the console 
  
  iCounter++;                               // Increases Counter   
  printf("Hello World %d\r\n", iCounter);   // Prints "Hello World" and the current counter reading to the console
}
