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
 * LIS3DSH interface functions
 * 
 * The LIS3DSH is an ultra-low-power high-performance three-axis linear accelerometer including
 * embedded state machines that can be used to implement autonomous applications. This file provides
 * interface functions for accessing the chip via SPI digital output interface. I2C access would also
 * be possible but is not implemented in this case.
 * 
 * Interfaces: 1x SPI
 *             1x GPIO
 *
 * @version 20190710
 * 
 * Revision history:
 * 20190710
 * - Removed LIS3DSH_WriteReg() and LIS3DSH_ReadReg()
 *   Use LIS3DSH_WriteBuf()/LIS3DSH_Write() or LIS3DSH_ReadBuf()/LIS3DSH_Read() instead.
 * - Edited LIS3DSH_ReadBuf(): Data is given back by reference, not by return value.
 * - Added return value (error code/successful) for LIS3DSH_WriteBuf() and LIS3DSH_ReadBuf(),
 *   indicating if the call of rM2M_SpiCom() was successful or not.
 */

/**
* Structure for LIS3DSH interface configuration
*
* cs:s32      - CS Port used for communication
* spi:s32     - SPI Port used for communication
* type:s32    - Type of chip (LIS3DSH or LIS3DH)
*/
#define TLIS3DSH_Handle[.cs, .spi, .type]

/**
* Structure that contains state machine register information
*
* reg_ofs:s32 - Offset of a state machine register
* value:s32   - Value of a state machine register
*/
#define TLIS3DSH_InitRegs[.reg_ofs, .value]

#define LIS3DSH_SPICLK     (1000000)
#define LIS3DSH_SPIMODE    (RM2M_SPI_CLKPHA)

/* Defines constants */
const
{
  LIS_TYPE_ERROR = 0,
  LIS_TYPE_LIS3DSH,
  LIS_TYPE_LIS3DH
}

const
{
  LIS3DSH_COM_READ    = (0x80),
  LIS3DSH_COM_ADR_MSK = (0x7F)
}

const
{
  LIS3DSH_U8    = 0x0000,
  LIS3DSH_S8    = 0x0100,
  LIS3DSH_U16   = 0x0200,
  LIS3DSH_S16   = 0x0300,
  LIS3DSH_DIRECT_REG_ONLY = 0xFF00,
}

