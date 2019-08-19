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
 * PMIC BQ25892 interface functions.
 */

/* Available BQ25892 I2C addresses */
#define BQ25892_I2C_ADR (0xD6)

/* Available register */
const
{
  BQ25892_REG00 = 0x00,
  BQ25892_REG01,  
  BQ25892_REG02,  
  BQ25892_REG03,  
  BQ25892_REG04,  
  BQ25892_REG05,  
  BQ25892_REG06,  
  BQ25892_REG07,  
  BQ25892_REG08,  
  BQ25892_REG09,  
  BQ25892_REG0A,  
  BQ25892_REG0B,  
  BQ25892_REG0C,  
  BQ25892_REG0D,  
  BQ25892_REG0E,  
  BQ25892_REG0F,  
  BQ25892_REG10,  
  BQ25892_REG11,  
  BQ25892_REG12,  
  BQ25892_REG13,  
  BQ25892_REG14,  
  BQ25892_N_REG
}

#define TBQ25892_Handle[.i2c, .addr, .reg{BQ25892_N_REG}]

BQ25892_Init(handle[TBQ25892_Handle], i2c, addr)
{
  new aData{BQ25892_N_REG};
  new iResult;

  handle.i2c = i2c;
  handle.addr = addr;
  
  /* read-back all register values (get initial read image) */
  aData{0} = BQ25892_REG00;
  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 1, BQ25892_N_REG);
  handle.reg = aData;

  return(iResult);
}

stock BQ25892_RegWrite(handle[TBQ25892_Handle], reg, val, mask=0xFF)
{
  new iResult;
  new aData{2};

  if((reg < 0) || (reg >= BQ25892_N_REG))
    return(ERROR);

  /* preserve bit values not within mask */
  aData{0} = reg;
  aData{1} = handle.reg{reg};
  aData{1} &= ~mask;
  aData{1} |= (val & mask);
  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 2, 0);
  if(iResult >= OK)
    handle.reg{reg} = aData{1};

  return(iResult);
}

stock BQ25892_RegRead(handle[TBQ25892_Handle], reg, &val)
{
  new iResult;
  new aData{2};

  if((reg < 0) || (reg >= BQ25892_N_REG))
    return(ERROR);

  aData{0} = reg;
  iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 1, 1);
  if(iResult >= OK)
  {
    /* store data */
    val = aData{0};
    handle.reg{reg} = val;
  }

  return(iResult);
}
