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
 * Extended "System Values" Example
 *
 * Reads the last valid values for Vin and Vaux periodically (record interval) from the system and stores the 
 * generated data record in the flash of the system. The measurement data generated this way is then 
 * transmitted periodically (transmission interval) to the server. 
 * 
 * Note: To use the recorded data within the interface of the server (e.g. reports, visualisations, graphics, etc.)
 *       it is necessary to define a Data Descriptor (see 30_System_Values_1.txt)
 *
 * 
 * Only compatible with rapidM2M M2xx
 * 
 *
 * @version 20190508  
 */

#include ".\rapidM2M M2\mx.inc"

/* Forward declarations of public functions */
forward public Timer1s();                   // Called up 1x per sec. for the program sequence

const
{
  INTERVAL_RECORD = 60,                     // Interval of record [s]
  INTERVAL_TX     = 10 * 60,                // Interval of transmission [s]
  
  SIZE_RECDATA    = 1+4+4,                  // Measurement data block consists of: "Split-tag" (u8) + Vin (u32) + Vaux (u32)                                              
}

/* Global variables for the remaining time until certain actions are triggered */
static iRecTimer;                           // Sec. until the next recording
static iTxTimer;                            // Sec. until the next transmission 

/* Application entry point */
main()
{
  /* Temporary memory for the index of a public function and the return value of a function  */
  new iIdx, iResult;

  /* Initialisation of a 1 sec. timer used for the general program sequence
     - Determining the function index that should be executed 1x per sec.
     - Transferring the index to the system
     - In case of a problem the index and return value of the init function are issued by the console  */
  iIdx = funcidx("Timer1s");
  iResult = rM2M_TimerAdd(iIdx);
  if(iResult < OK)
    printf("rM2M_TimerAdd(%d) = %d\r\n", iIdx, iResult);
  
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
    iRecTimer = INTERVAL_RECORD;            // Resets counter variable to defined record interval  
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

/* Reads the last valid values for Vin and Vaux from the system, compiles the data record to  
   be saved and transfers the compounded data record to the system to be recorded            */
RecordData()
{
  new aSysValues[TMx_SysValue];             // Temporary memory for the internal measurement values
  new aRecData{SIZE_RECDATA};               // Temporary memory in which the data record to be saved is compiled
  
  Mx_GetSysValues(aSysValues);              // Reads the last valid values for Vin and Vaux from the system 
                                            // The interval for determining these values is 1sec. and cannot be changed.
  
  /* Compiles the data record to be saved in the "aRecData" temporary memory
     - The first byte (position 0 in the "aRecData" array) is set to 0 so that the server copies
       the data record into measurement data channel 0 upon receipt, as specified during the design
         of the connector
     - Vin is copied to position 1-4. Data type: u32
     - Vaux is copied to position 5-8. Data type: u32                                        */  
  aRecData{0} = 0;                          // "Split-tag" 
  rM2M_Pack(aRecData, 1, aSysValues.VIn,  RM2M_PACK_U32|RM2M_PACK_BE);
  rM2M_Pack(aRecData, 5, aSysValues.VAux, RM2M_PACK_U32|RM2M_PACK_BE);
  
  rM2M_RecData(0, aRecData, SIZE_RECDATA);  // Transfers compounded data record to the system to be recorded
}