/* LIS3DSH commands (according to datasheet) */
const                                           // R/W | Default | Description
{                                               // ----------------------------
  LIS3DSH_REG_OUT_T     = 0x0C | LIS3DSH_S8,    // R   | 0x21    | Temperature Output Register
  LIS3DSH_REG_INFO1     = 0x0D | LIS3DSH_U8,    // R   | 0x21    | Information Register 1
  LIS3DSH_REG_INFO2     = 0x0E | LIS3DSH_U8,    // R   | 0x00    | Information Register 2
  LIS3DSH_REG_WHO_AM_I  = 0x0F | LIS3DSH_U8,    // R   | 0x3F    | Device Identification Register
  LIS3DSH_REG_OFF_X     = 0x10 | LIS3DSH_S8,    // R/W | 0x00    | X-axis Offset Compensation Register
  LIS3DSH_REG_OFF_Y     = 0x11 | LIS3DSH_S8,    // R/W | 0x00    | Y-axis Offset Compensation Register
  LIS3DSH_REG_OFF_Z     = 0x12 | LIS3DSH_S8,    // R/W | 0x00    | Z-axis Offset Compensation Register
  LIS3DSH_REG_CS_X      = 0x13 | LIS3DSH_S8,    // R/W | 0x00    | X-axis Constant Shift Register
  LIS3DSH_REG_CS_Y      = 0x14 | LIS3DSH_S8,    // R/W | 0x00    | Y-axis Constant Shift Register
  LIS3DSH_REG_CS_Z      = 0x15 | LIS3DSH_S8,    // R/W | 0x00    | Z-axis Constant Shift Value Register
  LIS3DSH_REG_LC        = 0x16 | LIS3DSH_S16,   // R/W | 0x0001  | Long Counter Registers
  LIS3DSH_REG_LC_L      = 0x16,                 // R/W | 0x01    | Long Counter Low Register
  LIS3DSH_REG_LC_H      = 0x17,                 // R/W | 0x00    | Long Counter High Register
  LIS3DSH_REG_STAT      = 0x18 | LIS3DSH_U8,    // R   | 0x00    | State Machine Register
  LIS3DSH_REG_PEAK1     = 0x19 | LIS3DSH_S8,    // R   | 0x00    | Peak 1 Register
  LIS3DSH_REG_PEAK2     = 0x1A | LIS3DSH_S8,    // R   | 0x00    | Peak 2 Register
  LIS3DSH_REG_VFC_1     = 0x1B | LIS3DSH_U8,    // R/W | 0x00    | Vector Filter Coefficient 1 Register
  LIS3DSH_REG_VFC_2     = 0x1C | LIS3DSH_U8,    // R/W | 0x00    | Vector Filter Coefficient 2 Register
  LIS3DSH_REG_VFC_3     = 0x1D | LIS3DSH_U8,    // R/W | 0x00    | Vector Filter Coefficient 3 Register
  LIS3DSH_REG_VFC_4     = 0x1E | LIS3DSH_U8,    // R/W | 0x00    | Vector Filter Coefficient 4 Register
  LIS3DSH_REG_THRS3     = 0x1F | LIS3DSH_U8,    // R/W | 0x00    | Threshold Value 3 Register
  LIS3DSH_REG_CTRL4     = 0x20 | LIS3DSH_U8,    // R/W | 0x07    | Control Register 4
  LIS3DSH_REG_CTRL1     = 0x21 | LIS3DSH_U8,    // R/W | 0x00    | Control Register 1 (SM1 interrupt configuration register)
  LIS3DSH_REG_CTRL2     = 0x22 | LIS3DSH_U8,    // R/W | 0x00    | Control Register 2 (SM2 interrupt configuration register)
  LIS3DSH_REG_CTRL3     = 0x23 | LIS3DSH_U8,    // R/W | 0x00    | Control Register 3
  LIS3DSH_REG_CTRL5     = 0x24 | LIS3DSH_U8,    // R/W | 0x00    | Control Register 5
  LIS3DSH_REG_CTRL6     = 0x25 | LIS3DSH_U8,    // R/W | 0x00    | Control Register 6
  LIS3DSH_REG_STATUS    = 0x27 | LIS3DSH_U8,    // R   | 0x00    | Status Data Register
  LIS3DSH_REG_OUT_X     = 0x28 | LIS3DSH_S16,   // R   | OUTPUT  | X-axis Output Acceleration Data
  LIS3DSH_REG_OUT_X_L   = 0x28,                 // R   | OUTPUT  | X-axis Output Acceleration Low Data
  LIS3DSH_REG_OUT_X_H   = 0x29,                 // R   | OUTPUT  | X-axis Output Acceleration High Data
  LIS3DSH_REG_OUT_Y     = 0x2A | LIS3DSH_S16,   // R   | OUTPUT  | Y-axis Output Acceleration Data
  LIS3DSH_REG_OUT_Y_L   = 0x2A,                 // R   | OUTPUT  | Y-axis Output Acceleration Low Data
  LIS3DSH_REG_OUT_Y_H   = 0x2B,                 // R   | OUTPUT  | Y-axis Output Acceleration High Data
  LIS3DSH_REG_OUT_Z     = 0x2C | LIS3DSH_S16,   // R   | OUTPUT  | Z-axis Output Acceleration Data
  LIS3DSH_REG_OUT_Z_L   = 0x2C,                 // R   | OUTPUT  | Z-axis Output Acceleration Low Data
  LIS3DSH_REG_OUT_Z_H   = 0x2D,                 // R   | OUTPUT  | Z-axis Output Acceleration High Data
  LIS3DSH_REG_FIFO_CTRL = 0x2E | LIS3DSH_U8,    // R/W | 0x00    | FIFO Control Register
  LIS3DSH_REG_FIFO_SRC  = 0x2F | LIS3DSH_U8,    // R   | 0x00    | FIFO Source Register
  LIS3DSH_REG_ST1_X     = 0x40 | LIS3DSH_DIRECT_REG_ONLY, /* State Machine 1 Code Register (direct register writing only) */
  LIS3DSH_REG_ST1_1     = 0x40 | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 1
  LIS3DSH_REG_ST1_2     = 0x41 | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 2
  LIS3DSH_REG_ST1_3     = 0x42 | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 3
  LIS3DSH_REG_ST1_4     = 0x43 | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 4
  LIS3DSH_REG_ST1_5     = 0x44 | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 5
  LIS3DSH_REG_ST1_6     = 0x45 | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 6
  LIS3DSH_REG_ST1_7     = 0x46 | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 7
  LIS3DSH_REG_ST1_8     = 0x47 | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 8
  LIS3DSH_REG_ST1_9     = 0x48 | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 9
  LIS3DSH_REG_ST1_10    = 0x49 | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 10
  LIS3DSH_REG_ST1_11    = 0x4A | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 11
  LIS3DSH_REG_ST1_12    = 0x4B | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 12
  LIS3DSH_REG_ST1_13    = 0x4C | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 13
  LIS3DSH_REG_ST1_14    = 0x4D | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 14
  LIS3DSH_REG_ST1_15    = 0x4E | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 15
  LIS3DSH_REG_ST1_16    = 0x4F | LIS3DSH_U8,    // W   | 0x00    | State Machine 1 Code Register 16
  LIS3DSH_REG_TIM4_1    = 0x50 | LIS3DSH_U8,    // W   | 0x00    | SM1 General Timer 4 Register
  LIS3DSH_REG_TIM3_1    = 0x51 | LIS3DSH_U8,    // W   | 0x00    | SM1 General Timer 3 Register
  LIS3DSH_REG_TIM2_1    = 0x52 | LIS3DSH_U16,   // W   | 0x0000  | SM1 General Timer 2
  LIS3DSH_REG_TIM2_1_L  = 0x52,                 // W   | 0x00    | SM1 General Timer 2 Low Register
  LIS3DSH_REG_TIM2_1_H  = 0x53,                 // W   | 0x00    | SM1 General Timer 2 High Register
  LIS3DSH_REG_TIM1_1    = 0x54 | LIS3DSH_U16,   // W   | 0x0000  | SM1 General Timer 1
  LIS3DSH_REG_TIM1_1_L  = 0x54,                 // W   | 0x00    | SM1 General Timer 1 Low Register
  LIS3DSH_REG_TIM1_1_H  = 0x55,                 // W   | 0x00    | SM1 General Timer 1 High Register
  LIS3DSH_REG_THRS2_1   = 0x56 | LIS3DSH_S8,    // W   | 0x00    | SM1 Threshold Value 1 Register
  LIS3DSH_REG_THRS1_1   = 0x57 | LIS3DSH_S8,    // W   | 0x00    | SM1 Threshold Value 2 Register
  LIS3DSH_REG_MASK1_B   = 0x59 | LIS3DSH_U8,    // W   | 0x00    | SM1 Swap Axis and Sign Mask Register
  LIS3DSH_REG_MASK1_A   = 0x5A | LIS3DSH_U8,    // W   | 0x00    | SM1 Default Axis and Sign Mask Register
  LIS3DSH_REG_SETT1     = 0x5B | LIS3DSH_U8,    // W   | 0x00    | SM1 Detection Settings Register
  LIS3DSH_REG_PR1       = 0x5C | LIS3DSH_U8,    // R   | 0x00    | SM1 Program and Reset Pointers Register
  LIS3DSH_REG_TC1       = 0x5D | LIS3DSH_U16,   // R   | 0x0000  | SM1 General Timer Counter
  LIS3DSH_REG_TC1_L     = 0x5D,                 // R   | 0x00    | SM1 General Timer Counter Low Register
  LIS3DSH_REG_TC1_H     = 0x5E,                 // R   | 0x00    | SM1 General Timer Counter High Register
  LIS3DSH_REG_OUTS1     = 0x5F | LIS3DSH_U8,    // R   | 0x00    | SM1 Output Set Flag Register
  LIS3DSH_REG_ST2_X     = 0x60 | LIS3DSH_DIRECT_REG_ONLY, /* State Machine 2 Code Register (direct register writing only) */
  LIS3DSH_REG_ST2_1     = 0x60 | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 1
  LIS3DSH_REG_ST2_2     = 0x61 | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 2
  LIS3DSH_REG_ST2_3     = 0x62 | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 3
  LIS3DSH_REG_ST2_4     = 0x63 | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 4
  LIS3DSH_REG_ST2_5     = 0x64 | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 5
  LIS3DSH_REG_ST2_6     = 0x65 | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 6
  LIS3DSH_REG_ST2_7     = 0x66 | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 7
  LIS3DSH_REG_ST2_8     = 0x67 | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 8
  LIS3DSH_REG_ST2_9     = 0x68 | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 9
  LIS3DSH_REG_ST2_10    = 0x69 | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 10
  LIS3DSH_REG_ST2_11    = 0x6A | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 11
  LIS3DSH_REG_ST2_12    = 0x6B | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 12
  LIS3DSH_REG_ST2_13    = 0x6C | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 13
  LIS3DSH_REG_ST2_14    = 0x6D | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 14
  LIS3DSH_REG_ST2_15    = 0x6E | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 15
  LIS3DSH_REG_ST2_16    = 0x6F | LIS3DSH_U8,    // W   | 0x00    | State Machine 2 Code Register 16
  LIS3DSH_REG_TIM4_2    = 0x70 | LIS3DSH_U8,    // W   | 0x00    | SM2 General Timer 4 Register
  LIS3DSH_REG_TIM3_2    = 0x71 | LIS3DSH_U8,    // W   | 0x00    | SM2 General Timer 3 Register
  LIS3DSH_REG_TIM2_2    = 0x72 | LIS3DSH_U16,   // W   | 0x0000  | SM2 General Timer 2
  LIS3DSH_REG_TIM2_2_L  = 0x72,                 // W   | 0x00    | SM2 General Timer 2 Low Register
  LIS3DSH_REG_TIM2_2_H  = 0x73,                 // W   | 0x00    | SM2 General Timer 2 High Register
  LIS3DSH_REG_TIM1_2    = 0x74 | LIS3DSH_U16,   // W   | 0x0000  | SM2 General Timer 1
  LIS3DSH_REG_TIM1_2_L  = 0x74,                 // W   | 0x00    | SM2 General Timer 1 Low Register
  LIS3DSH_REG_TIM1_2_H  = 0x75,                 // W   | 0x00    | SM2 General Timer 1 High Register
  LIS3DSH_REG_THRS2_2   = 0x76 | LIS3DSH_S8,    // W   | 0x00    | SM2 Threshold Value 1 Register
  LIS3DSH_REG_THRS1_2   = 0x77 | LIS3DSH_S8,    // W   | 0x00    | SM2 Threshold Value 2 Register
  LIS3DSH_REG_DES2      = 0x78 | LIS3DSH_U8,    // W   | 0x00    | SM2 Decimation Counter Value Register
  LIS3DSH_REG_MASK2_B   = 0x79 | LIS3DSH_U8,    // W   | 0x00    | SM2 Axis and Sign Mask Register
  LIS3DSH_REG_MASK2_A   = 0x7A | LIS3DSH_U8,    // W   | 0x00    | SM2 Axis and Sign Mask Register
  LIS3DSH_REG_SETT2     = 0x7B | LIS3DSH_U8,    // W   | 0x00    | SM2 Detection Settings Register
  LIS3DSH_REG_PR2       = 0x7C | LIS3DSH_U8,    // R   | 0x00    | SM2 Program and Reset Pointers Register
  LIS3DSH_REG_TC2       = 0x7D | LIS3DSH_U16,   // R   | 0x0000  | SM2 General Timer Counter
  LIS3DSH_REG_TC2_L     = 0x7D,                 // R   | 0x00    | SM2 General Timer Counter Low Register
  LIS3DSH_REG_TC2_H     = 0x7E,                 // R   | 0x00    | SM2 General Timer Counter High Register
  LIS3DSH_REG_OUTS2     = 0x7F | LIS3DSH_U8,    // R   | 0x00    | SM2 Output Set Flag Register
}

