/*******************************************************************************
**                  _     _  __  __ ___  __  __
**                 (_)   | ||  \/  |__ \|  \/  |
**  _ __ __ _ _ __  _  __| || \  / |  ) | \  / |
** | '__/ _` | '_ \| |/ _` || |\/| | / /| |\/| |
** | | | (_| | |_) | | (_| || |  | |/ /_| |  | |
** |_|  \__,_| .__/|_|\__,_||_|  |_|____|_|  |_|
**           | |
**           |_|
**
********************************************************************************
**  GPS interface functions
**
**  Geräteklasse:  rapidM2M
**  API Level:     1
** 
**  Author:        Gregor Bader @ Microtrionics
**  Version:       1.00
*******************************************************************************/

#include <string>

#define GPS_DEBUG           (0)
#define GPS_DEBUG_SAVEFRAME (1)

#define GPS_USE_RMC         (0)
//#define GPS_USE_VTG 1
//#define GPS_USE_GGA 1

forward public GPS_UartRx(const data{}, len);

#define TYPE_GPS_ITEM[.Latitude,            // []
                      .Longitude,           // []
                      .PosFixInd,           // []
                      .SatUsed,             // []
                      .Available,           // []
                      .HPrecision,          // []
                      .Altitude,            // [0.1 m]
//                      .SpeedOverGroundRMC,  // [0.001 kn]
//                      .CourseOverGroundRMC, // [0.1 degree]
                      .SpeedOverGround_kn,  // [0.001 kn]
                      .SpeedOverGround_kph, // [0.001 kph]
                      .CourseOverGround     // [0.1 degree]
                      ]

#define TYPE_GPS_ITEM_SAT[.SvPrnNumber,   // [] SV PRN number
                          .Elevation,     // [Degrees] Elevation in degrees, 90 maximum
                          .Azimuth,       // [Degrees] Azimuth, degrees from true north, 000 to 359
                          .Snr]           // [dB] SNR, 00-99 dB (null when not tracking)


/* used constants */
const
{
  GPS_MAX_FRAME_LENGTH = 100,   /* maximum supproted NMEA frame length */
  GPS_MAX_TOKEN_LENGTH = 20,    /* maximum length of a NMEA token */
}

const
{
  GPS_SAVE_STOP = 0,
  GPS_SAVE_START,
  GPS_SAVE_RUN,
}

#if GPS_DEBUG_SAVEFRAME != 0
  static iStateSave = GPS_SAVE_STOP;
#endif

static sNMEAFrame{GPS_MAX_FRAME_LENGTH+1};
static iFrameIndex=0;

static iGPS_Uart = -1;

static gaGPS[TYPE_GPS_ITEM];
const GPS_SAT_COUNT = 20;
static gaGPS_SV[GPS_SAT_COUNT][TYPE_GPS_ITEM_SAT];

stock GPS_Init(Uart = -1)
{
  new iIdx;
  new iResult;
  
  iGPS_Uart = Uart;
  if(Uart != -1)
  {
    /* init Uart #1 with 9600 baud 8N1 */
    iIdx = funcidx("GPS_UartRx");
    iResult = rM2M_UartInit(iGPS_Uart, 9600, RM2M_UART_8_DATABIT|RM2M_UART_PARITY_NONE|RM2M_UART_1_STOPBIT, iIdx);
    printf("rM2M_UartInit(%d) = %d\r\n", iIdx, iResult);
  }
}

stock GPS_Close()
{
  rM2M_UartClose(iGPS_Uart);
  iGPS_Uart = -1;
  gaGPS.Available = 0;
}

/* function to tokenize NMEA frame */
stock GPS_StrTok(const string{}, sResult{}, &index)
{
  new iChar;
  new iLen = 0;

  iChar = string{index};
  while((iChar != ',') && (iChar != '*') && (iChar != '\0') &&
        (iLen < GPS_MAX_TOKEN_LENGTH))
  {
    sResult{iLen++} = iChar;
    iChar = string{++index};
  }
  sResult{iLen} = '\0';

  /* set index to next character after ',' */
  if(iChar != '\0') ++index;
}

