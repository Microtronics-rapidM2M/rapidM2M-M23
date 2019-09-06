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
 *  GNSS interface functions
 * This header file can be used to handle received GNSS data based on NMEA format received from
 * any external GNSS receiver module that is connected to one of the UART peripheral interfaces.
 * Decoding of frames with Sentence ID "GGA", "GSV", "RMC" and "VTG" is supported, providing
 * information about longitude, latitude, speed, course and many other GNSS data tokens.
 * In addition, checksum calculation is implemented as well as predefined structures,
 * debugging and data saving functionalities.
 * 
 * Interfaces: 1x UART
 *
 * @version 20190618
 */

#include <string.inc>

/** 
 * Use this macro to configure debug mode:
 *  
 * 0: No debugging
 * 1: Decoded NMEA tokens are printed to the console
 * 2: Entire to be decoded NMEA frames are printed to the console
 */
#define GNSS_DEBUG           (0)

#define TYPE_GNSS_GGA[
                       .Available,
					   .UTC,                 // [hhmmss]
                       .Latitude,            // [decimal degrees]
                       .Longitude,           // [decimal degrees]
                       .PosFixInd,           // []
                       .SatUsed,             // []
                       .HPrecision,          // [0.1]
                       .Altitude,            // [0.1 m]
					   .GeoSep,              // [0.1 m]
					   .DGNSSage,            // [0.1 s]
					   .DGNSSID              // []
                     ]

#define TYPE_GNSS_GSV[
                       .SvPrnNumber,         // [] SV PRN number
                       .Elevation,           // [Degrees] Elevation in degrees, 90 maximum
                       .Azimuth,             // [Degrees] Azimuth, degrees from true north, 000 to 359
                       .Snr                  // [dB] SNR, 00-99 dB (null when not tracking)
					 ]

#define TYPE_GNSS_RMC[
                       .Available,
                       .UTC,                 // [hhmmss]
					   .Status{1},           // []
                       .Latitude,            // [decimal degrees]
                       .Longitude,           // [decimal degrees]
                       .SpeedOverGround,     // [0.001 kn]
                       .CourseOverGround,    // [0.1 degree]
					   .UTCdate,             // [ddmmyy]
					   .MagVar,              // [0.1 degree]
					   .Mode{1}              // []
					 ]

#define TYPE_GNSS_VTG[
                       .Available,
                       .CourseOverGroundTrue,     // [0.1 degree]
                       .CourseOverGroundMagnetic, // [0.1 degree]						  
					   .SpeedOverGround_kn,       // [0.001 kn]
                       .SpeedOverGround_kph,      // [0.001 kph]
					   .Mode{1}                   // []
                     ]
					   
/* Used constants */
const
{
  GNSS_MAX_FRAME_LENGTH = 100,   /* Maximum supported NMEA frame length */
  GNSS_MAX_TOKEN_LENGTH = 20,    /* Maximum length of a NMEA token */
}

const
{
  GNSS_SAVE_STOP = 0,
  GNSS_SAVE_START,
  GNSS_SAVE_RUN,
}

const GNSS_SAT_COUNT = 16;
static gaGNSS_GSV[GNSS_SAT_COUNT][TYPE_GNSS_GSV]; /* Satellite data buffer */
static iGNSS_Uart = -1;

/**
 * Function to initialise UART interface
 * 
 * This function is used to initialise UART peripheral interface for communication with
 * a GNSS module. The configuration is fixed to 9600 baud 8N1 and the result printed to
 * the console.
 * 
 * @param Uart:s32 - Number of the UART interface
 */
stock GNSS_Init(Uart = -1)
{
  new iIdx;
  new iResult;
  
  iGNSS_Uart = Uart;
  if(Uart != -1)
  {
    /* Init UART with 9600 baud 8N1 */
    iIdx = funcidx("UartRx");
    iResult = rM2M_UartInit(iGNSS_Uart, 9600, RM2M_UART_8_DATABIT|RM2M_UART_PARITY_NONE|RM2M_UART_1_STOPBIT, iIdx);
    printf("rM2M_UartInit(%d) = %d\r\n", iIdx, iResult);
  }
}