const
{
  LIS3DH_I2C_ADR = 0x19,
  LIS3DH_WHO_AM_I = 0x0F,
  LIS3DH_CTRL_REG1 = 0x20,
  LIS3DH_CTRL_REG2 = 0x21,
  LIS3DH_CTRL_REG3 = 0x22,
  LIS3DH_CTRL_REG4 = 0x23,
  LIS3DH_CTRL_REG5 = 0x24,
  LIS3DH_CTRL_REG6 = 0x25,
  LIS3DH_REFERENCE = 0x26,
  LIS3DH_INT1_CFG  = 0x30,
  LIS3DH_INT1_SRC  = 0x31,
  LIS3DH_INT1_THS  = 0x32,
  LIS3DH_INT1_DURATION = 0x33,
  LIS3DH_INT2_CFG = 0x34,
  LIS3DH_INT2_SRC = 0x35,
  LIS3DH_INT2_THS = 0x36,
  LIS3DH_INT2_DURATION = 0x37
}

/* State machine register offsets */
const
{
  LIS3DSH_SM_TIM4   = 0x0,
  LIS3DSH_SM_TIM3   = 0x1,
  LIS3DSH_SM_TIM2_L = 0x2,
  LIS3DSH_SM_TIM2_H = 0x3,
  LIS3DSH_SM_TIM1_L = 0x4,
  LIS3DSH_SM_TIM1_H = 0x5,
  LIS3DSH_SM_THRS2  = 0x6,
  LIS3DSH_SM_THRS1  = 0x7,
  LIS3DSH_SM_DES    = 0x8, // only for SM2 !
  LIS3DSH_SM_MASK_B = 0x9,
  LIS3DSH_SM_MASK_A = 0xA,
  LIS3DSH_SM_SETT   = 0xB,
  LIS3DSH_SM_PR     = 0xC,
  LIS3DSH_SM_TC_L   = 0xD,
  LIS3DSH_SM_TC_H   = 0xE,
  LIS3DSH_SM_OUTS   = 0xF,
}