/* verifies NMEA frame checksum */
stock GPS_NMEAIsChecksumValid(const frame{})
{
  new iIndex=0;
  new iChecksum;
  new iReceivedChecksum;
  new sReceivedChecksum{3};

  /* check start character */
  if(frame{iIndex++} != '$') 
    return(-1);

  /* calculate checksum */
  iChecksum = 0;
  while(frame{iIndex} != '*')
  {
    if((frame{iIndex} < 0x20) || (frame{iIndex} > 0x7F) ) 
      return(-2);
    iChecksum ^= frame{iIndex++};
  }

  iIndex++;
  sReceivedChecksum{0} = frame{iIndex++};
  sReceivedChecksum{1} = frame{iIndex++};
  sReceivedChecksum{2} = 0;
  iReceivedChecksum = strtol(sReceivedChecksum, 16);

  /* compare calculated checksum with received checksum */
  if(iChecksum == iReceivedChecksum) 
    return(0);

  /* checksum wrong */
  return(-3);
}

/*
1    = Total number of messages of this type in this cycle
2    = Message number
3    = Total number of SVs in view
4    = SV PRN number
5    = Elevation in degrees, 90 maximum
6    = Azimuth, degrees from true north, 000 to 359
7    = SNR, 00-99 dB (null when not tracking)
8-11 = Information about second SV, same as field 4-7
12-15= Information about third SV, same as field 4-7
16-19= Information about fourth SV, same as field 4-7
*/
stock GPS_NMEADecodeGSV(const sFrame{})
{
  new sToken{GPS_MAX_TOKEN_LENGTH};
  new iIdx = 0;
  new aItem[TYPE_GPS_ITEM_SAT];
  new iMsgIndex;
  
#if GPS_DEBUG >= 2
  printf("%s\n", sFrame);
#endif

  /* message ID $GPRMC */
  GPS_StrTok(sFrame, sToken, iIdx);
#if GPS_DEBUG >= 1
  printf("%s:\r\n", sToken);
#endif

  /* Total number of messages */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Messages Count = %s\r\n", sToken);
#endif
  }

  /* Total number of messages */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Message number = %s\r\n", sToken);
#endif
    iMsgIndex = atoi(sToken) - 1; // 0 - based Index
  }
  
  /* Total number of SVs in view */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Count SV = %s\r\n", sToken);
#endif
  }

  for(new iSVIdx = 0; iSVIdx < 4; iSVIdx++)
  {
    /* SV PRN number */
    GPS_StrTok(sFrame, sToken, iIdx);
    if(sToken{0} != '\0')
    {
#if GPS_DEBUG >= 1
      printf("  SvPrnNumber = %s\r\n", sToken);
#endif
      aItem.SvPrnNumber = atoi(sToken);
    }
    else
    {
      aItem.SvPrnNumber = 0;
    }
    
    /* Elevation */
    GPS_StrTok(sFrame, sToken, iIdx);
    if(sToken{0} != '\0')
    {
#if GPS_DEBUG >= 1
      printf("  Elevation = %s\r\n", sToken);
#endif
      aItem.Elevation = atoi(sToken);
    }
    else
    {
      aItem.Elevation = 0;
    }
    
    /* Azimuth */
    GPS_StrTok(sFrame, sToken, iIdx);
    if(sToken{0} != '\0')
    {
#if GPS_DEBUG >= 1
      printf("  Azimuth = %s\r\n", sToken);
#endif
      aItem.Azimuth = atoi(sToken);
    }
    else
    {
      aItem.Azimuth = 0;
    }

    /* SNR */
    GPS_StrTok(sFrame, sToken, iIdx);
    if(sToken{0} != '\0')
    {
#if GPS_DEBUG >= 1
      printf("  SNR = %s\r\n", sToken);
#endif
      aItem.Snr = atoi(sToken);
    }
    else
    {
      aItem.Snr = 0;
    }
    
    if(iMsgIndex * 4 + iSVIdx < GPS_SAT_COUNT)
    {
      gaGPS_SV[iMsgIndex * 4 + iSVIdx] = aItem;
      if(aItem.SvPrnNumber == 0)
      {
        break;
      }
    }
  }
}


