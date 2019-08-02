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
 * Calculates sunrise and sunset for the current day of the year and given latitude and longitude.
 * 
 * 
 * 
 * 
 *
 * @version 20190619
 */
 
#define DEF_POSITION_LAT   47.770364              // Middle of Austria (approximately St. Gilgen)
#define DEF_POSITION_LONG  13.334748

#define M_PI_180  (M_PI / 180.0)                  // Conversion factor from angular degrees to radians
#define M_H       (-(50.0 / 60.0) * M_PI_180)     // Height of the center of the sun (sunrise): radius + refraction
//#define M_B       (DEF_POSITION_LAT * M_PI_180) // Latitude in radians

const
{
  // Configuration flags for 	
  SUN_USE_UTC		= 0x01,
}

/**
 * Function to get the day of the year
 * 
 * The function returns the day of the year, starting with 1 (January 1st). The array ciMonth
 * provides the monthly day offset, depending on the months that have already passed this year.
 * 
 * @return s32 - Day of the year
 */
stock Sun_GetDayOfYear()
{
  static const ciMonth[] = [
    0, // 0 (no full month has passed)
    31, //Jan
    31+28, //Feb
    31+28+31, //Mar
    31+28+31+30, //Apr
    31+28+31+30+31, //May
    31+28+31+30+31+30, //Jun
    31+28+31+30+31+30+31, //Jul
    31+28+31+30+31+30+31+31, //Aug
    31+28+31+30+31+30+31+31+30, //Sep
    31+28+31+30+31+30+31+31+30+31, //Oct
    31+28+31+30+31+30+31+31+30+31+30  //Nov
  ]; // December is not needed (new year has already begun)

  new iYear, iMonth, iDay;
  
  rM2M_GetDate(iYear, iMonth, iDay);
  return ciMonth[iMonth-1] + iDay;
}

/**
 * Calculation of sunrise and sunset in minutes
 * 
 * This function can be used to calculate sunrise and sunset for the current day of the year
 * and given latitude and longitude. The information that is needed for sunrise and sunset
 * calculation can be split up to two parts:
 * 
 * - Equation of time: Provides information about the difference between apparent solar time
 *   and mean solar time. This is necessary because the apparent solar time can differ up to
 *   15 minutes from the mean solar time. The time equation can be understood as the difference
 *   between a mechanical or electrical clock and a sundial (our clocks always use a 24 hours 
 *   time span for one day).
 *   
 * - Time difference: This is the actual time span during which the sun is above the horizon
 *   (sunrise to midday or midday to sunset). To calculate this time difference the declination
 *   of the sun is needed. The declination is defined as the angular distance of a point
 *   (heavenly body -> in this case the sun) north or south of the celestial equator.
 * 
 * @param iSunrise:s32	 - Sunrise at given longitude and time zone in minutes
 * @param iSunset:s32    - Sunset at given longitude and time zone in minutes
 * @param fLatitude:f32  - Latitude for sunrise and sunset determination
 * @param fLongitude:f32 - Longitude for sunrise and sunset determination
 * @param iFlags:s32     - Flag that indicates usage of time zone offset
 */
stock Sun_GetDayLimit(&iSunrise, &iSunset, Float:fLatitude=DEF_POSITION_LAT, Float:fLongitude=DEF_POSITION_LONG, iFlags=0)
{
  new Float:fDayOfYear = float(Sun_GetDayOfYear());
  new iTimezoneOffset = rM2M_GetTimezoneOffset() / 60;  // Offset in minutes

#if defined DEBUG_Sunrise  
  printf("[Sunrise] DayOfYear: %g\r\nTimezoneOffset: %d\r\n", fDayOfYear, iTimezoneOffset);
#endif
  // Declination of the sun in radians
  // Formula 2008 by Arnold(at)Barmettler.com, fit to 20 years of average declinations (2008-2017)
  new Float:fSunDeclination = 0.409526325277017 * sin(0.0169060504029192 * (fDayOfYear - 80.0856919827619)); 
  
  // Difference between apparent solar time and mean solar time
  // formula 2008 by Arnold(at)Barmettler.com, fit to 20 years of average equation of time (2008-2017)
  new Float:fEquationoftime = -0.170869921174742 * sin(0.0336997028793971 * fDayOfYear + 0.465419984181394) - 0.129890681040717 * sin(0.0178674832556871 * fDayOfYear - 0.167936777524864);
  
  // Duration of half a day in hours: Duration from sunrise (Height M_H) to apparent solar time
  // If the argument of the acos function (absolute value) is greater than 1, the sun will never
  // reach the given height (sunset respectively sunrise). This phenomenon can appear in the far
  // north during polar night for example.
  new Float:fTimeDifference = 12.0 * acos((sin(M_H) - sin(fLatitude * M_PI_180) * sin(fSunDeclination)) / (cos(fLatitude * M_PI_180) * cos(fSunDeclination))) / M_PI;
  
  // Calculation of sunrise and sunset
  new Float:fAufgang    = 12.0 - fTimeDifference - fEquationoftime; // Sunrise at 0° longitude
  new Float:fUntergang  = 12.0 + fTimeDifference - fEquationoftime; // Sunset at 0° longitude

  // Sunrise at given longitude and time zone in minutes
  iSunrise = (fAufgang   - fLongitude / 15.0) * 60.0;
  // Sunset at given longitude and time zone in minutes
  iSunset  = (fUntergang - fLongitude / 15.0) * 60.0;
    
  if(!(iFlags & SUN_USE_UTC)) 
  {
    iSunrise += iTimezoneOffset;
    iSunset  += iTimezoneOffset;
  }
}