const
{ /* Output data rate configuration */
  LIS3DSH_ODR_POWERDOWN = 0x00,
  LIS3DSH_ODR_3HZ125    = 0x10,
  LIS3DSH_ODR_6HZ25     = 0x20,
  LIS3DSH_ODR_12HZ5     = 0x30,
  LIS3DSH_ODR_25HZ      = 0x40,
  LIS3DSH_ODR_50HZ      = 0x50,
  LIS3DSH_ODR_100HZ     = 0x60,
  LIS3DSH_ODR_400HZ     = 0x70,
  LIS3DSH_ODR_800HZ     = 0x80,
  LIS3DSH_ODR_1600HZ    = 0x90,
}

/**
 * Initialisation of SPI communication with LIS3DSH
 * 
 * This function copies the CS port and SPI interface to the given structure. The CS port is configured
 * as output and set to high so no data transfer takes place at the beginning. After that the device ID
 * is read out. If the ID register check succeeds, the two read-only information registers are read out.
 * If they are also correct (no error is returned), the ADD_INC bit in control register 6 is set so that
 * the address used to read/write data is increased at every block. Moreover, the two state machines are
 * disabled.
 * 
 * @param handle:TLIS3DSH_Handle    - Structure that contains LIS3DSH interface configuration
 * @param cs:s32                    - SPI CS port
 * @param spi:s32      	            - Number of SPI interface
 * @return s32                 	    - OK:       If successful
 *							   		- ERROR:	SPI communication error or not LIS3D(S)H
 *                                  - < ERROR:  Another error occurred. See the manual for all error codes.
 */
