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
 * transmitted periodically (transmission interval) to the server. The interval for recording and transmitting 
 * measurement data can be configured via the server.   
 * 
 * Note: To use the recorded data within the interface of the Server (e.g. reports, visualisations, graphics, etc.)
 *       it is necessary to define a Data Descriptor (see 30_System_Values_2.txt)
 *
 * Only compatible with rapidM2M M2xx
 * 
 *
 * @version 20190508  
 */
 
#include <rapidM2M M2\mx>
 
/* Forward declarations of public functions */
forward public Timer1s();                   // Called up 1x per sec. for the program sequence
forward public ReadConfig(iCfg);            // Called up when the config block is changed 

const
{
  INTERVAL_RECORD = 60,                     // Interval of record [s]
  INTERVAL_TX     = 10 * 60,                // Interval of transmission [s]
  
  SIZE_RECDATA    = 1+4+4,                  // Measurement data block consists of: "Split-tag" (u8) + Vin (u32) + Vaux (u32)   
  
  CFG_BASIC   = 0,                          // Config block 0 contains the basic config 
  CFG_SIZE    = 4+4,                        // Config block consists of: Record interval (u32) + transmission interval (u32)
}

/* Global variables to store the current configuration */
static iRecInterval;                        // Current record interval [sec.]
static iTxInterval;                         // Current transmission interval [sec.]

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
  
  /* Specification of the function that should be called up when a config block is changed
     - Determining the function index called up when changing one of the config blocks
     - Transferring the index to the system
     - In case of a problem the index and return value of the init function are issued by the console  */
  iIdx = funcidx("ReadConfig");
  iResult = rM2M_CfgOnChg(iIdx);
  if(iResult < OK)
    printf("rM2M_CfgOnChg(%d) = %d\r\n", iIdx, iResult);
  
  //Inits configuration
  ReadConfig(CFG_BASIC);
  
  iRecTimer = iRecInterval;                 // Sets counter variable to current record interval
  iTxTimer  = iTxInterval;                  // Sets counter variable to current transmission interval
  
  //Starts initial connection
  rM2M_TxSetMode(RM2M_TXMODE_WAKEUP);       // Sets the connection type to "Wakeup" mode
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
    iRecTimer = iRecInterval;               // Resets counter variable to current record interval  
  }
  
  iTxTimer--;                               // Counter counting down the sec. to the next transmission
  if(iTxTimer <= 0)                         // When the counter has expired ->
  {
    print("Start Transmission\r\n");
    rM2M_TxStart();                         // Initiates a connection to the server 
    iTxTimer = iTxInterval;                 // Resets counter var. to current transmission interval [sec.]
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


 /**
 * Function that should be called up when a config block is changed to make it possible to react 
 * to a changed configuration received from the server 
 *
 * @param iCfg:s32 - Number of the changed configuration memory block starting with 0 for the first memory block 
 */
public ReadConfig(iCfg)
{
  new iSize;                                // Temporary memory for the size of the basic config in bytes

  if(iCfg == CFG_BASIC)                     // If the changed configuration is the basic config ->
  {
    new aData{CFG_SIZE};                    // Temporary memory for the basic config read from the system
    new iTmp;                               // Temporary memory for a basic config parameter

    /* Reads basic config from the system and copies it to the temporary memory. The number of the config block
       and return value of the read function (number of bytes or error code) is then issued via the console */
    iSize = rM2M_CfgRead(iCfg, 0, aData, CFG_SIZE);
    printf("Cfg %d size = %d\r\n", iCfg, iSize);

    // If the number of read bytes is lower than the size of the basic config (i.e. config invalid or not available) ->
	// Info: Error codes are negative
	if(iSize < CFG_SIZE)                    
    { 
      /* The following block initially copies the default values to the temporary memory for the read
         basic config and then sets the temporary memory for the size of the basic config to the current
         size. This ensures that the following "IF" is executed during re-initialisation and when
         changes are received. If special actions have to be executed for individual parameters,
         it is thus not necessary to implement these separately for both cases.                         */
	  iTmp = INTERVAL_RECORD; 
      rM2M_Pack(aData,  0, iTmp, RM2M_PACK_U32|RM2M_PACK_BE);
	  iTmp = INTERVAL_TX;  
      rM2M_Pack(aData,  4, iTmp, RM2M_PACK_U32|RM2M_PACK_BE);
      iSize = CFG_SIZE;
      print("created new Config #0\r\n");
    }

    // If the number of read bytes at least equates to the size of the basic config (i.e. config valid) ->
    if(iSize >= CFG_SIZE)
    {

      /* Copies record interval (u32, Big Endian) at position 0-3 from the temporary memory for the read
         basic config into the temporary memory for a parameter                                         */
      rM2M_Pack(aData,  0, iTmp, RM2M_PACK_GET|RM2M_PACK_U32|RM2M_PACK_BE);
      if(iTmp != iRecInterval)
      {
        printf("iRecInterval changed to %d s\r\n", iTmp);  // Issues received value via the console
        iRecInterval = iTmp;                               // Copies received value into global variable
      }
      
	  /* Copies transmission interval (u32, Big Endian) at position 4-7 from the temporary memory for the
         read basic config into the temporary memory for a parameter                                    */	  
      rM2M_Pack(aData,  4, iTmp, RM2M_PACK_GET|RM2M_PACK_U32|RM2M_PACK_BE);
      if(iTmp != iTxInterval)
      {
        printf("iTxInterval changed to %d s\r\n", iTmp);   // Issues received value via the console
        iTxInterval = iTmp;                                // Copies received value into global variable
        
        iTxTimer = iTxInterval;                            // Sets counter var. to the new transmission interval immediately
      }
    }
  }
}
