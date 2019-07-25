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
 * Alarm interface functions
 *
 * Provides generic functions and constants for alarm implementation.
 * 
 * Supported by all rapidM2M devices.
 * 
 * @version 20190619
 *
 * revision history:
 * 20150901 
 * - first version
 */

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
  AL_ALARM               = 0x20,           // "Split-tag" for the alarm data record (has to be 0x20)
  AL_SIZE                = 13,             // Size of the alarm data record (has to be 13 bytes)
}

/**
 * Generates an alarm data record and hands it over to the system for recording
 * 
 * Use this function to generate an alarm. Note that the alarm record must always have the
 * same format as shown below because of the data decoding algorithm on the server side.
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
  
  rM2M_RecData(0, aData, AL_SIZE);            // Transfers the compounded alarm record to the system for recording
}