stock LIS3DSH_Init(handle[TLIS3DSH_Handle], cs, spi)
{
  new aDataBuf{1};
  new aData{3};
  
  /* Temporary memory for the return value of a function */
  new iResult;
  
  handle.cs = cs;
  handle.spi = spi;
  
  /* Inits CS for LIS3DSH */
  rM2M_GpioDir(handle.cs, RM2M_GPIO_OUTPUT);
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);

  iResult = LIS3DSH_ReadBuf(handle, LIS3DSH_REG_WHO_AM_I, aDataBuf, 1);
  if(iResult != OK) return iResult;
  aData{0} = aDataBuf{0};
  
  /* Checks ID register */
  if(aData{0} == 0x3F)
  {
    iResult = LIS3DSH_ReadBuf(handle, LIS3DSH_REG_INFO1, aDataBuf, 1);
    if(iResult != OK) return iResult;
    aData{1} = aDataBuf{0};
    iResult = LIS3DSH_ReadBuf(handle, LIS3DSH_REG_INFO2, aDataBuf, 1);
    if(iResult != OK) return iResult;
    aData{2} = aDataBuf{0};
    
    /* LIS3DSH */
    handle.type = LIS_TYPE_LIS3DSH;

    /* Checks INFO1 & INFO2 register values */
    if((aData{1} != 0x21) ||
       (aData{2} != 0x00))
    {
      /* SPI communication error */
      return(ERROR);
    }

    /* Enables auto address increment */
    iResult = LIS3DSH_Write(handle, LIS3DSH_REG_CTRL6, 0x10);
    if(iResult != OK) return iResult;
    
    /* Disables both state machines */
    iResult = LIS3DSH_Write(handle, LIS3DSH_REG_CTRL1, 0x00);
    if(iResult != OK) return iResult;
    iResult = LIS3DSH_Write(handle, LIS3DSH_REG_CTRL2, 0x00);
    if(iResult != OK) return iResult;
  }
  else if(aData{0} == 0x33)
  {
    /* LIS3DH */
    handle.type = LIS_TYPE_LIS3DH;
  }
  else
  {
    /* Not LIS3D(S)H or SPI communication error */
    handle.type = LIS_TYPE_ERROR;
    return(ERROR);
  }

  return(OK);
}

/**
 * Reads register of LIS3DSH and returns unpacked data
 * 
 * Use this function to read unsigned 8-Bit, signed 8-Bit, unsigned 16-Bit or signed 16-Bit registers
 * of the LIS3DSH. The read data is unpacked according to the data type in the register information
 * variable.
 * 
 * @param handle:TLIS3DSH_Handle - Structure that contains LIS3DSH interface configuration
 * @param iRegInfo:s32           - Register information contains address and data type
 * @param iData:s32              - Unpacked data read from the register
 * @return s32                   - OK:    If successful
 *                               - ERROR: Register information error
 *                               - < OK:  Another error occurred. See the manual for all error codes.
 */
