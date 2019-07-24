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
 * @version 20190508
 */

#define TLIS3DSH_Handle[.cs, .spi, .type]
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

/* LIS3DSH commands (according to datasheet) */
const
{
  LIS3DSH_REG_OUT_T = 0x0C,
  LIS3DSH_REG_INFO1 = 0x0D,
  LIS3DSH_REG_INFO2 = 0x0E,
  LIS3DSH_REG_WHO_AM_I = 0x0F,
  LIS3DSH_REG_OFF_X = 0x10,
  LIS3DSH_REG_OFF_Y = 0x11,
  LIS3DSH_REG_OFF_Z = 0x12,
  LIS3DSH_REG_CS_X = 0x13,
  LIS3DSH_REG_CS_Y = 0x14,
  LIS3DSH_REG_CS_Z = 0x15,
  LIS3DSH_REG_LC_L = 0x16,
  LIS3DSH_REG_LC_H = 0x17,
  LIS3DSH_REG_STAT = 0x18,
  LIS3DSH_REG_PEAK1 = 0x19,
  LIS3DSH_REG_PEAK2 = 0x1A,
  LIS3DSH_REG_VFC_1 = 0x1B,
  LIS3DSH_REG_VFC_2 = 0x1C,
  LIS3DSH_REG_VFC_3 = 0x1D,
  LIS3DSH_REG_VFC_4 = 0x1E,
  LIS3DSH_REG_THRS3 = 0x1F,
  LIS3DSH_REG_CTRL4 = 0x20,
  LIS3DSH_REG_CTRL1 = 0x21,
  LIS3DSH_REG_CTRL2 = 0x22,
  LIS3DSH_REG_CTRL3 = 0x23,
  LIS3DSH_REG_CTRL5 = 0x24,
  LIS3DSH_REG_CTRL6 = 0x25,
  LIS3DSH_REG_STATUS = 0x27,
  LIS3DSH_REG_OUT_X_L = 0x28,
  LIS3DSH_REG_OUT_X_H = 0x29,
  LIS3DSH_REG_OUT_Y_L = 0x2A,
  LIS3DSH_REG_OUT_Y_H = 0x2B,
  LIS3DSH_REG_OUT_Z_L = 0x2C,
  LIS3DSH_REG_OUT_Z_H = 0x2D,
  LIS3DSH_REG_FIFO_CTRL = 0x2E,
  LIS3DSH_REG_FIFO_SRC = 0x2F,
  LIS3DSH_REG_ST1 = 0x40,
  LIS3DSH_REG_TIM4_1 = 0x50,
  LIS3DSH_REG_TIM3_1 = 0x51,
  LIS3DSH_REG_TIM2_1_L = 0x52,
  LIS3DSH_REG_TIM2_1_H = 0x53,
  LIS3DSH_REG_TIM1_1_L = 0x54,
  LIS3DSH_REG_TIM1_1_H = 0x55,
  LIS3DSH_REG_THRS2_1 = 0x56,
  LIS3DSH_REG_THRS1_1 = 0x57,
  LIS3DSH_REG_MASK1_B = 0x59,
  LIS3DSH_REG_MASK1_A = 0x5A,
  LIS3DSH_REG_SETT1 = 0x5B,
  LIS3DSH_REG_PR1 = 0x5C,
  LIS3DSH_REG_TC1_L = 0x5D,
  LIS3DSH_REG_TC1_H = 0x5E,
  LIS3DSH_REG_OUTS1 = 0x5F,
  LIS3DSH_REG_ST2 = 0x60,
  LIS3DSH_REG_TIM4_2 = 0x70,
  LIS3DSH_REG_TIM3_2 = 0x71,
  LIS3DSH_REG_TIM2_2_L = 0x72,
  LIS3DSH_REG_TIM2_2_H = 0x73,
  LIS3DSH_REG_TIM1_2_L = 0x74,
  LIS3DSH_REG_TIM1_2_H = 0x75,
  LIS3DSH_REG_THRS2_2 = 0x76,
  LIS3DSH_REG_THRS1_2 = 0x77,
  LIS3DSH_REG_DES2 = 0x78,
  LIS3DSH_REG_MASK2_B = 0x79,
  LIS3DSH_REG_MASK2_A = 0x7A,
  LIS3DSH_REG_SETT2 = 0x7B,
  LIS3DSH_REG_PR2 = 0x7C,
  LIS3DSH_REG_TC2_L = 0x7D,
  LIS3DSH_REG_TC2_H = 0x7E,
  LIS3DSH_REG_OUTS2 = 0x7F,
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
{
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

/* Initialises LIS3DSH */
stock LIS3DSH_Init(handle[TLIS3DSH_Handle], cs, spi)
{
  new aData{3};
  
  handle.cs = cs;
  handle.spi = spi;
  
  /* Inits CS for LIS3DSH */
  rM2M_GpioDir(handle.cs, RM2M_GPIO_OUTPUT);
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);

  aData{0} = LIS3DSH_Read(handle, LIS3DSH_REG_WHO_AM_I);

  /* Checks ID register */
  if(aData{0} == 0x3F)
  {
    aData{1} = LIS3DSH_Read(handle, LIS3DSH_REG_INFO1);
    aData{2} = LIS3DSH_Read(handle, LIS3DSH_REG_INFO2);
    
    /* LIS3DSH */
    handle.type = LIS_TYPE_LIS3DSH;

    /* Checks INFO1 & INFO2 register values */
    if((aData{1} != 0x21) ||
       (aData{2} != 0x00))
    {
      /* spi communication error */
      return(ERROR);
    }

    /* Enables auto address increment */
    LIS3DSH_Write(handle, LIS3DSH_REG_CTRL6, 0x10);

    /* Disables both state machines */
    LIS3DSH_Write(handle, LIS3DSH_REG_CTRL1, 0x00);
    LIS3DSH_Write(handle, LIS3DSH_REG_CTRL2, 0x00);
  }
  else if(aData{0} == 0x33)
  {
    /* LIS3DH */
    handle.type = LIS_TYPE_LIS3DH;
  }
  else
  {
    /* Not LIS3D(S)H or spi communication error */
    handle.type = LIS_TYPE_ERROR;
    return(ERROR);
  }

  return(OK);
}

/* Reads buffer (flexible length) of LIS3DSH */
stock LIS3DSH_ReadBuf(handle[TLIS3DSH_Handle], Address, Data{}, Len)
{
  Data{0} = LIS3DSH_COM_READ|Address;

  /* Sets CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_LOW);

  /* Sets address and reads data */
  rM2M_SpiCom(handle.spi, Data, 1, Len);

  /* Clears CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);

  /* Returns register value */
  return(Data{0});
}

/* Reads buffer (1 Byte) of LIS3DSH */
stock LIS3DSH_Read(handle[TLIS3DSH_Handle], Address)
{
  new Data{1};

  LIS3DSH_ReadBuf(handle, Address, Data, 1);

  //printf("@%02X = %02X\r\n", Address, Data{0});
  
  /* Returns single register value */
  return(Data{0});
}

/* Writes buffer (flexible length) of LIS3DSH */
stock LIS3DSH_WriteBuf(handle[TLIS3DSH_Handle], Address, Data{}, Len)
{
  new Adr{1};

  Adr{0} = Address & ~LIS3DSH_COM_READ;

  /* Sets CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_LOW);

  /* Sets address */
  rM2M_SpiCom(handle.spi, Adr, 1, 0);

  /* Writes data */
  rM2M_SpiCom(handle.spi, Data, Len, 0);

  /* Clears CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);
}

/* Writes Buffer (1 Byte) of LIS3DSH */
stock LIS3DSH_Write(handle[TLIS3DSH_Handle], Address, Value)
{
  new Data{1};

  Data{0} = Value;

  LIS3DSH_WriteBuf(handle, Address, Data, 1);

  //printf("%02X -> @%02X\r\n", Value, Address);
}

/* Enables state machine */
stock LIS3DSH_LoadStateMachine(handle[TLIS3DSH_Handle], sm, code{16}, const init_regs[][TLIS3DSH_InitRegs])
{
  new i, sm_code_reg, sm_regs;

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
  LIS3DSH_Write(handle, sm, 0x00);

  /* Loads state machine code */
  LIS3DSH_WriteBuf(handle, sm_code_reg, code, 16);

  /* Inits additional state machine registers */
  i = 0;
  while(init_regs[i].reg_ofs > 0)
  {
    LIS3DSH_Write(handle, sm_regs+init_regs[i].reg_ofs, init_regs[i].value);
    i++;
  }

  /* Enables state machine */
  LIS3DSH_Write(handle, sm, 0x01);

  return(OK);
}

/* Disables state machine */
stock LIS3DSH_DisableStateMachine(handle[TLIS3DSH_Handle], sm)
{
  if(sm == 1)
    sm = LIS3DSH_REG_CTRL1;
  else if(sm == 2)
    sm = LIS3DSH_REG_CTRL2;
  else
    return(ERROR);

  /* Disables state machine */
  LIS3DSH_Write(handle, sm, 0x00);

  return(OK);
}

/* Writes register */
stock LIS3DSH_WriteReg(handle[TLIS3DSH_Handle], addr, data)
{
  new aData{2};

  aData{0} = addr & ~LIS3DSH_COM_READ;
  aData{1} = data;

  /* Sets CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_LOW);

  /* Writes register value */
  rM2M_SpiCom(handle.spi, aData, 2, 0);

  /* Clears CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);
}

/* Reads register */
stock LIS3DSH_ReadReg(handle[TLIS3DSH_Handle], addr, &data)
{
  new aData{2};

  /* Sets CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_LOW);

  /* Writes register address */
  aData{0} = addr | LIS3DSH_COM_READ;
  rM2M_SpiCom(handle.spi, aData, 1, 0);

  /* Reads register value */
  rM2M_SpiCom(handle.spi, aData, 0, 1);
  data = aData{0};

  /* Clears CS */
  rM2M_GpioSet(handle.cs, RM2M_GPIO_HIGH);
}

stock LIS3DSH_ReadMeasurementRaw(handle[TLIS3DSH_Handle], &x, &y, &z)
{
  new aRawX{2}, aRawY{2}, aRawZ{2};         // 16-bit values from accelerometer
  
  LIS3DSH_ReadBuf(handle, LIS3DSH_REG_OUT_X_L, aRawX, 2);
  LIS3DSH_ReadBuf(handle, LIS3DSH_REG_OUT_Y_L, aRawY, 2);
  LIS3DSH_ReadBuf(handle, LIS3DSH_REG_OUT_Z_L, aRawZ, 2);

  
  rM2M_Pack(aRawX, 0, x, RM2M_PACK_S16 + RM2M_PACK_GET);
  rM2M_Pack(aRawY, 0, y, RM2M_PACK_S16 + RM2M_PACK_GET);
  rM2M_Pack(aRawZ, 0, z, RM2M_PACK_S16 + RM2M_PACK_GET);
  
}

// using +/- 2g sensitivity get data in mg (1/1000 g)
stock LIS3DSH_ReadMeasurement(handle[TLIS3DSH_Handle], &x, &y, &z)
{
  LIS3DSH_ReadMeasurementRaw(handle, x, y, z);
  //raw -> mg
  // x=float(x) * 0.061;
  // y=float(y) * 0.061;
  // z=float(z) * 0.061;
  
  // optimize -> integer division
  // 1/0.061 = 16.393 -> 16393
  x = x * 1000 / 16393;
  y = y * 1000 / 16393;
  z = z * 1000 / 16393;
  
}