/**
 * Function to close UART interface
 */
stock GPS_Close()
{
  rM2M_UartClose(iGNSS_Uart);
  iGNSS_Uart = -1;
}

/**
 * Function to tokenize NMEA frame
 *
 * Gives back a specific token, NMEA tokens are separated by commas inside one frame. This function scans
 * character by character until it detects a ',' (end of token), a '*' (checksum) or '\0' (end of frame).
 * The characters are stored in sResult{} and at the end the index is set to the beginning of the next token.
 * 
 * @param string[]:u8  - String that contains the NMEA frame to be tokenized
 * @param sResult[]:u8 - String that contains the token
 * @param index:s32    - Current position in the NMEA frame (offset)
 */
stock GNSS_StrTok(const string{}, sResult{}, &index)
{
  new iChar;                      /* Buffer for single character of a token */
  new iLen = 0;                   /* Length of token */
  
  /* Gets character at defined position in NMEA frame */
  iChar = string{index};
  
  while((iChar != ',') && (iChar != '*') && (iChar != '\0') &&
        (iLen < GNSS_MAX_TOKEN_LENGTH))
  {
    sResult{iLen++} = iChar;      /* Stores current character in sResult (token) */
    iChar = string{++index};      /* Gets next character */
  }
  sResult{iLen} = '\0';           /* End of token */

  /* Sets index to next character after ',' */
  if(iChar != '\0') ++index;
}

/**
 * Function to verify NMEA frame checksum
 *
 * The GNSS checksum calculation is performed by considering all characters between '$' and
 * '*'. After '*' two characters follow, representing the two hexadecimal digits of the
 * checksum. The calculation simply consists of executing a XOR operation character by character
 * starting with the first one after '$'.
 * 
 * @param frame[]:u8 - String that contains NMEA frame to be verified
 * @return s32       - -3: Checksum wrong
 *                   - -2: Character of frame is no letter, digit or punctuation
 *                         (ASCII range 0x21 to 0x7E)
 *                   - -1: NMEA frame does not start with '$'
 *                   -  0: Calculated and received checksum are equal
 */
stock GNSS_NMEAIsChecksumValid(const frame{})
{
  new iIndex=0;
  new iChecksum;
  new iReceivedChecksum;
  new sReceivedChecksum{3};

  /* Checks start character */
  if(frame{iIndex++} != '$') 
    return(-1);

  /* Calculates checksum */
  iChecksum = 0;
  while(frame{iIndex} != '*')
  {
    if((frame{iIndex} < 0x20) || (frame{iIndex} > 0x7F) ) 
      return(-2);
    iChecksum ^= frame{iIndex++};
  }
  iIndex++;
  
  /* Gets received checksum and converts it to value */
  sReceivedChecksum{0} = frame{iIndex++};
  sReceivedChecksum{1} = frame{iIndex++};
  sReceivedChecksum{2} = 0;
  iReceivedChecksum = strtol(sReceivedChecksum, 16);

  /* Compares calculated checksum with received checksum */
  if(iChecksum == iReceivedChecksum) 
    return(0);

  /* Checksum wrong */
  return(-3);
}

/**
 * Function to decode NMEA frames with Sentence ID "GSV"
 * 
 * This function decodes NMEA frames with Sentence ID "GSV" and saves the information of
 * all satellites in view. Each GSV-frame contains information about SV PRN number, elevation,
 * azimuth and SNR of four SVs. The first token gives information about how many frames are sent
 * in total (max. 4), the second indicates the number and the third contains the specific
 * SV number (SV PRN number). Consequently, at a maximum information about 16 SVs is provided.
 * The data is saved in gaGNSS_GSV[], an array of GNSS_SAT_COUNT structures of the type TYPE_GNSS_GSV.
 *
 * --------------------------------------------------------------------
 * NMEA GSV sentence:
 * 
 * 1    = Total number of messages of this type in this cycle
 * 2    = Message number
 * 3    = Total number of SVs in view
 * 4    = SV PRN number
 * 5    = Elevation in degrees, 90 maximum
 * 6    = Azimuth, degrees from true north, 000 to 359
 * 7    = SNR, 00-99 dB (null when not tracking)
 * 8-11 = Information about second SV, same as field 4-7
 * 12-15= Information about third SV, same as field 4-7
 * 16-19= Information about fourth SV, same as field 4-7
 * --------------------------------------------------------------------
 *
 * @param sFrame[]:u8 - String that contains NMEA frame
 */
