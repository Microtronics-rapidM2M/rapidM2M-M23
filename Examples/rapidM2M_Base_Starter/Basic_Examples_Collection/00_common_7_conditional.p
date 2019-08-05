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
 * Simple rapidM2M "Conditional" Example
 * 
 * Example on how to use if and switch statements in rapidM2M projects
 * 
 * Compatible with every rapidM2M hardware
 *
 * @version 20190724
 */

/* Application entry point */
main()
{
  /* Get date and time */
  new sDateTime[TrM2M_DateTime];
  rM2M_GetDateTime(sDateTime);
  
  if(sDateTime.hour < 11)
  {
    print("Good Morning!\r\n");
  }
  else if(sDateTime.hour > 16)
  {
    print("Good Evening!\r\n");
  }
  else
  {
    print("Enjoy your meal!\r\n");
  }
  
  /* Note: No break statement is used inside a switch statement */
  switch(sDateTime.DoW)
  {
    case 5:
      print("Today is saturday!");
    case 6:
      print("Today is sunday!");
    default:
      print("Today is a workday!");
  }
}
