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
 * Extended "I2C TMP112" Example
 * 
 * Reads temperature measurement from the TMP112 periodically (record interval) and stores the
 * generated data record in the flash of the system. The measurement data generated this way is
 * then transmitted periodically (transmission interval) to the server.
 *
 * Note: To use the recorded data within the interface of the server (e.g. reports, visualisations, graphics, etc.)
 *       it is necessary to define a Data Descriptor (see 31_i2c_1_tmp112.txt)
 * 
 * Only compatible with rapidM2M M3 and rapidM2M M2xx
 * Special hardware circuit necessary
 * 
 * @version 20190715
 */
 
#include "tmp112.inc"

/* Handle for available sensors */
static hTemp[TTMP112_Handle];

/* Forward declarations of public functions */
forward public Timer1s();

const
{
  PORT_I2C = 0,                             // I2C port used for communication with temperature sensor
  
  INTERVAL_RECORD = 60,                     // Interval of record [s]
  INTERVAL_TX     = 10 * 60,                // Interval of transmission [s]
  
  SIZE_RECDATA    = 1+4,                    // Measurement data block consists of:
                                            // "Split-tag"(u8) + iTemp(s32)
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

  /* Initialisation of TMP112 sensor with address 1 (0x90). The address is determined by the ADD0 pin.
     In this case the ADD0 pin is connected to GND. The result is printed to the console. */
  if(TMP112_Init(hTemp, PORT_I2C, TMP112_I2C_ADR1) < OK)
  {
    print("TMP112 Error\r\n");
  }
  else 
    print("TMP112 OK\r\n");
  
  
  iRecTimer = INTERVAL_RECORD;              // Sets counter variable to defined record interval
  iTxTimer  = INTERVAL_TX;                  // Sets counter variable to defined transmission interval
  
  rM2M_TxStart();                           // Initiates a connection to the server
}

/* 1 sec. timer is used for the general program sequence */
public Timer1s()
{
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
  new iTemp;                                // Temporary memory for the temperature measurement 
  new aRecData{SIZE_RECDATA}                // Temporary memory in which the data record to be saved is compiled
  
  /* Reads actual temperature from TMP112 sensor */
  TMP112_GetTemp(hTemp, iTemp);
  
  /* Compiles the data record to be saved in the "aRecData" temporary memory
    - The first byte (position 0 in the "aRecData" array) is set to 0 so that the server copies
      the data record into measurement data channel 0 upon receipt, as specified during the design
      of the data descriptor.
    - iTemp is copied to position 1-4. Data type: s32 */
  aRecData{0} = 0;                          // "Split-tag" 
  rM2M_Pack(aRecData, 1, iTemp,  RM2M_PACK_S32|RM2M_PACK_BE);
  
  rM2M_RecData(0, aRecData, SIZE_RECDATA);  // Transfers compounded data record to the system to be recorded
}
