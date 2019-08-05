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
 * Simple "Pack/Unpack" Example
 *
 * First packs a float32 type variable into a byte array and then unpacks the data back into a float32 type variable.
 * After that, the float32 type variable is issued via the console. 
 * 
 * Compatible with every rapidM2M hardware
 *
 * @version 20190619 
 */

/* application entry point */
main()
{
  new aBuffer{16};                          // Temporary array to store the packed data 
  new Float:fValue;                         // Temporary memory for a float32 type value that should be copied into the packed data array

  fValue = 10.0;                                                            // Sets temporary memory for a float32 type to "10"
  rM2M_Pack(aBuffer, 0, fValue, RM2M_PACK_F32|RM2M_PACK_BE);                // Writes fValue at position 0 to aBuffer in BigEndian-Order
  
  rM2M_Pack(aBuffer, 0, fValue, RM2M_PACK_F32|RM2M_PACK_BE|RM2M_PACK_GET);  // Reads fValue at position 0 from aBuffer in BigEndian-Order
  
  printf("Value %f\r\n", fValue);                                           // Issues the temporary memory for a float32 type via the console
}