stock GNSS_NMEADecodeGSV(const sFrame{})
{
  new sToken{GNSS_MAX_TOKEN_LENGTH};
  new iIdx = 0;
  new aItem[TYPE_GNSS_GSV];
  new iMsgIndex;
  
#if GNSS_DEBUG >= 2
  printf("%s\n", sFrame);
#endif
  
  /* Talker ID + Sentence ID */
  GNSS_StrTok(sFrame, sToken, iIdx);
#if GNSS_DEBUG >= 1
  printf("%s:\r\n", sToken);
#endif

  /* Total number of messages */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Messages Count = %s\r\n", sToken);
#endif
  }

  /* Message number */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Message number = %s\r\n", sToken);
#endif
    iMsgIndex = atoi(sToken) - 1; // 0 - based index
  }
  
  /* Total number of SVs in view */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Count SV = %s\r\n", sToken);
#endif
  }

  for(new iSVIdx = 0; iSVIdx < 4; iSVIdx++)
  {
    /* SV PRN number */
    GNSS_StrTok(sFrame, sToken, iIdx);
    if(sToken{0} != '\0')
    {
#if GNSS_DEBUG >= 1
      printf("  SvPrnNumber = %s\r\n", sToken);
#endif
      aItem.SvPrnNumber = atoi(sToken);
    }
    else
    {
      aItem.SvPrnNumber = 0;
    }
    
    /* Elevation */
    GNSS_StrTok(sFrame, sToken, iIdx);
    if(sToken{0} != '\0')
    {
#if GNSS_DEBUG >= 1
      printf("  Elevation = %s\r\n", sToken);
#endif
      aItem.Elevation = atoi(sToken);
    }
    else
    {
      aItem.Elevation = 0;
    }
    
    /* Azimuth */
    GNSS_StrTok(sFrame, sToken, iIdx);
    if(sToken{0} != '\0')
    {
#if GNSS_DEBUG >= 1
      printf("  Azimuth = %s\r\n", sToken);
#endif
      aItem.Azimuth = atoi(sToken);
    }
    else
    {
      aItem.Azimuth = 0;
    }

    /* SNR */
    GNSS_StrTok(sFrame, sToken, iIdx);
    if(sToken{0} != '\0')
    {
#if GNSS_DEBUG >= 1
      printf("  SNR = %s\r\n", sToken);
#endif
      aItem.Snr = atoi(sToken);
    }
    else
    {
      aItem.Snr = 0;
    }
    
    if(iMsgIndex * 4 + iSVIdx < GNSS_SAT_COUNT)
    {
      gaGNSS_GSV[iMsgIndex * 4 + iSVIdx] = aItem;
      if(aItem.SvPrnNumber == 0)
      {
        break;
      }
    }
  }
}

