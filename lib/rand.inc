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
 * Pseudorandom number generators
 * 
 * Various implementations of pseudorandom number generators (PRNG). The return value
 * is always a number between 0.0 and 1.0.
 * 
 * Revision history:
 * 20190711
 * - Added JSDoc comments
 */

/**
 * PRN calculation by using Xorshift (32bit) algorithm
 *
 * A PRN between 0.0 and 1.0 is calculated using the Xorshift (32bit) algorithm. A static
 * variable contains the starting value for the Xorshift. If no (re)initialisation should be
 * performed, the calculation result of the last function call is used as new starting value when
 * calling the function again (static variable). The initialisation value should be a true random
 * number.
 * 
 * @param bInit - Determines the initial value
 *                - false: Seed will not be (re)initialised with system time (default)
 *                - true:  Seed will be (re)initialised with system time
 * @return f32  - PRN between 0.0 and 1.0
 */
stock Float:rand_xor(bInit = false) 
{
  /* Initialisation value - should be a true random number (e.g. system time) */
  static x = 314159265;
  
  /* Checks if the system time should be used as initial value */
  if(bInit)
    x = rM2M_GetTime();
  
  /* In this case the triple 13/17/5 is used for bit shifting. This constellation passes almost
     all tests for randomness according to George Marsaglia's Xorshift RNGs on page 4. Please
     refer to this document for more detailed information about the Xorshift algorithm.      */
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  
  /* The multiplication factor below equals 1/(2^32 + 2). By multiplying the 32 bit result with
     this factor, a PRN between -0.5 and 0.5 is the output. After adding 0.5, the result is
     strictly between 0 and 1. */
  return float(x) * 2.328306435454494e-10 + 0.5;
}

/**
 * PRN calculation by using rnd algorithm of VB6
 *
 * A PRN between 0.0 and 1.0 is calculated using the rnd algorithm of VB6. A static
 * variable contains the starting value. If no (re)initialisation should be performed,
 * the calculation result of the last function call is used as new starting value when
 * calling the function again (static variable). The initialisation value should be a true
 * random number.
 * 
 * @param bInit - Determines the initial value
 *                - false: Seed will not be (re)initialised with system time (default)
 *                - true:  Seed will be (re)initialised with system time
 * @return f32  - PRN between 0.0 and 1.0
 */
stock Float:rand_vb6(bInit = false)
{
  /* Initialisation value - should be a true random number (e.g. system time) */
  static rndVal = 0x50000

  /* Checks if the system time should be used as initial value */
  if(bInit)
    rndVal = rM2M_GetTime();
  
  /* Algorithm */
  rndVal = (rndVal * 0x43fd43fd + 0xc39ec3) & 0xffffff;
  return float(rndVal) / 16777216.0;
}

/**
 * PRN calculation by using Multiply-with-carry algorithm
 *
 * A PRN between 0.0 and 1.0 is calculated using George Marsaglia's MWC algorithm to produce an
 * unsigned integer. Two static variables that contain the seeds are separately used within the
 * algorithm and the output results directly from them. The two partial results are used as
 * initial values when calling the function again (static variables). The initialisation value
 * of one seed should be a true random number.
 * 
 * @param bInit - Determines the initial value
 *                - false: Seed will not be (re)initialised with system time (default)
 *                - true:  One of the seeds will be (re)initialised with system time
 * @return f32  - PRN between 0.0 and 1.0
 */
stock Float:rand_mwc(bInit = false)
{
  new iReturn;
  
// The random generator seed can be set three ways:
// 1) specifying two non-zero unsigned integers
// 2) specifying one non-zero unsigned integer and taking a default value for the second
// 3) setting the seed from the system time

  /* Seeds */
  static m_z = 521288629
  static m_w = 362436069 // Initialisation value should be a true random number
  
  /* Checks if the system time should be used as initial value */
  if(bInit)
    m_w = rM2M_GetTime();
  
  /* Algorithm */
  m_z = 36969 * (m_z & 65535) + (m_z >> 16);
  m_w = 18000 * (m_w & 65535) + (m_w >> 16);
  iReturn = (m_z << 16) + m_w;
  
  /* The multiplication factor below equals 1/(2^32 + 2). By multiplying the 32 bit result with
     this factor, a PRN between -0.5 and 0.5 is the output. After adding 0.5, the result is
     strictly between 0 and 1. */
  return (iReturn + 1.0) * 2.328306435454494e-10 + 0.5;
}

/**
 * PRN calculation by using CRC32 algorithm
 *
 * A PRN between 0.0 and 1.0 is calculated using the CRC32 algorithm. A static variable
 * contains the starting value for the CRC32. If no (re)initialisation should be performed,
 * the calculation result of the last function call is used as new starting value when
 * calling the function again (static variable). The system function CRC32() requires an
 * array of s32 values (cells) as parameter input hence the initialisation value needs
 * to be a s32 type cell array. The initialisation value should be a true random number.
 * 
 * @param bInit - Determines the initial value
 *                - false: Seed will not be (re)initialised with system time (default)
 *                - true:  Seed will be (re)initialised with system time
 * @return f32  - PRN between 0.0 and 1.0
 */
stock Float:rand_crc(bInit = false)
{
  /* Initialisation value - should be a true random number (e.g. system time) */
  static x[0] = 314159265;

  /* Checks if the system time should be used as initial value */
  if(bInit)
    x[0] = rM2M_GetTime();
  
  /* Performs a CRC32 for the given 32 bit variable. The second parameter indicates the number
     of bytes to be considered ( 4 x 8bit = 32 bit )*/
  x[0] = CRC32(x[0],4);
  
  /* The multiplication factor below equals 1/(2^32 + 2). By multiplying the 32 bit result with
     this factor, a PRN between -0.5 and 0.5 is the output. After adding 0.5, the result is
     strictly between 0 and 1. */
  return float(x[0]) * 2.328306435454494e-10 + 0.5;
}