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
 * Extended "File transfer" example
 * 
 * Simulates receiving a big file (Uart.txt) from e.g. UART and sends it to the server.
 * A new file will be registered at the start of the script. File size 6400 bytes. When the 
 * device connects to the server, the server requests the file. The server does not request 
 * the entire file at once but split into 4kB blocks. If the device receives the request 
 * for a 4kB block from the server, the reception of a 64 byte data block via UART is 
 * simulated every 100ms until the data for an entire 4kB block is available. The requested  
 * block is then transferred to the server.  
 *  
 *  +-----------+                   +--------+                     +--------+
 *  | e.g. UART | --> (64 byte) --> | Device | --> (4096 byte) --> | Server |
 *  +-----------+                   +--------+                     +--------+
 *  
 * Compatible with every rapidM2M hardware.
 *  
 */

#pragma amxram  65536                                    // Sets the size of the RAM available to the script (max. 64kB)
#pragma dynamic  5120                                    // 5120 cells minimum stack/heap size
#include <string>

/* Forward declarations of public functions */
forward public Timer100ms();                             // Called up every 100ms to simulate the reception of a 64 byte 
                                                         // block via UART after the server has requested the file 
forward public FileCmd(id, cmd, const data{}, len, ofs); // Called up when a file transfer command is received 

const
{
  BUFFER_UART_SIZE     = 64,                             // Size of the UART reception buffer
  BUFFER_TRANSFER_SIZE = 4096,                           // Size of the buffer for a block of the file
}

// States of the file transfer
const
{
  STATE_NONE = 0,										 // Idle
  STATE_GETINFO,                                         // Server is waiting for properties of a file
  STATE_GETDATA,                                         // Server is waiting for part of a file
}

static srUartFile[TFT_Info];                             // Contains the properties of the file entry
static aBufferFile{BUFFER_TRANSFER_SIZE};                // Buffer containing a block of the file
static iBufferFileLength;                                // Current number of bytes in the buffer for the file

static iPrevOffset;                                      // Previously received byte offset within the file
static iPrevLength;                                      // Previously received number of bytes requested by the server
static iPrevId;                                          // Previously received Unique Identification

static cszFileCmd[]{} = [                                // Array containing a description string for each possible file transfer command 
  "FT_CMD_NONE",
  "FT_CMD_UNLOCK",
  "FT_CMD_LIST",
  "FT_CMD_READ",
  "FT_CMD_STORE",
  "FT_CMD_WRITE",
  "FT_CMD_DELETE"
];

static iState = STATE_NONE;                              // Current state of the file transfer

// Used to generate a sample file that e.g. is sent via UART
// The size of the sample file is 6400 bytes (64 Characters per line, 100 lines)
static aFilePart{} = "Das ist eine Zeile einer Testdatei ... 1234567890 - 1234567890\r\n"; // Array containing a line of the sample file 
static iFileLineCount = 100;                                                               // Number of lines of the sample file 

/* Application entry point */
main()
{
  /* Temporary memory for the index of a public function and the return value of a function  */
  new iIdx, iResult;

  // Initialises the file entry (set filename to "Uart.txt", empty file)
  srUartFile.name     = "Uart.txt";
  srUartFile.size     = 0;
  srUartFile.stamp    = 0;
  srUartFile.stamp256 = 0;  
  srUartFile.crc      = 0x00;
  srUartFile.flags    = FT_FLAG_READ;
  
  /* Registers the file so that it can be accessed by the server
     - Determining the function index that should be called up when a file transfer command is received
     - Transferring the index to the system, setting the filename and setting the Unique Identification 
	   with which the file is then referenced                                                           
     - Issues the return value of the "FT_Register()" function via the console                        */
  iIdx = funcidx("FileCmd");
  iResult = FT_Register(srUartFile.name, 0, iIdx);
  printf("FT_Register(%d) = %d\r\n", 0, iResult);
  
    /* init 100ms Timer, used for uart tx */
  iIdx = funcidx("Timer100ms");
  iResult = rM2M_TimerAddExt(iIdx, true, 100);
  printf("rM2M_TimerAddExt(%d) = %d\r\n", iIdx, iResult);
  
  rM2M_TxSetMode(RM2M_TXMODE_WAKEUP);                    // Sets the connection type to "Wakeup" mode 
}