/**
 * Function to decode NMEA frames with Sentence ID "VTG"
 * 
 * This function decodes NMEA frames with Sentence ID "VTG". The data is stored in the given 
 * structure. For conversion/resolution information of the stored data, please refer to the
 * TYPE_GNSS_VTG structure declaration above.
 *
 * --------------------------------------------------------------------
 * NMEA VTG sentence:
 * 
 * 1    = Course over ground (true north)
 * 2    = Fixed text 'T' indicates that course over ground is relative to true north
 * 3    = Course over ground (magnetic)
 * 4    = Fixed text 'M' indicates that course over ground is relative to magnetic north
 * 5    = Speed over ground in knots
 * 6    = Fixed text 'N' indicates that speed over ground is in knots
 * 7    = Speed over ground in kilometres/hour
 * 8    = Fixed text 'K' indicates that speed over ground is in kilometres/hour
 * 9    = Mode indicator N=No Fix, E=Estimated/Dead Reckoning Fix, A=Autonomous GNSS Fix, D=Differential GNSS Fix
 * 10   = Checksum
 * --------------------------------------------------------------------
 *
 * @param sFrame[]:u8                        - String that contains NMEA frame
 * @param gaGNSS_VTG_item:TYPE_GNSS_VTG      - Structure that contains GNSS VTG data
 */
stock GNSS_NMEADecodeVTG(const sFrame{}, gaGNSS_VTG_item[TYPE_GNSS_VTG])
{
  new sToken{GNSS_MAX_TOKEN_LENGTH};
  new iIdx = 0;

#if GNSS_DEBUG >= 2
  printf("%s\n", sFrame);
#endif
  
  gaGNSS_VTG_item.Available = 1;
  
  /* Talker ID + Sentence ID */
  GNSS_StrTok(sFrame, sToken, iIdx);
#if GNSS_DEBUG >= 1
  printf("%s:\r\n", sToken);
#endif
  
  /* Course over ground (true north) */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Course over ground (true north) = %s\r\n", sToken);
#endif
    gaGNSS_VTG_item.CourseOverGroundTrue = atof(sToken) * 10.0
  }
  else
  {
    gaGNSS_VTG_item.CourseOverGroundTrue = 0;
  }
  
  /* Fixed text 'T' indicates that course over ground is relative to true north  */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Fixed text 'T' = %s\r\n", sToken);
#endif
  }

  /* Course over ground (magnetic) */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Course over ground (magnetic) = %s\r\n", sToken);
#endif
    gaGNSS_VTG_item.CourseOverGroundMagnetic = atof(sToken) * 10.0
  }
  else
  {
    gaGNSS_VTG_item.CourseOverGroundMagnetic = 0;
  }
  
  /* Fixed text 'M' indicates that course over ground is relative to magnetic north  */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Fixed text 'M' = %s\r\n", sToken);
#endif
  }

  /* Speed over ground in knots */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Speed over ground in knots = %s\r\n", sToken);
#endif
    gaGNSS_VTG_item.SpeedOverGround_kn = atof(sToken) * 1000.0;
  }
  else
  {
    gaGNSS_VTG_item.SpeedOverGround_kn = 0;
  }
  
  /* Fixed text 'N' indicates that speed over ground is in knots  */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Fixed text 'N' = %s\r\n", sToken);
#endif
  }

  /* Speed over ground in kilometres/hour */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Speed over ground in kph = %s\r\n", sToken);
#endif
    
    gaGNSS_VTG_item.SpeedOverGround_kph = atof(sToken) * 1000.0;
  }
  else
  {
    gaGNSS_VTG_item.SpeedOverGround_kph = 0;
  }
  
  /* Fixed text 'K' indicates that speed over ground is in kilometres/hour */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Fixed text 'K' = %s\r\n", sToken);
#endif
  }
  
  /* Mode indicator */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Mode Indicator = %s\r\n", sToken);
#endif
    gaGNSS_VTG_item.Mode{0} = sToken{0};
  }
  else
  {
    gaGNSS_VTG_item.Mode{0} = '0';
  }
}

