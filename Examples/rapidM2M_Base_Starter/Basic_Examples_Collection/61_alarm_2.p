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
 * Extended "Alarm" Example
 *
 * Simulates and records (record interval) a temperature value that changes between 19°C and 31°C in 1°C steps.  
 * The measurement data generated this way is then transmitted periodically (transmission interval) to the server. 
 * If the temperature is greater than or equal to 25°C, an alarm is triggered. Once an alarm has been triggered and the 
 * temperature falls to or below 25°C - 5% (i.e. 23,75°C) again, the alarm is released. In both cases (when triggering  
 * or releasing the alarm) an alarm record is generated and transmitted to the server immediately.
 * 
 * 
 * Compatible with every rapidM2M hardware
 *
 * @version 20190613
 */
 
/* forward declarations of public functions */
forward public Timer1s();                  // Called up 1x per sec. for the program sequence

const
{
  AL_FLG_WARNING   = 0b00000001,           // Warning
  AL_FLG_ALARM     = 0b00000010,           // Alarm
  AL_FLG_FAULTLOW  = 0b00000100,           // Technical error of low priority
  AL_FLG_FAULTHIGH = 0b00001000,           // Technical error of high priority
  AL_FLG_UNDERFLOW = 0b10000000,           // Underflow of value (has to be set when value is below alarm level)
                                           // Bit set:     alarm/warning, as value <= level 
										   // Bit cleared: alarm/warning, as value >= level
}
 
const
{
  AL_ALARM               = 0x20,           //"Split-tag" for the alarm data record (has to be 0x20)
  AL_SIZE                = 13,             //Size of the alarm data record (has to be 13 Byte)
}

const
{
  INTERVAL_RECORD = 60,                    // Interval of record [s]
  INTERVAL_TX     = 30 * 60,               // Interval of transmission [s]
  
  SIZE_RECDATA    = 1 + 4,                 // Measurement data block consists of: "Split-tag" (u8) + Temperature (f32)
}

const Float:fAlarmThreshold = 25.0;        // If the temperature exceeds this threshold, an alarm is triggered.
const Float:fHysteresis     = 5.0;         // Hysteresis for releasing an active alarm [%]
                                           // Once an alarm has been triggered, the alarm is released if the temperature 
                                           // falls below "fAlarmThreshold - Abs(fAlarmThreshold * (fHysteresis/100) )" 


/* Global variables for the remaining time until certain actions are triggered */
static iRecTimer;                          // Sec. until the next recording
static iTxTimer;                           // Sec. until the next transmission 


static Float:fTemperature = 20.0;          // Global variables for the current temperature

/* application entry point */
main()
{
  /* Temporary memory for the index of a public function and the return value of a function             */  
  new iIdx, iResult;

  /* Initialisation of a cyclic 1 sec. timer
     - Determining the function index that should be executed 1x per sec.
     - Transferring the index to the system
	 - In case of a problem the index and return value of the init function are issued by the console   */
  iIdx = funcidx("Timer1s");
  iResult = rM2M_TimerAdd(iIdx);
  printf("rM2M_TimerAdd(%d) = %d\r\n", iIdx, iResult);
  
  iRecTimer = INTERVAL_RECORD;                // Sets counter variable to defined record interval
  iTxTimer  = INTERVAL_TX;                    // Sets counter variable to defined transmission interval
  
  Al_SetAlarm(0, 0, 0, 0.0, fAlarmThreshold); // Clear alarm on start
  rM2M_TxStart();                             // Initiates a connection to the server
}

/* 1 sec. timer is used for the general program sequence */
public Timer1s()
{
  iRecTimer--;                                // Counter counting down the sec. to the next recording
  if(iRecTimer <= 0)                          // When the counter has expired -> 
  {
    UpdateTemperature();                      // Calls up the function to update the current temperature
    RecordData();                             // Calls up the function to record the data 
    CheckAlarm();                             // Calls up the function which checks whether an alarm has to be triggered 
    iRecTimer = INTERVAL_RECORD;              // Resets counter variable to defined record interval
  }
  
  iTxTimer--;                                 // Counter counting down the sec. to the next transmission
  if(iTxTimer <= 0)                           // When the counter has expired -> 
  {
    print("Start Transmission\r\n");
    rM2M_TxStart();                           // Initiates a connection to the server 
    iTxTimer = INTERVAL_TX;                   // Resets counter var. to defined transmission interval [sec.]  
  }
}