/* decode NMEA $GPVTG frame */
/*
1    = Course over ground (true north)
2    = Fixed text 'T' indicates that Course over ground is relative to true north
3    = Course over ground (magnetic)
4    = Fixed text 'M' indicates that Course over ground relative to magnetic north
5    = Speed over ground in knots
6    = Fixed text 'N' indicates that speed over ground in in knots
7    = Speed over ground in kilometers/hour
8    = Fixed text 'K' indicates that speed over ground is in kilometers/hour
9    = Mode Indicator N=No Fix, E=Estimated/Dead Reckoning Fix, A=Autonomous GNSS Fix, D=Differential GNSS Fix
10   = Checksum
*/
stock GPS_NMEADecodeVTG(const sFrame{}, &iSpeedOverGround_kn, &iSpeedOverGround_kph, &iCourseOverGround)
{
  new sToken{GPS_MAX_TOKEN_LENGTH};
  new iIdx = 0;

#if GPS_DEBUG >= 2
  printf("%s\n", sFrame);
#endif

  /* message ID $GPVTG */
  GPS_StrTok(sFrame, sToken, iIdx);
#if GPS_DEBUG >= 1
  printf("%s:\r\n", sToken);
#endif
  
  /* Course over ground (true north) */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Course over ground (true north) = %s\r\n", sToken);
#endif
    iCourseOverGround = atof(sToken) * 10.0
  }
  else
  {
    iCourseOverGround = 0;
  }
  
  /* Fixed text 'T' indicates that Course over ground is relative to true north  */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Fixed text 'T' = %s\r\n", sToken);
#endif
  }

  /* Course over ground (Magnetic) */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Course over ground (magnetic) = %s\r\n", sToken);
#endif
  }
  
  /* Fixed text 'M' indicates that Course over ground is relative to magnetic north  */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Fixed text 'M' = %s\r\n", sToken);
#endif
  }

  /* Speed over ground in knots */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Speed over ground in knots = %s\r\n", sToken);
#endif
    iSpeedOverGround_kn = atof(sToken) * 1000.0;
  }
  else
  {
    iSpeedOverGround_kn = 0;
  }
  
  /* Fixed text 'N' indicates that speed over ground in in knots  */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Fixed text 'N' = %s\r\n", sToken);
#endif
  }

  /* Speed over ground in kilometers/hour */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Speed over ground in kph = %s\r\n", sToken);
#endif
    
    iSpeedOverGround_kph = atof(sToken) * 1000.0;
  }
  else
  {
    iSpeedOverGround_kph = 0;
  }
  
  /* Fixed text 'K' indicates that speed over ground is in kilometers/hour */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Fixed text 'K' = %s\r\n", sToken);
#endif
  }
  
  /* Mode Indicator */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Mode Indicator = %s\r\n", sToken);
#endif
  }
}

#if GPS_USE_RMC
/* decode NMEA $GPRMC frame */
/*
1    = UTC of position fix
2    = Data status (V=navigation receiver warning)
3    = Latitude of fix
4    = N or S
5    = Longitude of fix
6    = E or W
7    = Speed over ground in knots
8    = Track made good in degrees True
9    = UT date
10   = Magnetic variation degrees (Easterly var. subtracts from true course)
11   = E or W
12   = Checksum
*/
stock GPS_NMEADecodeRMC(const sFrame{}, &iSpeedOverGround, &iCourseOverGround)
{
  new sToken{GPS_MAX_TOKEN_LENGTH};
  new iIdx = 0;

#if GPS_DEBUG >= 2
  printf("%s\n", sFrame);
#endif

  /* message ID $GPRMC */
  GPS_StrTok(sFrame, sToken, iIdx);
#if GPS_DEBUG >= 1
  printf("%s:\r\n", sToken);
#endif
  
  /* UTC of position fix */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  UTC of position fix = %s\r\n", sToken);
#endif
  }
  
  /* Data status  */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Data status  = %s\r\n", sToken);
#endif
  }
  
  /* Latitude of fix */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Latitude of fix = %s\r\n", sToken);