/**
 * Function to decode NMEA frames with Sentence ID "RMC"
 * 
 * This function decodes NMEA frames with Sentence ID "RMC". The data is stored in the given 
 * structure. For conversion/resolution information of the stored data, please refer to the
 * TYPE_GNSS_RMC structure declaration above.
 * 
 * --------------------------------------------------------------------
 * NMEA RMC sentence:
 * 
 * 1    = UTC of position fix
 * 2    = Data status (V=navigation receiver warning)
 * 3    = Latitude of fix
 * 4    = N or S
 * 5    = Longitude of fix
 * 6    = E or W
 * 7    = Speed over ground in knots
 * 8    = Track made good in degrees True
 * 9    = UTC date
 * 10   = Magnetic variation degrees (Easterly var. subtracts from true course)
 * 11   = E or W
 * 12   = Mode Indicator N=No Fix, E=Estimated/Dead Reckoning Fix, A=Autonomous GNSS Fix, D=Differential GNSS Fix
 * 13   = Checksum
 * --------------------------------------------------------------------
 *
 * @param sFrame[]:u8                        - String that contains NMEA frame
 * @param gaGNSS_RMC_item:TYPE_GNSS_RMC      - Structure that contains GNSS RMC data
 */
stock GNSS_NMEADecodeRMC(const sFrame{}, gaGNSS_RMC_item[TYPE_GNSS_RMC])
{
  new sToken{GNSS_MAX_TOKEN_LENGTH};
  new iIdx = 0;

#if GNSS_DEBUG >= 2
  printf("%s\n", sFrame);
#endif
  
  gaGNSS_RMC_item.Available = 1;
  
  /* Talker ID + Sentence ID */
  GNSS_StrTok(sFrame, sToken, iIdx);
#if GNSS_DEBUG >= 1
  printf("%s:\r\n", sToken);
#endif
  
  /* UTC of position fix */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  UTC of position fix = %s\r\n", sToken);
#endif
    gaGNSS_RMC_item.UTC = atoi(sToken);
  }
  else
  {
    gaGNSS_RMC_item.UTC = 0;
  }
  
  /* Data status  */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Data status  = %s\r\n", sToken);
#endif
    gaGNSS_RMC_item.Status{0} = sToken{0};
  }
  else
  {
    gaGNSS_RMC_item.Status{0} = '0';
  }

  /* Latitude of fix */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
    new iDegrees;
    new iMinutes;

    iDegrees  = (sToken{0} - '0') * 10;
    iDegrees += (sToken{1} - '0');

    iMinutes  = (sToken{2} - '0') * 100000;
    iMinutes += (sToken{3} - '0') * 10000;
    iMinutes += (sToken{5} - '0') * 1000;
    iMinutes += (sToken{6} - '0') * 100;
    iMinutes += (sToken{7} - '0') * 10;
    iMinutes += (sToken{8} - '0');

    gaGNSS_RMC_item.Latitude = iDegrees * 1000000;
    gaGNSS_RMC_item.Latitude += (iMinutes * 100) / 60;
  }
  else
  {
    gaGNSS_RMC_item.Latitude = cellmax;
  }
  
  /* N/S indicator */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} == 'S')
  {
    gaGNSS_RMC_item.Latitude *= -1;
  }
#if GNSS_DEBUG >= 1
  printf("  Lat = %d\r\n", gaGNSS_RMC_item.Latitude);
#endif
  
  /* Longitude of fix */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
    new iDegrees;
    new iMinutes;

    iDegrees  = (sToken{0} - '0') * 100;
    iDegrees += (sToken{1} - '0') * 10;
    iDegrees += (sToken{2} - '0');

    iMinutes  = (sToken{3} - '0') * 100000;
    iMinutes += (sToken{4} - '0') * 10000;
    iMinutes += (sToken{6} - '0') * 1000;
    iMinutes += (sToken{7} - '0') * 100;
    iMinutes += (sToken{8} - '0') * 10;
    iMinutes += (sToken{9} - '0');

    gaGNSS_RMC_item.Longitude = iDegrees * 1000000;
    gaGNSS_RMC_item.Longitude += (iMinutes * 100) / 60;
  }
  else
  {
    gaGNSS_RMC_item.Longitude = cellmax;
  }

  /* E/W indicator */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} == 'W')
  {
    gaGNSS_RMC_item.Longitude *= -1;
  }
#if GNSS_DEBUG >= 1
  printf("  Long = %d\r\n", gaGNSS_RMC_item.Longitude);
