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
 * Simple "UART" Example
 *
 * Issues the text "Data" every secound via the UART interface and receives data via the UART interface.
 * Once data have been received via the UART interface, it is issued via the console.
 *
 * Only compatible with rapidM2M M2xx and rapidM2M M3
 * 
 *
 * @version 20190508  
 */
 
/* Forward declarations of public functions */
forward public Timer1s();                   // Called up 1x per sec. to issue the text "Data" via the UART interface
forward public UartRx(const data{}, len)    // Called up when characters are received via the UART interface

const
{
  PORT_UART = 0,                            // The first UART interface should be used
}

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
  printf("rM2M_TimerAdd(%d) = %d\r\n", iIdx, iResult);
  
  /* Initialisation of the UART interface that should be used 
     - Determining the function index that should be called up when characters are received
     - Transferring the index to the system and configure the interface                      
	   (115200 Baud, 8 data bits, no parity, 1 stop bit)                                     */ 
  iIdx = funcidx("UartRx");
  iResult = rM2M_UartInit(PORT_UART, 115200, RM2M_UART_8_DATABIT|RM2M_UART_PARITY_NONE|RM2M_UART_1_STOPBIT, iIdx);
}

/* 1s Timer used to issue the text "Data" via the UART interface every second */
public Timer1s()
{
  rM2M_UartWrite(PORT_UART, "Data\r\n", 6);
}

/**
 * Callback function that is called up when characters are received via the UART interface
 *
 * @param data[]:u8 - Array that contains the received data 
 * @param len:s32   - Number of received bytes 
 */
public UartRx(const data{}, len)
{
  printf("UartRx (%d) %s\r\n", len, data);  // Issues the received characters via the console
}