stock LIS3DSH_Read(handle[TLIS3DSH_Handle], iRegInfo, &iData)
{
  new iResult;                              // Temporary memory for the return value of a function
  new iLen=1;                               // Temporary memory for the number of bytes to be read
  new iAddr;                                // Temporary memory for the register address
  new iType;                                // Temporary memory for the data type
  new aRaw{2};                              // Temporary memory for the raw data
  
  /* Gets address and data type */
  iAddr = iRegInfo & 0x000000FF;
  iType = iRegInfo & 0x0000FF00;
  
  /* Sets number of bytes to be read depending on the data type */
  switch(iType)
  {
    case LIS3DSH_S8, LIS3DSH_U8:
      iLen = 1;
    case LIS3DSH_S16, LIS3DSH_U16:
      iLen = 2;
    case LIS3DSH_DIRECT_REG_ONLY:
      return (ERROR);
    default:
      return (ERROR);
  }
  
  /* Sets R-Bit */
  aRaw{0} = LIS3DSH_COM_READ|iAddr;

  /* Sets CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_LOW);

  /* Sets address and reads data */
  iResult = rM2M_SpiCom(handle.spi, aRaw, 1, iLen);

  /* Clears CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);
  
  if(iResult < OK) return(iResult);
  
  /* Unpacks data depending on the register data type */
  switch(iType)
  {
    case LIS3DSH_S8:
      iResult = rM2M_Pack(aRaw, 0, iData, RM2M_PACK_S8 + RM2M_PACK_GET);
    case LIS3DSH_U8:
      iResult = rM2M_Pack(aRaw, 0, iData, RM2M_PACK_U8 + RM2M_PACK_GET);
    case LIS3DSH_S16:
      iResult = rM2M_Pack(aRaw, 0, iData, RM2M_PACK_S16 + RM2M_PACK_GET);
    case LIS3DSH_U16:
      iResult = rM2M_Pack(aRaw, 0, iData, RM2M_PACK_U16 + RM2M_PACK_GET);
    default:
      return (ERROR);
  }
  
  /* Returns error code if failed or OK if successful */
  return iResult;
}

/**
 * Reads raw data from a number of LIS3DSH registers
 * 
 * Use this function to read a defined number of registers from the LIS3DSH. The data is given
 * back as packed array.
 * 
 * @param handle:TLIS3DSH_Handle  - Structure that contains LIS3DSH interface configuration
 * @param iRegInfo:s32            - Register information contains address and data type
 * @param aData[]:u8              - Packed array for raw register data to be read
 * @param iLen:s32                - Number of bytes to be read
 * @return s32                    - OK:    If successful
 *                                - ERROR: Number of bytes to be read >255
 *                                - < OK:  Another error occurred. See the manual for all error codes.
 */
stock LIS3DSH_ReadBuf(handle[TLIS3DSH_Handle], iRegInfo, aData{}, iLen)
{
  new iResult;                              // Temporary memory for the return value of a function
  new iAddr;                                // Temporary memory for the register address
  
  /* Checks number of bytes to be read */
  if(iLen > 255) return(ERROR);
  
  /* Data types do not matter -> Raw register values are treated as unsigned 8-Bit */
  iAddr = iRegInfo & 0x000000FF;
  
  /* Sets R-Bit */
  aData{0} = LIS3DSH_COM_READ|iAddr;

  /* Sets CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_LOW);

  /* Sets address and reads data */
  iResult = rM2M_SpiCom(handle.spi, aData, 1, iLen);

  /* Clears CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);
  
  /* Returns error code if failed or OK if successful */
  return iResult;
}

/**
 * Writes register of LIS3DSH with packed data
 * 
 * Use this function to write unsigned 8-Bit, signed 8-Bit, unsigned 16-Bit or signed 16-Bit registers
 * of the LIS3DSH. The data to be written is packed according to the data type in the register information
 * variable and sent to the sensor.
 * 
 * @param handle:TLIS3DSH_Handle - Structure that contains LIS3DSH interface configuration
 * @param iRegInfo:s32           - Register information contains address and data type
 * @param iData:s32              - Data to be packed and written to the sensor
 * @return s32                   - OK:    If successful
 *                               - ERROR: Register information error
 *                               - < OK:  Another error occurred. See the manual for all error codes.
 */