#endif

  /* Speed over ground in knots */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Speed over ground in knots = %s\r\n", sToken);
#endif
    gaGNSS_RMC_item.SpeedOverGround = atof(sToken) * 1000.0;
  }
  else
  {
    gaGNSS_RMC_item.SpeedOverGround = 0;
  }
  /* Track made good in degrees True */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Track made good in degrees True = %s\r\n", sToken);
#endif
    gaGNSS_RMC_item.CourseOverGround = atof(sToken) * 10.0;
  }
  else
  {
    gaGNSS_RMC_item.CourseOverGround = 0;
  }
  
  /* UTC date */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  UTC date = %s\r\n", sToken);
#endif
    gaGNSS_RMC_item.UTCdate = atoi(sToken);
  }
  else
  {
    gaGNSS_RMC_item.UTCdate = 0;
  }
  
  /* Magnetic variation degrees */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
    gaGNSS_RMC_item.MagVar = atof(sToken) * 10.0;
#if GNSS_DEBUG >= 1
    printf("  Magnetic variation degrees  = %s\r\n", sToken);
#endif
  }
  else
  {
    gaGNSS_RMC_item.MagVar = cellmax;
  }
  
  /* E/W */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  E/W = %s\r\n", sToken{0});
#endif
	  
    if(sToken{0} == 'W')
    {
      // Magnetic variation scale goes from -180.0 (west) to +180.0 (east)
      gaGNSS_RMC_item.MagVar *= -1;
    }
  }

  /* Mode indicator */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Mode Indicator = %s\r\n", sToken);
#endif
    gaGNSS_RMC_item.Mode{0} = sToken{0};
  }
  else
  {
    gaGNSS_RMC_item.Mode{0} = '0';
  }
}

/**
 * Function to decode NMEA frames with Sentence ID "GGA"
 * 
 * This function decodes NMEA frames with Sentence ID "GGA". The data is stored in the given 
 * structure. For conversion/resolution information of the stored data, please refer to the
 * TYPE_GNSS_GGA structure declaration above.
 * 
 * --------------------------------------------------------------------
 * NMEA GGA sentence:
 * 
 * 1    = UTC of Position
 * 2    = Latitude
 * 3    = N or S
 * 4    = Longitude
 * 5    = E or W
 * 6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
 * 7    = Number of satellites in use [not those in view]
 * 8    = Horizontal dilution of position
 * 9    = Antenna altitude above/below mean sea level (geoid)
 * 10   = Meters  (Antenna height unit)
 * 11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and
          mean sea level.  -=geoid is below WGS-84 ellipsoid)
 * 12   = Meters  (Units of geoidal separation)
 * 13   = Age in seconds since last update from diff. reference station
 * 14   = Diff. reference station ID#
 * 15   = Checksum
 * --------------------------------------------------------------------
 *
 * @param sFrame[]:u8                        - String that contains NMEA frame
 * @param gaGNSS_GGA_item:TYPE_GNSS_GGA      - Structure that contains GNSS GGA data
 */
stock GNSS_NMEADecodeGGA(const sFrame{}, gaGNSS_GGA_item[TYPE_GNSS_GGA])
{
  new sToken{GNSS_MAX_TOKEN_LENGTH};
  new iIdx = 0;

#if GNSS_DEBUG >= 2
  printf("%s\n", sFrame);
#endif
  
  gaGNSS_GGA_item.Available = 1;
  
  /* Talker ID + Sentence ID */
  GNSS_StrTok(sFrame, sToken, iIdx);
#if GNSS_DEBUG >= 1
  printf("%s:\r\n", sToken);
#endif

  /* UTC Time */
  GNSS_StrTok(sFrame, sToken, iIdx);
  
  gaGNSS_GGA_item.UTC = atoi(sToken);
  
#if GNSS_DEBUG >= 1
  printf("  UTC = %s\r\n", sToken);
#endif

  /* Latitude ddmm.mmmm */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
    new iDegrees;
    new iMinutes;

    iDegrees  = (sToken{0} - '0') * 10;
    iDegrees += (sToken{1} - '0');

    iMinutes  = (sToken{2} - '0') * 100000;
    iMinutes += (sToken{3} - '0') * 10000;
    iMinutes += (sToken{5} - '0') * 1000;
    iMinutes += (sToken{6} - '0') * 100;
    iMinutes += (sToken{7} - '0') * 10;
    iMinutes += (sToken{8} - '0');

    gaGNSS_GGA_item.Latitude = iDegrees * 1000000;
    gaGNSS_GGA_item.Latitude += (iMinutes * 100) / 60;
  }

  /* N/S indicator */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} == 'S')
  {
    gaGNSS_GGA_item.Latitude *= -1;
  }
