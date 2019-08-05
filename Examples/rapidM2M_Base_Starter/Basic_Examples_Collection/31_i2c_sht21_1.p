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
 * Extended "I2C SHT21" Example
 * 
 * Reads temperature measurement from the SHT21 periodically (record interval) and stores the
 * generated data record in the flash of the system. The measurement data generated this way is
 * then transmitted periodically (transmission interval) to the server.
 *
 * Note: To use the recorded data within the interface of the server (e.g. reports, visualisations, graphics, etc.)
 *       it is necessary to define a Data Descriptor (see 31_i2c_1_sht21.txt)
 * 
 * Only compatible with rapidM2M M3 and rapidM2M M2xx
 * Special hardware circuit necessary
 * 
 * @version 20190717
 */

#include "sht21.inc"

/* Handle for available sensors */
static hTempRh[TSHT21_Handle];

/* Forward declarations of public functions */
forward public Timer1s();

const
{
  PORT_I2C = 0,                             // I2C port used for communication with SHT21
  
  INTERVAL_RECORD = 60,                     // Interval of record [s]
  INTERVAL_TX     = 10 * 60,                // Interval of transmission [s]
  
  SIZE_RECDATA = 1 + 2 + 2,                 // Measurement data block consists of:
                                            // "Split-tag"(u8) + iTemperature(s16) + iHumidity(s16)
}

/* Global variables for the remaining time until certain actions are triggered */
static iRecTimer;                           // Sec. until the next recording  
static iTxTimer;                            // Sec. until the next transmission

/* Application entry point */
main()
{
 /* Temporary memory for the index of a public function and the return value of a function */
  new iIdx, iResult;
  
  /* Initialisation of a 1 sec. timer used for the general program sequence
     - Determining the function index that should be executed 1x per sec.
     - Transferring the index to the system
     - Index and function return value used to generate the timer are issued via the console */
  iIdx = funcidx("Timer1s");
  iResult = rM2M_TimerAdd(iIdx);
  printf("rM2M_TimerAdd(%d) = %d\r\n", iIdx, iResult);
  
  /* Initialisation of I2C #0 with 400kHz clock */
  iResult = rM2M_I2cInit(PORT_I2C, 400000, 0);
  printf("rM2M_I2cInit() = %d\r\n", iResult);

  /* Initialisation of SHT21 sensor with address 1 (0x80) */
  if(SHT21_Init(hTempRh, PORT_I2C, SHT21_I2C_ADR1) < OK)
  {
    printf("SHT21 Error\r\n");
  }
  else 
  {
    printf("SHT21 OK\r\n");
  }
  
  iRecTimer = INTERVAL_RECORD;              // Sets counter variable to defined record interval
  iTxTimer  = INTERVAL_TX;                  // Sets counter variable to defined transmission interval
  
  rM2M_TxStart();                           // Initiates a connection to the server
}

/* 1 sec. timer is used for the general program sequence */
public Timer1s()
{
  /* Handles SHT21 measurement process */
  SHT21_HandleTimer(hTempRh);
  
  iRecTimer--;                              // Counter counting down the sec. to the next recording
  if(iRecTimer <= 0)                        // When the counter has expired ->  
  {
    print("Create Record\r\n");     
    RecordData();                           // Calls up the function to record the data 
    iRecTimer = INTERVAL_RECORD;            // Resets counter variable to defined record interval [sec.]
  }
  
  iTxTimer--;                               // Counter counting down the sec. to the next transmission
  if(iTxTimer <= 0)                         // When the counter has expired -> 
  {
    print("Start Transmission\r\n");
    rM2M_TxStart();                         // Initiates a connection to the server 
    iTxTimer = INTERVAL_TX;                 // Resets counter var. to defined transmission interval [sec.]  
  }
  
  // Issues the seconds until the next recording and the next transmission to the console
  printf("iRecTimer=%d iTxTimer=%d\r\n", iRecTimer, iTxTimer);
}

RecordData()
{
  new iResult;                              // Temporary memory for the return value of a function
  new iTemperature;                         // Temporary memory for the temperature
  new iHumidity;                            // Temporary memory for the humidity
  new aRecData{SIZE_RECDATA};               // Temporary memory in which the data record to be saved is compiled
  
  /* Reads measurements */
  iResult = SHT21_GetTemperature(hTempRh, iTemperature); // iTemperature with 0.01 scale
  if(iResult < OK)
  {
    iTemperature = 0x7FFF;                  // Set to NaN
    printf("ReadTemp(%d)\r\n", iResult);
  }
      
  iResult = SHT21_GetHumidity(hTempRh, iHumidity);       // iHumidity with 0.1 scale
  if(iResult < OK)
  {
    iHumidity = 0x7FFF;                     // Set to NaN
    printf("ReadRH(%d)\r\n", iResult);
  }
    
  /* Compiles the data record to be saved in the "aRecData" temporary memory
    - The first byte (position 0 in the "aRecData" array) is set to 0 so that the server copies
      the data record into measurement data channel 0 upon receipt, as specified during the design
      of the data descriptor.
    - iTemperature is copied to position 1-2. Data type: s16
    - iHumidity is copied to position 3-4.    Data type: s16 */
  aRecData{0} = 0;                          // "Split-tag"
  rM2M_Pack(aRecData, 1, iTemperature, RM2M_PACK_BE + RM2M_PACK_S16); // Scaling is done on server
  rM2M_Pack(aRecData, 3, iHumidity,    RM2M_PACK_BE + RM2M_PACK_S16); // Scaling is done on server

  rM2M_RecData(0, aRecData, SIZE_RECDATA);  // Transfers compounded data record to the system to be recorded
  
  /* Prints the temperature and humidity to the console */
  printf("T:%g rH:%g\r\n", float(iTemperature) / 100.0, float(iHumidity) / 10.0);
}