/**
 * Callback function that is called up when a file transfer command is received 
 *
 * @param id:s32    - Unique Identification with which the file is referenced (specified during registration) 
 * @param cmd:s32   - File transfer command that was received from the system 
 * @param data[]:u8 - Only relevant for following file transfer commands:
 *                     FT_CMD_STORE: Array that contains the properties of the file that should be newly created. Structure:
 *                        Offset    Bytes   Explanation
                          0         4       Time stamp of the file
                          8         4       File size in bytes
                         12         4       Ethernet CRC32 of the file
                         16         2       File flags
                         18       256       File name
 *                    FT_CMD_WRITE: Array that contains the data received from the server
 * @param len:s32   - Only relevant for following file transfer commands:
 *                     FT_CMD_READ: Number of bytes requested by the server
 *                     FT_CMD_STORE: Size of the file property block received from the server
 *                     FT_CMD_WRITE: Number of bytes received from the server
 * @param ofs:s32   - Only relevant for following file transfer commands:
 *                     FT_CMD_READ: Byte offset within the file of the data block to be transferred to the server
 *                     FT_CMD_WRITE: Byte offset within the file of the data block received from the server
 */
public FileCmd(id, cmd, const data{}, len, ofs)
{
  // Issues received Unique Identification, file transfer command (description string and value), length and offset  
  printf("FileCmd(%d, %s (%d), data{}, %d, %d)\r\n", id, cszFileCmd[cmd], cmd, len, ofs);

  switch(cmd)                                            // Switches the received file transfer command -> 
  {
    case FT_CMD_LIST:                                    // The server requests the properties of a file
    {
      iState = STATE_GETINFO;                            // Sets the state of the file transfer to "Server is waiting for properties of a file" 
      
      // The server expects FT_SetPropsExt() within 10 seconds
    }
    case FT_CMD_READ:                                    // The server requests part of a file.
    {
      iPrevLength = len;                                 // Copies currently requested number of bytes to the variable for the previously received requested number of bytes
      iPrevOffset = ofs;                                 // Copies current byte offset to the variable for the previously received byte offset 
      iState = STATE_GETDATA;                            // Sets the state of the file transfer to "Server is waiting for a part of a file" 
	  
      // The server expects FT_Read() within 10 seconds
    }
    case FT_CMD_UNLOCK:                                  // File transfer session terminated. The server releases the block again.
    {
      iState = STATE_NONE;                               // Sets the state of the file transfer to "idle" 
      
      // Finished reading file
    }
    default:                                             // Received file transfer command is not handled by this script 
    {
      FT_Error(id);                                      // Displays a file handling error and terminates any file command
    }
  }
  
  iPrevId = id;                                          // Copies current Unique Identification to the variable for the previously received Unique Identification
}

/* 100ms timer used to simulate the reception of a 64 byte block via UART after the server has requested the file */
public Timer100ms()
{
  new iResult;                                           // Temporary memory for the return value of a function
  
  switch(iState)                                         // Switches the state of the file transfer 
  {
    case STATE_GETINFO:                                  // Server is waiting for properties of a file
    {
      iBufferFileLength = 0;                             // Resets file buffer
      
      srUartFile.stamp = Uart_GetStamp();                // Updates the timestamp in properties of the file entry
      srUartFile.crc   = Uart_GetCrc();                  // Updates the CRC in properties of the file entry
      srUartFile.size  = Uart_GetSize();                 // Updates the file size in properties of the file entry
      
	  // Issues the timestamp, file size, CRC and file flags via the console
      printf("Props %08X %d %08X %04X\r\n", srUartFile.stamp, srUartFile.size, srUartFile.crc, srUartFile.flags);
      FT_SetPropsExt(iPrevId, srUartFile);               // Sets the properties of the file 
      
      iState = STATE_NONE;                               // Sets the state of the file transfer to "idle" 
    }
    case STATE_GETDATA:                                  // Server is waiting for part of a file
    {
      new aData{BUFFER_UART_SIZE};                       // Temporary memory for the UART reception buffer
      new iLength;                                       // Number of bytes to be read from the simulated UART 
      
      iLength = iPrevLength - iBufferFileLength;         // Calculates the difference between the number of bytes requested by the server and the number of bytes in the buffer for the file
      if(iLength > BUFFER_UART_SIZE)                     // If the difference is greater than the size of the UART reception buffer ->   
        iLength = BUFFER_UART_SIZE;                      //    Limits the number of bytes to be read from the simulated UART to the size of the UART reception buffer  
      
      // Gets a data block from the simulated UART
	  Uart_GetDataBlock(iPrevOffset + iBufferFileLength, iLength, aData);
      
	  // Issues current number of bytes in the buffer for the file and number of bytes requested by the server via the console
      printf("%d/%d\r\n", iBufferFileLength, iPrevLength);
      
	  // If the number of bytes in the buffer for the file + number of bytes to be read from the simulated UART is greater than the number of bytes requested by the server
      if((iBufferFileLength + iLength) > iPrevLength)    
      { 
	    // Buffer not big enough - fill remaining buffer
        printf("ERROR Datalength from UART\r\n");
      }
      else
      { 
        // Enough space for writing block to buffer for the file 
        rM2M_SetPackedB(aBufferFile, iBufferFileLength, aData, iLength);// Copies the data read from the simulated UART to the buffer for the file   
        iBufferFileLength += iLength;                    // Adds number of bytes to be read from the simulated UART to the current number of bytes in the buffer for the file
      }
      
      // If the current number of bytes in the buffer for the file matches the bytes requested by the server (i.e. requested part of the file is complete)
      if(iBufferFileLength == iPrevLength) 
      {
        printf("FT_Read()\r\n");
        
		// Hands over the data to the system to transfer them to the server and issues in case of a problem the return value of the FT_Read via the console
		iResult = FT_Read(iPrevId, aBufferFile, iPrevLength);
        if(iResult < OK)
          printf("FT_Read()=%d\r\n", iResult);
        
        iBufferFileLength = 0;                           // Resets file buffer
        iState = STATE_NONE;                             // Sets the state of the file transfer to "idle" 
      }
    }
  }
}