stock LIS3DSH_Write(handle[TLIS3DSH_Handle], iRegInfo, iData)
{
  new iResult;                              // Temporary memory for the return value of a function
  new iLen;                                 // Temporary memory for the number of bytes to be written
  new iAddr;                                // Temporary memory for the register address
  new iType;                                // Temporary memory for the data type
  new aRaw{2 + 1};                          // Temporary memory for the packed data to be written

  /* Gets address and data type */
  iAddr = iRegInfo & 0x000000FF;
  iType = iRegInfo & 0x0000FF00;
  
  /* Sets number of bytes to be written depending on the data type */
  switch(iType)
  {
    case LIS3DSH_S8:
    {
      iLen = 1;
      iType = RM2M_PACK_S8;
    }
    case LIS3DSH_U8:
    {
      iLen = 1;
      iType = RM2M_PACK_U8;
    }
    case LIS3DSH_S16:
    {
      iLen = 2;
      iType = RM2M_PACK_S16;
    }
    case LIS3DSH_U16:
    {
      iLen = 2;
      iType = RM2M_PACK_U16;
    }
    case LIS3DSH_DIRECT_REG_ONLY:
      return (ERROR);
    default:
      return (ERROR);
  }
  
  /* Clears W-Bit */
  aRaw{0} = iAddr & ~LIS3DSH_COM_READ;
  
  /* Stores data into packed array according to the data type */
  iResult = rM2M_Pack(aRaw, 1, iData, iType);
  if(iResult < OK) return(iResult);
  
  /* Sets CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_LOW);
  
  /* Sets address and writes data */
  iResult = rM2M_SpiCom(handle.spi, aRaw, iLen + 1, 0);

  /* Clears CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);
  
  /* Returns error code if failed or OK if successful */
  return iResult;
}

/**
 * Writes a number of LIS3DSH registers
 * 
 * Use this function to write a defined number of LIS3DSH registers. The data needs to be given
 * to the function as a packed array.
 * 
 * @param handle:TLIS3DSH_Handle    - Structure that contains LIS3DSH interface configuration
 * @param iRegInfo:s32              - Register information contains address and data type
 * @param aData:u8		            - Packed array for raw register data to be written
 * @param iLen:s32                  - Number of bytes to be written
 * @return s32                      - OK:    If successful
 *                                  - ERROR: Number of bytes to be written >255
 *                                  - < OK:  Another error occurred. See the manual for all error codes.
 */
stock LIS3DSH_WriteBuf(handle[TLIS3DSH_Handle], iRegInfo, aData{}, iLen)
{
  new iResult;                              // Temporary memory for the return value of a function
  new iAddr;                                // Temporary memory for the register address
  new aAdr{1};                              // Temporary memory for the register address (packed array)
  
  /* Checks number of bytes to be written */
  if(iLen > 255) return(ERROR);
  
  /* Data types do not matter -> Raw register values are treated as unsigned 8-Bit */
  iAddr = iRegInfo & 0x000000FF;
  
  /* Clears W-Bit */
  aAdr{0} = iAddr & ~LIS3DSH_COM_READ;
  
  /* Sets CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_LOW);
  
  /* Sets address */
  iResult = rM2M_SpiCom(handle.spi, aAdr, 1, 0);
  
  /* Clears CS and return result if failed */
  if(iResult < OK)
  {
    rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);
    return iResult;
  }
  
  /* Sets registers */
  iResult = rM2M_SpiCom(handle.spi, aData, iLen, 0);
  
  /* Clears CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);
  
  /* Returns error code if failed or OK if successful */
  return iResult;
}

/**
 * Enables state machine
 * 
 * The function detects at first which state machine should be used (1 or 2) and initialises the
 * associated register addresses. After disabling the state machine, the 16-byte code is handed over to
 * the LIS3DSH_WriteBuf() function. Additional state machine registers are then also initialised. See the
 * LIS3DSH datasheet for further information. When the process is finished, the state machine is enabled.
 * 
 * @param handle:TLIS3DSH_Handle    - Structure that contains LIS3DSH interface configuration
 * @param sm:s32                    - Indicates which of the two state machines should be enabled
 * @param code[]:u8                 - Registers to implement 16 step op-code
 * @param init_regs[]:TLIS3DSH_InitRegs
 *                                  - Array of structures that contain additional state machine register
 *                                    information
 * @return s32                      - OK:       If successful
 *                                  - ERROR:    Wrong state machine number or SPI communication error
 *                                  - < ERROR:  Another error occurred. See the manual for all error codes.
 */