/* Compiles the data record to be saved and transfers the compounded data record to the system to be recorded */
RecordData()
{
  new aRecData{SIZE_RECDATA}                  // Temporary memory in which the data record to be saved is compiled
  
  /* Compiles the data record to be saved in the "aRecData" temporary memory
     - The first byte (position 0 in the "aRecData" array) is set to 0 so that the server copies
       the data record into measurement data channel 0 upon receipt, as specified during the design
         of the connector.
     - Temperature is copied to position 1-4. Data type: f32                                            */  
  aRecData{0} = 0;                            // "Split-tag" 
  rM2M_Pack(aRecData, 1, fTemperature,  RM2M_PACK_F32 + RM2M_PACK_BE);
  
  rM2M_RecData(0, aRecData, SIZE_RECDATA);    // Transfers compounded data record to the system to be recorded
}


/**
 * Calculates the absolute value 
 *
 * @param fValue:f32 - Value for which the absolute value should be determined
 * @return f32       - Absolute value of the transferred value 
 * 
 */
Float:Abs(Float:fValue)
{
 return ((fValue)<0 ? (-fValue) : (fValue))
}

/**
 * Checks whether an alarm has to be triggered or released
 *
 * If the temperature exceeds the threshold, an alarm is triggered. Once an alarm has been triggered the function 
 * checks whether the temperature falls below the threshold again and releases the alarm in that case.
 * 
 * Note: It is recommended to use a hysteresis. This means that the alarm should only be released again when the measured 
 *       value falls 5% below the alarm threshold (see Example "61_alarm_2.p") 
 * 
 */
CheckAlarm()
{
  static iIsAlarmActive;                      // Current alarm state(0 = no alarm, 1 = alarm active)
  
  // If an alarm is currently active and the temperature falls to or below the alarm threshold - 5% (hysteresis) ->
  if((iIsAlarmActive == 1) && ( fTemperature <= ( fAlarmThreshold - Abs( (fAlarmThreshold * fHysteresis/100) ) )   ))
  { 
    // Clears alarm and starts transmission
    printf("clear Alarm\r\n");
    Al_SetAlarm(0, 0, 0, fTemperature, fAlarmThreshold); //Clear alarm
    rM2M_TxStart();                           // Initiates a connection to the server  
    iIsAlarmActive = 0;                       // Sets current alarm state to "no alarm"
  }
  // Otherwise -> If no alarm is currently active and the temperature is greater than or equal to the alarm threshold ->
  else if((iIsAlarmActive == 0) && (fTemperature >= fAlarmThreshold))
  { 
    // Sets alarm and starts transmission
    printf("set Alarm\r\n");
    Al_SetAlarm(0, 0, AL_FLG_ALARM, fTemperature, fAlarmThreshold); // Sets alarm
    rM2M_TxStart();                           // Initiates a connection to the server  
    iIsAlarmActive = 1;                       // Sets current alarm state to "alarm active"  
  }
}

/**
 * Simulates a sensor
 *
 * This function generates a temperature value between 19°C and 31°C.
 * As long as the temperature is less than or equal to 30°C, the temperature value is increased by 1°C each time 
 * the function is called. Then it is reduced by 1°C until it is less than 20°C. 
 * 
 */
UpdateTemperature()
{
  static Float:fDelta = 1.0;                  // Value added or subtracted to the temperature when the function is called
    
  if(fTemperature > 30.0)                     // If the upper limit(31°C) is reached ->
    fDelta = -1.0;                            // Sets the value which is added or subtracted to the temperature to "-1"
    
  if(fTemperature < 20.0)                     // If the lower limit(19°C) is reached ->
    fDelta = 1.0;                             // Sets the value which is added or subtracted to the temperature to "+1"
    
  fTemperature = fTemperature + fDelta;       // Calculates the new temperature 
  
  printf("Temperature=%g\r\n", fTemperature);
}

/**
 * Generates an alarm data record and hands it over to the system for recording
 *
 * @param iHistData:s32  - HistData-stream of the server which is used for the measurement data
 * @param iChannel:s32   - Index of the measurement data channel that creates the alarm
 * @param iFlags:s32     - Alarm flags (see  AL_FLG_xxx)
 * @param fValue:f32     - Current value that has violated the alarm threshold 
 * @param fThreshold:f32 - Alarm threshold which has been violated
 */ 
Al_SetAlarm(iHistData, iChannel, iFlags, Float:fValue, Float:fThreshold)
{
  new aData{AL_SIZE};                         // Temporary memory in which the alarm record to be saved is compiled
  
  aData{0} = AL_ALARM;                                              // Split-Tag
  aData{1} = iHistData;                                             // Datastream
  rM2M_Pack(aData,  2, iChannel,    RM2M_PACK_BE + RM2M_PACK_U16);  // Channel Index
  aData{4} = iFlags;                                                // Flags
  rM2M_Pack(aData,  5, fValue,      RM2M_PACK_BE + RM2M_PACK_F32);  // Value
  rM2M_Pack(aData,  9, fThreshold,  RM2M_PACK_BE + RM2M_PACK_F32);  // Threshold
  
  rM2M_RecData(0, aData, AL_SIZE);            //Transfers the compounded alarm record to the system for recording
}