#endif
  }
  
  /* N/S */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  N/S = %s\r\n", sToken);
#endif
  }
  
  /* Longitude of fix */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Longitude of fix = %s\r\n", sToken);
#endif
  }
  
  /* E/W */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  E/W = %s\r\n", sToken);
#endif
  }
  
  /* Speed over ground in knots */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Speed over ground in knots = %s\r\n", sToken);
#endif
    iSpeedOverGround = atof(sToken) * 1000.0;
  }
  else
  {
    iSpeedOverGround = 0;
  }
  /* Track made good in degrees True */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Track made good in degrees True = %s\r\n", sToken);
#endif
    iCourseOverGround = atof(sToken) * 10.0;
  }
  else
  {
    iCourseOverGround = 0;
  }
  /* UT date */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  UT date = %s\r\n", sToken);
#endif
  }
  
  /* Magnetic variation degrees  */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
  printf("  Magnetic variation degrees  = %s\r\n", sToken);
#endif
  }
  
  /* E/W  */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  E/W = %s\r\n", sToken);
#endif
  }
}
#endif

/* decode NMEA $GPGGA frame */
/*
1    = UTC of Position
2    = Latitude
3    = N or S
4    = Longitude
5    = E or W
6    = GPS quality indicator (0=invalid; 1=GPS fix; 2=Diff. GPS fix)
7    = Number of satellites in use [not those in view]
8    = Horizontal dilution of position
9    = Antenna altitude above/below mean sea level (geoid)
10   = Meters  (Antenna height unit)
11   = Geoidal separation (Diff. between WGS-84 earth ellipsoid and
       mean sea level.  -=geoid is below WGS-84 ellipsoid)
12   = Meters  (Units of geoidal separation)
13   = Age in seconds since last update from diff. reference station
14   = Diff. reference station ID#
15   = Checksum
*/
stock GPS_NMEADecodeGGA(const sFrame{}, &iLat, &iLong, &iFix, &iSatUsed, &iHPrecision, &iAltitude)
{
  new sToken{GPS_MAX_TOKEN_LENGTH};
  new iIdx = 0;

#if GPS_DEBUG >= 2
  printf("%s\n", sFrame);
#endif

  /* message ID $GPGGA */
  GPS_StrTok(sFrame, sToken, iIdx);
#if GPS_DEBUG >= 1
  printf("%s:\r\n", sToken);
#endif

  /* UTC Time */
  GPS_StrTok(sFrame, sToken, iIdx);
#if GPS_DEBUG >= 1
  printf("  UTC = %s\r\n", sToken);
#endif

  /* Latitude ddmm.mmmm */
  GPS_StrTok(sFrame, sToken, iIdx);
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

    iLat = iDegrees * 1000000;
    iLat += (iMinutes * 100) / 60;
  }

  /* N/S Indicator */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} == 'S')
  {
    iLat *= -1;
  }
#if GPS_DEBUG >= 1
  printf("  Lat = %d\r\n", iLat);
#endif

  /* Longitude dddmm.mmmm */
  GPS_StrTok(sFrame, sToken, iIdx);
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

    iLong = iDegrees * 1000000;
    iLong += (iMinutes * 100) / 60;
  }

  /* E/W Indicator */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} == 'W')
  {
    iLong *= -1;
  }
#if GPS_DEBUG >= 1
  printf("  Long = %d\r\n", iLong);
#endif

  /* Position Fix Indicator */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  PosFixInd = %s\r\n", sToken);
#endif

    iFix = atoi(sToken);
  }
  else
  {
    /* no position fix */
    iFix = 0;
    iLat = cellmax;
    iLong = cellmax;
  }

  /* Satellites Used */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  SatUsed = %s\r\n", sToken);
#endif

    iSatUsed = atoi(sToken);
  }

  /* HDOP */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  HDOP = %s\r\n", sToken);
#endif

    iHPrecision = atoi(sToken);
  }

  /* MSL Altitude */
  GPS_StrTok(sFrame, sToken, iIdx);
  if(sToken{0} != '\0')
  {
#if GPS_DEBUG >= 1
    printf("  Alt = %s\r\n", sToken);
#endif

    iAltitude = atof(sToken) * 10.0;
  }
}