stock LIS3DSH_LoadStateMachine(handle[TLIS3DSH_Handle], sm, code{16}, const init_regs[][TLIS3DSH_InitRegs])
{
  new i, sm_code_reg, sm_regs;
  
  /* Temporary memory for the return value of a function */
  new iResult;
  
  if(sm == 1)
  {
    sm = LIS3DSH_REG_CTRL1;
    sm_code_reg = LIS3DSH_REG_ST1;
    sm_regs = 0x50;
  }
  else if(sm == 2)
  {
    sm = LIS3DSH_REG_CTRL2;
    sm_code_reg = LIS3DSH_REG_ST2;
    sm_regs = 0x70;
  }
  else
    return(ERROR);

  /* Disables state machine */
  iResult = LIS3DSH_Write(handle, sm, 0x00);
  if(iResult != OK) return iResult;
  
  /* Loads state machine code */
  iResult = LIS3DSH_WriteBuf(handle, sm_code_reg, code, 16);
  if(iResult != OK) return iResult;
  
  /* Inits additional state machine registers */
  i = 0;
  while(init_regs[i].reg_ofs > 0)
  {
    iResult = LIS3DSH_Write(handle, sm_regs+init_regs[i].reg_ofs, init_regs[i].value);
    if(iResult != OK) return iResult;
    i++;
  }

  /* Enables state machine */
  iResult = LIS3DSH_Write(handle, sm, 0x01);
  if(iResult != OK) return iResult;

  return(OK);
}

/**
 * Disables state machine
 * 
 * The function detects which state machine should be disabled and then clears the EN bit in
 * the corresponding register.
 * 
 * @param handle:TLIS3DSH_Handle	- Structure that contains LIS3DSH interface configuration
 * @param sm:s32           			- Indicates which of the two state machines should be disabled
 * @return s32                 		- OK:		If successful
 *                                  - ERROR:    Wrong state machine number or SPI communication error
 *                                  - < ERROR:  Another error occurred. See the manual for all error codes.
 */
stock LIS3DSH_DisableStateMachine(handle[TLIS3DSH_Handle], sm)
{
  /* Temporary memory for the return value of a function */
  new iResult;
  
  if(sm == 1)
    sm = LIS3DSH_REG_CTRL1;
  else if(sm == 2)
    sm = LIS3DSH_REG_CTRL2;
  else
    return(ERROR);

  /* Disables state machine */
  iResult = LIS3DSH_Write(handle, sm, 0x00);
  if(iResult != OK) return iResult;
  
  return(OK);
}

/**
 * Reads raw measurement data of LIS3DSH
 * 
 * The function reads and buffers the raw output values of X- Y- and Z-axis. Output data of one axis
 * consists of two bytes. The rM2M_Pack() function provides access to packed data and allows
 * to store the packed values in another variable. This stored data is accessed by reference.
 * 
 * @param handle:TLIS3DSH_Handle	- Structure that contains LIS3DSH interface configuration
 * @param x:s32           			- X-axis output raw value
 * @param y:s32           			- Y-axis output raw value
 * @param z:s32           			- Z-axis output raw value
 * @return s32                      - OK:   If successful
 *                                  - < OK: An error occurred. See the manual for all error codes.
 */
stock _LIS3DSH_ReadMeasurementRaw(handle[TLIS3DSH_Handle], &x, &y, &z)
{
  new aRaw{6};                              // 16-bit values from accelerometer
  
  /* Temporary memory for the return value of a function */
  new iResult;
  
  iResult = LIS3DSH_ReadBuf(handle, LIS3DSH_REG_OUT_X, aRaw, 6);
  if(iResult != OK) return iResult;
  
  rM2M_Pack(aRaw, 0, x, RM2M_PACK_S16 + RM2M_PACK_GET);
  rM2M_Pack(aRaw, 2, y, RM2M_PACK_S16 + RM2M_PACK_GET);
  rM2M_Pack(aRaw, 4, z, RM2M_PACK_S16 + RM2M_PACK_GET);
  
  return OK;
}

/**
 * Reads measurement data of LIS3DSH in mg
 * 
 * The function reads LIS3DSH raw value and converts it into mg using integer division. Integer devision
 * is used for optimisation purposes.
 * 
 * @param handle:TLIS3DSH_Handle    - Structure that contains LIS3DSH interface configuration
 * @param x:s32                     - X-axis output value in mg
 * @param y:s32                     - Y-axis output value in mg
 * @param z:s32                     - Z-axis output value in mg
 * @return s32                      - OK:   If successful
 *                                  - < OK: An error occurred. See the manual for all error codes.
 */
stock LIS3DSH_ReadMeasurement(handle[TLIS3DSH_Handle], &x, &y, &z)
{
  // Using +/- 2g sensitivity get data in mg (1/1000 g)
  
  /* Temporary memory for the return value of a function */
  new iResult;
  
  iResult = _LIS3DSH_ReadMeasurementRaw(handle, x, y, z);
  if(iResult != OK) return iResult;
  
  // raw -> mg
  // x=float(x) * 0.061;
  // y=float(y) * 0.061;
  // z=float(z) * 0.061;
  
  // optimise -> integer division
  // 1/0.061 = 16.393 -> 16393
  x = x * 1000 / 16393;
  y = y * 1000 / 16393;
  z = z * 1000 / 16393;
  
  return OK;
}
