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
 * SINe and COSine curve functions
 * 
 * This header file can be used to calculate sine and cosine curves. The shape of the curves are configurable 
 * through the function parameters. Each time one of the functions is called the next value of the curve is 
 * calculated. A curve is separated into 360 steps.  
 *
 * @version 20190808
 *
 * Revision history:
 * 20190808
 * - Added JSDoc comments
 */


/**
 * Calculates sine curve
 * 
 * The function calculates values for a sine curve. A call of the function
 * calculates the next value of the curve.
 * 
 * @param fFrequency:f32	          - Number of cycles (full sinusoidal wave) within the 360 steps 
 * @param fScale:f32                  - Peak-to-peak amplitude of the curve
 * @param fOffset:f32                 - Zero offset of the curve (vertical shift)
 * @param iStep:s32                   - Time distance (in steps) between the last calculated value and the current value to be calculated   
 * @return f32                 		  - Current calculated measurement value
 */
Float:SinCurve(Float:fFrequency=1.0, Float:fScale=1.0, Float:fOffset=0.0, iStep=1)
{
  static iValue;
  
  iValue = ((iValue + iStep) % 360);
  
  return sin(float(iValue) * M_PI / 180.0 * fFrequency) * fScale + fOffset;
}


/**
 * Calculates cosine curve
 * 
 * The function calculates values for a cosine curve. A call of the function
 * calculates the next value of the curve.
 * 
 * @param fFrequency:f32	          - Number of cycles (full cosine wave) within the 360 steps 
 * @param fScale:f32                  - Peak-to-peak amplitude of the curve
 * @param fOffset:f32                 - Zero offset of the curve (vertical shift)
 * @param iStep:s32                   - Time distance (in steps) between the last calculated value and the current value to be calculated
 * @return f32                 		  - Current calculated measurement value
 */
Float:CosCurve(Float:fFrequency=1.0, Float:fScale=1.0, Float:fOffset=0.0, iStep=1)
{
  static iValue;
  
  iValue = ((iValue + iStep) % 360);
  
  return cos(float(iValue) * M_PI / 180.0 * fFrequency) * fScale + fOffset;
}