/* Uart receive callback */
public GPS_UartRx(const data{}, len)
{
  new iIndex=0;
  new iCurrentChar=0;

  /* print received data */
#if GPS_DEBUG >= 3
  printf("GPS_UartRx(\"%s\", %d)\r\n", data, len);
#endif

  while(iIndex < len)
  {
    iCurrentChar = data{iIndex++};

    /* reset frame on start character */
    if(iCurrentChar == '$') iFrameIndex = 0;
    /* process frame on end of line */
    else if(iCurrentChar == '\r' || iCurrentChar == '\n' || iFrameIndex == 79)
    {
      /* zero terminate string */
      sNMEAFrame{iFrameIndex} = '\0';

      /* verify checksum */
      if(GPS_NMEAIsChecksumValid(sNMEAFrame) >= 0)
      {
        new sToken{GPS_MAX_TOKEN_LENGTH};
        new iIdx = 0;

        GPS_StrTok(sNMEAFrame, sToken, iIdx);

        /* use only GGA frames */
        if(strcmp(sToken, "$GPGGA") == 0)
        {
          /* mark GPS available and decode frame */
          gaGPS.Available = 1;
          GPS_NMEADecodeGGA(sNMEAFrame,
                            gaGPS.Latitude,
                            gaGPS.Longitude,
                            gaGPS.PosFixInd,
                            gaGPS.SatUsed,
                            gaGPS.HPrecision,
                            gaGPS.Altitude);
        }
        else if (strcmp(sToken, "$GPRMC") == 0)
        { // First Frame that will be sent.
#if GPS_USE_RMC != 0
          GPS_NMEADecodeRMC(sNMEAFrame,
                            gaGPS.SpeedOverGroundRMC,
                            gaGPS.CourseOverGroundRMC);
#endif
#if GPS_DEBUG_SAVEFRAME != 0
          if(iStateSave == GPS_SAVE_RUN) //Stop Saving Frames
            iStateSave = GPS_SAVE_STOP;
          else if(iStateSave == GPS_SAVE_START) //Start Saving GPS Frames
            iStateSave = GPS_SAVE_RUN;
#endif
        }
        else if (strcmp(sToken, "$GPVTG") == 0)
        {
          GPS_NMEADecodeVTG(sNMEAFrame,
                            gaGPS.SpeedOverGround_kn,
                            gaGPS.SpeedOverGround_kph,
                            gaGPS.CourseOverGround);
        }
        else if(strcmp(sToken, "$GPGSV") == 0)
        {
          GPS_NMEADecodeGSV(sNMEAFrame);        
        }
        else if(strcmp(sToken, "$GPGSV") == 0)
        {
          GPS_NMEADecodeGSV(sNMEAFrame);        
        }
        
#if GPS_DEBUG_SAVEFRAME != 0
        // Save GPS Frame
        if(iStateSave == GPS_SAVE_RUN)
        {
          GPS_SaveFrame(sNMEAFrame);
        }
#endif
      }

      /* reset frame index -> start new NMEA frame */
      iFrameIndex = 0;
    }
    if(iFrameIndex < GPS_MAX_FRAME_LENGTH)
      sNMEAFrame{iFrameIndex++} = iCurrentChar;
  }
}

stock GPS_GetInfo()
{
  return gaGPS;
}

stock GPS_GetInfoSV(iIndex, aGPS_SV[TYPE_GPS_ITEM_SAT])
{
  if(iIndex < GPS_SAT_COUNT)
  {
    aGPS_SV = gaGPS_SV[iIndex];
    if(aGPS_SV.SvPrnNumber != 0)
      return OK;
  }
  
  return ERROR
}

#if GPS_DEBUG_SAVEFRAME != 0
stock GPS_SaveNextFrame()
{
  iStateSave = GPS_SAVE_START;
}

stock GPS_SaveFrame(const sFrame{})
{
  printf("%s (%d)\r\n", sFrame, strlen(sFrame));
  
  rM2M_RecData(0, sFrame, GPS_MAX_FRAME_LENGTH);
}
#endif