#if GNSS_DEBUG >= 1
  printf("  Lat = %d\r\n", gaGNSS_GGA_item.Latitude);
#endif

  /* Longitude dddmm.mmmm */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
    new iDegrees;
    new iMinutes;

    iDegrees  = (sToken{0} - '0') * 100;
    iDegrees += (sToken{1} - '0') * 10;
    iDegrees += (sToken{2} - '0');

    iMinutes  = (sToken{3} - '0') * 100000;
    iMinutes += (sToken{4} - '0') * 10000;
    iMinutes += (sToken{6} - '0') * 1000;
    iMinutes += (sToken{7} - '0') * 100;
    iMinutes += (sToken{8} - '0') * 10;
    iMinutes += (sToken{9} - '0');

    gaGNSS_GGA_item.Longitude = iDegrees * 1000000;
    gaGNSS_GGA_item.Longitude += (iMinutes * 100) / 60;
  }

  /* E/W indicator */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} == 'W')
  {
    gaGNSS_GGA_item.Longitude *= -1;
  }
#if GNSS_DEBUG >= 1
  printf("  Long = %d\r\n", gaGNSS_GGA_item.Longitude);
#endif

  /* Position fix indicator */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  PosFixInd = %s\r\n", sToken);
#endif

    gaGNSS_GGA_item.PosFixInd = atoi(sToken);
  }
  else
  {
    /* No position fix */
    gaGNSS_GGA_item.PosFixInd = 0;
    gaGNSS_GGA_item.Latitude = cellmax;
    gaGNSS_GGA_item.Longitude = cellmax;
  }

  /* Satellites used */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  SatUsed = %s\r\n", sToken);
#endif

    gaGNSS_GGA_item.SatUsed = atoi(sToken);
  }
  else
  {
    gaGNSS_GGA_item.SatUsed = cellmax;
  }
  
  /* HDOP */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  HDOP = %s\r\n", sToken);
#endif

    gaGNSS_GGA_item.HPrecision = atof(sToken) * 10.0;
  }
  else
  {
    gaGNSS_GGA_item.HPrecision = cellmax;
  }

  /* MSL altitude */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Alt = %s\r\n", sToken);
#endif

    gaGNSS_GGA_item.Altitude = atof(sToken) * 10.0;
  }
  else
  {
    gaGNSS_GGA_item.Altitude = cellmax;
  }

  /* Antenna height unit */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Antenna height unit = %s\r\n", sToken);
#endif
  }
  
  /* Geoidal separation */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Geoidal separation = %s\r\n", sToken);
#endif
  
    gaGNSS_GGA_item.GeoSep = atof(sToken) * 10.0;
  }
  else
  {
    gaGNSS_GGA_item.GeoSep = cellmax;
  }
  
  /* Units of geoidal separation */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  Units of geoidal separation = %s\r\n", sToken);
#endif
  }
  
  /* DGNSS age */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  DGNSS age = %s\r\n", sToken);
#endif
    gaGNSS_GGA_item.DGNSSage = atof(sToken) * 10.0;
  }
  else
  {
    gaGNSS_GGA_item.DGNSSage = 0;
  }
  
  /* DGNSS ID */
  GNSS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GNSS_DEBUG >= 1
    printf("  DGNSS ID = %s\r\n", sToken);