/**
 * Calculates the CRC32 for the generated sample file
 *
 * @return s32 - CRC32 of the generated sample file
 */
Uart_GetCrc()
{
  new iCrc = 0;                                          // Temporary memory for the calculated CRC32
  new iFilePartSize;                                     // Temporary memory for the size (in byte) of a line 
  
  iFilePartSize = strlen(aFilePart);                     // Size of a line of the generated sample file
  
  for(new iIdx = 0; iIdx < iFileLineCount; iIdx++)       // For the number of lines of the generated sample file ->  
  {
    iCrc = CRC32(aFilePart, iFilePartSize, iCrc);        // Calculates the moving CRC32  
  }
  
  return iCrc;
}

/**
 * Calculates size of the generated sample file
 *
 * @return s32 - Size of the generated sample file (in byte)
 */
Uart_GetSize()
{
  new iFilePartSize;                                     // Temporary memory for the size (in byte) of a line 
  
  iFilePartSize = strlen(aFilePart);                     // Size of a line of the generated sample file
  
  return iFilePartSize * iFileLineCount;                 // Return (size of a line) * (number of lines)
}

/**
 * Returns the current system time in UTC
 *
 * @return s32 - current system time in UTC
 */
Uart_GetStamp()
{
  return rM2M_GetTime();
}

/**
 * Returns a data block from the simulated UART
 *
 * @param iOffset:s32 - Byte offset within the generated sample file
 * @param iLength:s32 - Number of bytes that should be read from the simulated UART
 * @param aData[]:u8  - Array to store the data 
 */
Uart_GetDataBlock(iOffset, iLength, aData{BUFFER_UART_SIZE})
{
  new iOffsetStart;                                      // Index of the byte of the array containing a line of the sample file that 
                                                         // should be copied to the array for returning the requested data    
  new iFilePartSize;                                     // Temporary memory for the size (in bytes) of a line 
  new iDataLength = 0;                                   // Temporary memory for the number of already written data
  
  iFilePartSize = strlen(aFilePart);                     // Size of a line of the generated sample file

  iOffsetStart = iOffset % iFilePartSize;                // Calculates where to start copying the array containing a line of the sample 
                                                         // to the array for returning the requested data (necessary if a complete line was 
														 // not copied the last time the function was called) 

  // As long as the desired number of bytes has not been written to the array for returning the requested data
  while(iDataLength < iLength)                          
  {
    aData{iDataLength} = aFilePart{iOffsetStart};       // Copies a byte from the array containing a line of the sample file to 
	                                                    // the array for returning the requested data   
    iDataLength++;                                      // Increases the number of already written data 
    iOffsetStart = (iOffsetStart + 1) % iFilePartSize;  // Calculates which byte of the array containing a line of the sample is the next for copying
                                                        // (necessary for starting copying again at the beginning of the line when the end of the line is reached)
  }
}