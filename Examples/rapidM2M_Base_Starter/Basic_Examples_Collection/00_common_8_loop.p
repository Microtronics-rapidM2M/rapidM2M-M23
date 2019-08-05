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
 * Simple rapidM2M "Loop" Example
 * 
 * Example on how to use loops in rapidM2M projects
 * 
 * Compatible with every rapidM2M hardware
 *
 * @version 20190724
 */

/* Application entry point */
main()
{
  new iBlock;
  
  /* Calculates factorials in blocks of three */
  do
  {
    print("--\r\n");
    for(new i=iBlock; i<=iBlock+2; i++)
    {
      /* Initial value must be 1 */
      new iRes = 1;
      new j = i;
      
      while(j > 0)
      {
        /* Calculates factorial for variable i with all positive integers smaller than i */
        iRes *= j--;
      }
      
      /* Prints factorial */
      printf("%d! = %d\r\n", i, iRes);
    }
    iBlock += 3;
  } while(iBlock <= 8)
    
  print("--\r\n");
}