#endif
    gaGNSS_GGA_item.DGNSSage = atoi(sToken);
  }
  else
  {
    gaGNSS_GGA_item.DGNSSID = 0;
  }
}

/**
 * Function to get the Talker ID of a NMEA frame or a token
 * 
 * @param sString[]:u8     - Full NMEA frame or first token of an NMEA frame (including '$')
 * @param sTalkerID[]:u8   - Requested Talker ID
 * @return s32             -  0: Success
 *                         - -1: String is too short to be a token
 *                         - -2: String does not start with '$'
 */
stock GNSS_GetTalkerID(const sString{}, sTakerID{})
{
  if(strlen(sString) < 6) return -1;
  if(sString{0} != '$') return -2;

  sTakerID{0} = sString{1};
  sTakerID{1} = sString{2};
  
  return 0;
}

/**
 * Function to get the Sentence ID of a NMEA frame or a token
 * 
 * @param sString[]:u8     - Full NMEA frame or first token of an NMEA frame (including '$')
 * @param sSentenceID[]:u8 - Requested Sentence ID
 * @return s32             -  0: Success
 *                         - -1: String is too short to be a token
 *                         - -2: String does not start with '$'
 */
stock GNSS_GetSentenceID(const sString{}, sSentenceID{})
{
  if(strlen(sString) < 6) return -1;
  if(sString{0} != '$') return -2;

  sSentenceID{0} = sString{3};
  sSentenceID{1} = sString{4};
  sSentenceID{2} = sString{5};
  
  return 0;
}

/**
 * Function to print a NMEA frame to the screen
 * 
 * @param sFrame[]:u8      - NMEA frame
 * @return s32             -  0: Success
 *                         - -1: String is too long
 *                         - -2: String does not start with '$'
 */
stock GNSS_PrintNMEAframe(const sFrame{})
{
  if(strlen(sFrame) > GNSS_MAX_FRAME_LENGTH) return -1;
  if(sFrame{0} != '$') return -2;
  
  new sString{256};
  sprintf(sString, sizeof(sString), "GNSS: %s\r\n", sFrame);
  print(sString);
  
  return 0;
}

/**
 * Function that returns GNSS GSV information
 * 
 * The variable iIndex indicates the position of the array gaGNSS_GSV in which the data is located.
 * GNSS_SAT_COUNT represents the number of SV structures in total and therefore iIndex must be
 * smaller. If the PRN number of the requested SV structure is 0, no satellite data is available
 * and in further consequence an error is returned. Note that there is only data available if
 * any NMEA frames including a "GSV" Sentence ID have been decoded previously.
 * 
 * @param iIndex:s32                        - Index for SV state access
 * @param gaGNSS_GSVitem:TYPE_GNSS_GSV      - Structure that contains SV state information
 * @return s32                              -  0: Success
 *                                          - -1: Index for SV state access is too high
 *                                          - -2: Requested SV structure does not have a valid PRN
 *                                                and as a consequence does not contain valid data
 */
stock GNSS_GetInfoGSV(iIndex, gaGNSS_GSVitem[TYPE_GNSS_GSV])
{
  if(iIndex < GNSS_SAT_COUNT)
  {
    gaGNSS_GSVitem = gaGNSS_GSV[iIndex];
    if(gaGNSS_GSVitem.SvPrnNumber != 0) return 0;
    else return -2;
  }
  
  return -1;
}

/**
 * Function that starts the saving process of a frame
 */
stock GNSS_SaveNextFrame()
{
  iStateSave = GNSS_SAVE_START;
}

/**
 * Function that saves a frame
 * 
 * This function firstly prints the given frame to the console and secondly saves it in the
 * internal flash memory.
 * 
 * @param sFrame[]:u8 - String that contains NMEA frame
 */
stock GNSS_SaveFrame(const sFrame{})
{
  printf("%s (%d)\r\n", sFrame, strlen(sFrame));
  
  rM2M_RecData(0, sFrame, GNSS_MAX_FRAME_LENGTH);
}
