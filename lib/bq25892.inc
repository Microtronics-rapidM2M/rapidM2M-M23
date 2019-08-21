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
 * BQ25892 interface functions
 *
 * The BQ25892 is a highly integrated 5A switch mode battery charge management and system
 * power path management device for single cell Li-Ion and Li-polymer batteries.
 * 
 * Interfaces: 1x I2C
 *
 * @version 20190814
 * 
 * revision history:
 * 20190814
 * - Added JSDoc comments
 * - Refactored register read and write functions
 * - Removed register state logic, instead read the register value before writing to save the state of the register
 * - Added shipping mode function
 * - Added register reset function
 */

 /* Available I2C addresses */
#define BQ25892_I2C_ADR (0xD6)

/**
* Structure for BQ25892 interface configuration
*
* i2c:s32      - I2C port used for communication
* addr:s32     - Address of the I2C device
*/
#define TBQ25892_Handle[.i2c, .addr]


/* REG00 R/W */
const                                                     // Reset by           | Description
{                                                         // --------------------------------
  BQ25892_REG00_EN_HIZ                  = (0x80),         // REG_RST & Watchdog | Enable HIZ mode
  BQ25892_REG00_EN_ILIM                 = (0x40),         // REG_RST & Watchdog | Enable ILIM pin
  BQ25892_REG00_IINLIM_1600             = (0x20),         // REG_RST            | Input current limit, 1600mA
  BQ25892_REG00_IINLIM_800              = (0x10),         // REG_RST            | Input current limit, 800mA
  BQ25892_REG00_IINLIM_400              = (0x08),         // REG_RST            | Input current limit, 400mA
  BQ25892_REG00_IINLIM_200              = (0x04),         // REG_RST            | Input current limit, 200mA
  BQ25892_REG00_IINLIM_100              = (0x02),         // REG_RST            | Input current limit, 100mA 
  BQ25892_REG00_IINLIM_50               = (0x01),         // REG_RST            | Input current limit, 50mA
  BQ25892_REG00_IINLIM_MSK              = (0x3F),         //                    | IINLIM bit mask
                                                          // --------------------------------
  BQ25892_REG00_DEFAULT                 = (0x48),         // Default state of the register
};


/* REG01 R/W */
const                                                     // Reset by           | Description
{                                                         // ---------------------------------
  BQ25892_REG01_BHOT_TH_34              = (0x00 << 6),    // REG_RST & Watchdog | 00 - VBHOT1 threshold (34.75%) (default)
  BQ25892_REG01_BHOT_TH_37              = (0x01 << 6),    // REG_RST & Watchdog | 01 - VBHOT0 threshold (37.75%)
  BQ25892_REG01_BHOT_TH_31              = (0x02 << 6),    // REG_RST & Watchdog | 10 - VBHOT2 threshold (31.25%)
  BQ25892_REG01_BHOT_DISABLE            = (0x03 << 6),    // REG_RST & Watchdog | 11 - Disable boost mode thermal protection
  BQ25892_REG01_BHOT_MSK                = (0x03 << 6),    //                    | BHOT bit mask
  BQ25892_REG01_BCOLD                   = (0x20),         // REG_RST & Watchdog | Boost mode cold temperature monitor threshold
                                                          //                    | 0 - VBCOLD0 threshold (77%) (default)
                                                          //                    | 1 - VBCOLD1 threshold (80%)
  BQ25892_REG01_VINDPM_OS_1600          = (0x10),         // REG_RST            | Input voltage limit offset, 1600mV
  BQ25892_REG01_VINDPM_OS_800           = (0x08),         // REG_RST            | Input voltage limit offset, 800mV
  BQ25892_REG01_VINDPM_OS_400           = (0x04),         // REG_RST            | Input voltage limit offset, 400mV
  BQ25892_REG01_VINDPM_OS_200           = (0x02),         // REG_RST            | Input voltage limit offset, 200mV
  BQ25892_REG01_VINDPM_OS_100           = (0x01),         // REG_RST            | Input voltage limit offset, 100mV
  BQ25892_REG01_VINDPM_OS_MSK           = (0x1F),         //                    | VINDPM_OS bit mask 
                                                          // ---------------------------------
  BQ25892_REG01_DEFAULT                 = (0x06),         // Default state of the register
};


/* REG02 R/W */
const                                                     // Reset by           | Description
{                                                         // --------------------------------
  BQ25892_REG02_CONV_START              = (0x80),         // REG_RST & Watchdog | ADC conversion start control
  BQ25892_REG02_CONV_RATE               = (0x40),         // REG_RST & Watchdog | ADC conversion rate selection
  BQ25892_REG02_BOOST_FREQ              = (0x20),         // REG_RST & Watchdog | Boost mode frequency selection
  BQ25892_REG02_ICO_EN                  = (0x10),         // REG_RST            | Input current optimizer (ICO) enable
  BQ25892_REG02_HVDCP_EN                = (0x08),         // REG_RST            | High voltage DCP enable (bq25890 only)
  BQ25892_REG02_MAXC_EN                 = (0x04),         // REG_RST            | MaxCharge adapter enable (bq25890 only)
  BQ25892_REG02_FORCE_DPDM              = (0x02),         // REG_RST & Watchdog | Force D+/D- detection
  BQ25892_REG02_AUTO_DPDM_EN            = (0x01),         // REG_RST            | Automatic D+/D- detection enable
                                                          // --------------------------------
  BQ25892_REG02_DEFAULT                 = (0x1D),         // Default state of the register
};


/* REG03 R/W */
const                                                     // Reset by           | Description
{                                                         // --------------------------------
  BQ25892_REG03_BAT_LOADEN              = (0x80),         // REG_RST & Watchdog | Battery load (IBATLOAD) enable
  BQ25892_REG03_WD_RST                  = (0x40),         // REG_RST & Watchdog | I2C watchdog timer reset
  BQ25892_REG03_OTG_CONFIG              = (0x20),         // REG_RST & Watchdog | Boost (OTG) mode configuration
  BQ25892_REG03_CHG_CONFIG              = (0x10),         // REG_RST & Watchdog | Charge enable configuration
  BQ25892_REG03_SYS_MIN_04              = (0x08),         // REG_RST            | Minimum system voltage limit, 0.4V
  BQ25892_REG03_SYS_MIN_02              = (0x04),         // REG_RST            | Minimum system voltage limit, 0.2V
  BQ25892_REG03_SYS_MIN_01              = (0x02),         // REG_RST            | Minimum system voltage limit, 0.1V
  BQ25892_REG03_SYS_MIN_MSK             = (0x0E),         //                    | SYS_MIN bit mask
  BQ25892_REG03_RESERVED                = (0x01),         // REG_RST & Watchdog | Reserved
                                                          // --------------------------------
  BQ25892_REG03_DEFAULT                 = (0x1A),         // Default state of the register
};


/* REG04 R/W */
const                                                     // Reset by              | Description
{                                                         // -----------------------------------
  BQ25892_REG04_EN_PUMPX                = (0x80),         // Softwareby & Watchdog | Current pulse control enable
  BQ25892_REG04_ICHG_4096               = (0x40),         // Softwareby & Watchdog | Fast charge current limit, 4096mA
  BQ25892_REG04_ICHG_2048               = (0x20),         // Softwareby & Watchdog | Fast charge current limit, 2048mA
  BQ25892_REG04_ICHG_1024               = (0x10),         // Softwareby & Watchdog | Fast charge current limit, 1024mA
  BQ25892_REG04_ICHG_512                = (0x08),         // Softwareby & Watchdog | Fast charge current limit, 512mA
  BQ25892_REG04_ICHG_256                = (0x04),         // Softwareby & Watchdog | Fast charge current limit, 256mA
  BQ25892_REG04_ICHG_128                = (0x02),         // Softwareby & Watchdog | Fast charge current limit, 128mA
  BQ25892_REG04_ICHG_64                 = (0x01),         // Softwareby & Watchdog | Fast charge current limit, 64mA 
  BQ25892_REG04_ICHG_MSK                = (0x7F),         //                       | ICHG bit mask
                                                          // -----------------------------------
  BQ25892_REG04_DEFAULT                 = (0x20),         // Default state of the register
};


/* REG05 R/W */
const                                                     // Reset by              | Description
{                                                         // -----------------------------------
  BQ25892_REG05_IPRECHG_512             = (0x80),         // Softwareby & Watchdog | Precharge current limit, 512mA
  BQ25892_REG05_IPRECHG_256             = (0x40),         // Softwareby & Watchdog | Precharge current limit, 256mA
  BQ25892_REG05_IPRECHG_128             = (0x20),         // Softwareby & Watchdog | Precharge current limit, 128mA
  BQ25892_REG05_IPRECHG_64              = (0x10),         // Softwareby & Watchdog | Precharge current limit, 64mA
  BQ25892_REG05_IPRECHG_MSK             = (0xF0),         //                       | IPRECHG bit mask
  BQ25892_REG05_ITERM_512               = (0x08),         // Softwareby & Watchdog | Termination current limit, 512mA
  BQ25892_REG05_ITERM_256               = (0x04),         // Softwareby & Watchdog | Termination current limit, 256mA
  BQ25892_REG05_ITERM_128               = (0x02),         // Softwareby & Watchdog | Termination current limit, 128mA
  BQ25892_REG05_ITERM_64                = (0x01),         // Softwareby & Watchdog | Termination current limit, 64mA 
  BQ25892_REG05_ITERM_MSK               = (0x0F),         //                       | ITERM bit mask
                                                          // -----------------------------------
  BQ25892_REG05_DEFAULT                 = (0x13),         // Default state of the register
};


/* REG06 R/W */
const                                                     // Reset by              | Description
{                                                         // -----------------------------------
  BQ25892_REG06_VREG_512                = (0x80),         // Softwareby & Watchdog | Charge voltage limit, 512mA
  BQ25892_REG06_VREG_256                = (0x40),         // Softwareby & Watchdog | Charge voltage limit, 256mA
  BQ25892_REG06_VREG_128                = (0x20),         // Softwareby & Watchdog | Charge voltage limit, 128mA
  BQ25892_REG06_VREG_64                 = (0x10),         // Softwareby & Watchdog | Charge voltage limit, 64mA
  BQ25892_REG06_VREG_32                 = (0x08),         // Softwareby & Watchdog | Charge voltage limit, 32mA 
  BQ25892_REG06_VREG_16                 = (0x04),         // Softwareby & Watchdog | Charge voltage limit, 16mA 
  BQ25892_REG06_VREG_MSK                = (0xFC),         //                       | VREG bit mask
  BQ25892_REG06_BATLOWV                 = (0x02),         // Softwareby & Watchdog | Battery precharge to fast charge threshold
  BQ25892_REG06_VRECHG                  = (0x01),         // Softwareby & Watchdog | Battery recharge threshold offset
                                                          // -----------------------------------
  BQ25892_REG06_DEFAULT                 = (0x5E),         // Default state of the register
};


/* REG07 R/W */
const                                                     // Reset by              | Description
{                                                         // -----------------------------------
  BQ25892_REG07_EN_TERM                 = (0x80),         // Softwareby & Watchdog | Charging termination enable
  BQ25892_REG07_STAT_DIS                = (0x40),         // Softwareby & Watchdog | STAT pin disable
  BQ25892_REG07_WATCHDOG_DIS            = (0x00 << 4),    // Softwareby & Watchdog | I2C Watchdog timer setting, 00 – disable Watchdog timer
  BQ25892_REG07_WATCHDOG_40             = (0x01 << 4),    // Softwareby & Watchdog | I2C Watchdog timer setting, 01 – 40s (default)
  BQ25892_REG07_WATCHDOG_80             = (0x02 << 4),    // Softwareby & Watchdog | I2C Watchdog timer setting, 10 – 80s
  BQ25892_REG07_WATCHDOG_160            = (0x03 << 4),    // Softwareby & Watchdog | I2C Watchdog timer setting, 11 – 160s
  BQ25892_REG07_WATCHDOG_MSK            = (0x03 << 4),    // Softwareby & Watchdog | WATCHDOG bit mask
  BQ25892_REG07_EN_TIMER                = (0x08),         // Softwareby & Watchdog | Charging safety timer enable
  BQ25892_REG07_CHG_TIMER_5             = (0x00 << 1),    // Softwareby & Watchdog | Fast charge timer setting, 00 – 5 hrs
  BQ25892_REG07_CHG_TIMER_8             = (0x01 << 1),    // Softwareby & Watchdog | Fast charge timer setting, 01 – 8 hrs
  BQ25892_REG07_CHG_TIMER_12            = (0x02 << 1),    // Softwareby & Watchdog | Fast charge timer setting, 10 – 12 hrs (default)
  BQ25892_REG07_CHG_TIMER_20            = (0x03 << 1),    // Softwareby & Watchdog | Fast charge timer setting, 11 – 20 hrs
  BQ25892_REG07_CHG_TIMER_MSK           = (0x03 << 1),    // Softwareby & Watchdog | CHG_TIMER bit mask
  BQ25892_REG07_JEITA_ISET              = (0x01),         // Softwareby & Watchdog | JEITA low temperature current setting
                                                          // -----------------------------------
  BQ25892_REG07_DEFAULT                 = (0x9D),         // Default state of the register
};


/* REG08 R/W */
const                                                     // Reset by              | Description
{                                                         // -----------------------------------
  BQ25892_REG08_BAT_COMP_80             = (0x80),         // Softwareby & Watchdog | IR compensation resistor setting
  BQ25892_REG08_BAT_COMP_40             = (0x40),         // Softwareby & Watchdog | IR compensation resistor setting
  BQ25892_REG08_BAT_COMP_20             = (0x20),         // Softwareby & Watchdog | IR compensation resistor setting
  BQ25892_REG08_BAT_COMP_MSK            = (0xE0),         //                       | BAT_COMP bit mask
  BQ25892_REG08_VCLAMP_128              = (0x10),         // Softwareby & Watchdog | IR compensation voltage clamp
  BQ25892_REG08_VCLAMP_64               = (0x08),         // Softwareby & Watchdog | IR compensation voltage clamp
  BQ25892_REG08_VCLAMP_32               = (0x04),         // Softwareby & Watchdog | IR compensation voltage clamp
  BQ25892_REG08_VCLAMP_MSK              = (0x1C),         //                       | VCLAMP bit mask
  BQ25892_REG08_TREG_60                 = (0x00),         // Softwareby & Watchdog | Thermal regulation threshold, 00 – 60°C
  BQ25892_REG08_TREG_80                 = (0x01),         // Softwareby & Watchdog | Thermal regulation threshold, 01 – 80°C
  BQ25892_REG08_TREG_100                = (0x02),         // Softwareby & Watchdog | Thermal regulation threshold, 10 – 100°C
  BQ25892_REG08_TREG_120                = (0x03),         // Softwareby & Watchdog | Thermal regulation threshold, 11 – 120°C (default)
  BQ25892_REG08_TREG_MSK                = (0x03),         //                       | TREG bit mask
                                                          // -----------------------------------
  BQ25892_REG08_DEFAULT                 = (0x03),         // Default state of the register
};


/* REG09 R/W */
const                                                     // Reset by              | Description
{                                                         // -----------------------------------
  BQ25892_REG09_FORCE_ICO               = (0x80),         // Softwareby & Watchdog | Force start input current optimizer (ICO)
  BQ25892_REG09_TMR2X_EN                = (0x40),         // Softwareby & Watchdog | Safety timer setting during DPM or thermal regulation
  BQ25892_REG09_BATFET_DIS              = (0x20),         // Softwareby            | Force BATFET off to enable ship mode
  BQ25892_REG09_JEITA_VSET              = (0x10),         // Softwareby & Watchdog | JEITA high temperature voltage setting
  BQ25892_REG09_BATFET_DLY              = (0x08),         // Softwareby            | BATFET turn off delay control
  BQ25892_REG09_BATFET_RST_EN           = (0x04),         // Softwareby            | BATFET full system reset enable
  BQ25892_REG09_PUMPX_UP                = (0x02),         // Softwareby & Watchdog | Current pulse control voltage up enable
  BQ25892_REG09_PUMPX_DN                = (0x01),         // Softwareby & Watchdog | Current pulse control voltage down enable
                                                          // -----------------------------------
  BQ25892_REG09_DEFAULT                 = (0x44),         // Default state of the register
};


/* REG0A R/W */
const                                                     // Reset by              | Description
{                                                         // -----------------------------------
  BQ25892_REG0A_BOOSTV_512              = (0x80),         // Softwareby & Watchdog | Boost mode voltage regulation, 512mV
  BQ25892_REG0A_BOOSTV_256              = (0x40),         // Softwareby & Watchdog | Boost mode voltage regulation, 256mV
  BQ25892_REG0A_BOOSTV_128              = (0x20),         // Softwareby            | Boost mode voltage regulation, 128mV
  BQ25892_REG0A_BOOSTV_64               = (0x10),         // Softwareby & Watchdog | Boost mode voltage regulation, 64mV
  BQ25892_REG0A_BOOSTV_MSK              = (0xF0),         //                       | BOOSTV bit mask
  BQ25892_REG0A_RESERVED                = (0x08),         // Softwareby & Watchdog | Reserved
  BQ25892_REG0A_BOOST_LIM_05            = (0x00),         // Softwareby & Watchdog | Boost mode current limit, 000: 0.5A
  BQ25892_REG0A_BOOST_LIM_075           = (0x01),         // Softwareby & Watchdog | Boost mode current limit, 001: 0.75A
  BQ25892_REG0A_BOOST_LIM_12            = (0x02),         // Softwareby & Watchdog | Boost mode current limit, 010: 1.2A
  BQ25892_REG0A_BOOST_LIM_14            = (0x03),         // Softwareby & Watchdog | Boost mode current limit, 011: 1.4A
  BQ25892_REG0A_BOOST_LIM_165           = (0x04),         // Softwareby & Watchdog | Boost mode current limit, 100: 1.65A
  BQ25892_REG0A_BOOST_LIM_1875          = (0x05),         // Softwareby & Watchdog | Boost mode current limit, 101: 1.875A
  BQ25892_REG0A_BOOST_LIM_215           = (0x06),         // Softwareby & Watchdog | Boost mode current limit, 110: 2.15A
  BQ25892_REG0A_BOOST_LIM_245           = (0x07),         // Softwareby & Watchdog | Boost mode current limit, 111: 2.45A
  BQ25892_REG0A_BOOST_LIM_MSK           = (0x07),         // Softwareby & Watchdog | BOOST_LIM bit mask
                                                          // -----------------------------------
  BQ25892_REG0A_DEFAULT                 = (0x73),         // Default state of the register
};


/* REG0B Read only */
const                                                     // Reset by | Description
{                                                         // ----------------------
  BQ25892_REG0B_VBUS_STAT_NO_INP        = (0x00 << 5),    // N/A      | VBUS status register, 000: no input
  BQ25892_REG0B_VBUS_STAT_SDP           = (0x01 << 5),    // N/A      | VBUS status register, 001: USB host SDP
  BQ25892_REG0B_VBUS_STAT_ADAPTER       = (0x02 << 5),    // N/A      | VBUS status register, 010: adapter (3.25A)
  BQ25892_REG0B_VBUS_STAT_OTG           = (0x07 << 5),    // N/A      | VBUS status register, 111: OTG
  BQ25892_REG0B_VBUS_STAT_MSK           = (0x07 << 5),    // N/A      | VBUS_STAT bit mask
  BQ25892_REG0B_CHRG_STAT_NO            = (0x00 << 3),    // N/A      | Charging status, 00 – not charging
  BQ25892_REG0B_CHRG_STAT_PRE           = (0x01 << 3),    // N/A      | Charging status, 01 – pre-charge ( < VBATLOWV)
  BQ25892_REG0B_CHRG_STAT_FAST          = (0x02 << 3),    // N/A      | Charging status, 10 – fast charging
  BQ25892_REG0B_CHRG_STAT_TERM          = (0x03 << 3),    // N/A      | Charging status, 11 – charging termination done
  BQ25892_REG0B_CHRG_STAT_MSK           = (0x03 << 3),    // N/A      | CHRG_STAT bit mask
  BQ25892_REG0B_PG_STAT                 = (0x04),         // N/A      | Power good status
  BQ25892_REG0B_RESERVED                = (0x02),         // N/A      | Reserved: always reads 0
  BQ25892_REG0B_VSYS_STAT               = (0x01),         // N/A      | VSYS regulation status
};


/* REG0C Read only */
const                                                     // Reset by | Description
{                                                         // ----------------------
  BQ25892_REG0C_WATCHDOG_FAULT          = (0x80),         // N/A      | Watchdog fault status
  BQ25892_REG0C_BOOST_FAULT             = (0x40),         // N/A      | Boost mode fault status
  BQ25892_REG0C_CHRG_FAULT_NORM         = (0x00 << 4),    // N/A      | Charge fault status, 00 – normal
  BQ25892_REG0C_CHRG_FAULT_INP          = (0x01 << 4),    // N/A      | Charge fault status, 01 – input fault
  BQ25892_REG0C_CHRG_FAULT_THERM        = (0x02 << 4),    // N/A      | Charge fault status, 10 - thermal shutdown
  BQ25892_REG0C_CHRG_FAULT_CHARGE       = (0x03 << 4),    // N/A      | Charge fault status, 11 – charge safety timer expiration
  BQ25892_REG0C_CHRG_FAULT_MSK          = (0x03 << 4),    // N/A      | CHRG_FAULT bit mask
  BQ25892_REG0C_BAT_FAULT               = (0x08),         // N/A      | Battery fault status
  BQ25892_REG0C_NTC_F_BUCK_NORM         = (0x00),         // N/A      | NTC fault status, 000 – normal
  BQ25892_REG0C_NTC_F_BUCK_WARM         = (0x02),         // N/A      | NTC fault status, 010 – TS warm
  BQ25892_REG0C_NTC_F_BUCK_COOL         = (0x03),         // N/A      | NTC fault status, 011 – TS cool
  BQ25892_REG0C_NTC_F_BUCK_COLD         = (0x05),         // N/A      | NTC fault status, 101 – TS cold
  BQ25892_REG0C_NTC_F_BUCK_HOT          = (0x06),         // N/A      | NTC fault status, 110 – TS hot
  BQ25892_REG0C_NTC_F_BOOST_NORM        = (0x00),         // N/A      | NTC fault status, 000 – normal
  BQ25892_REG0C_NTC_F_BOOST_COLD        = (0x05),         // N/A      | NTC fault status, 101 – TS cold
  BQ25892_REG0C_NTC_F_BOOST_HOT         = (0x06),         // N/A      | NTC fault status, 110 – TS hot
  BQ25892_REG0C_NTC_FAULT_MSK           = (0x07),         // N/A      | NTC_FAULT bit mask
};


/* REG0D R/W */
const                                                     // Reset by   | Description
{                                                         // ------------------------
  BQ25892_REG0D_FORCE_VINDPM            = (0x80),         // Softwareby | VINDPM threshold setting method
  BQ25892_REG0D_VINDPM_6400             = (0x40),         // Softwareby | Absolute VINDPM threshold, 6400mV
  BQ25892_REG0D_VINDPM_3200             = (0x20),         // Softwareby | Absolute VINDPM threshold, 3200mV
  BQ25892_REG0D_VINDPM_1600             = (0x10),         // Softwareby | Absolute VINDPM threshold, 1600mV
  BQ25892_REG0D_VINDPM_800              = (0x08),         // Softwareby | Absolute VINDPM threshold, 800mV
  BQ25892_REG0D_VINDPM_400              = (0x04),         // Softwareby | Absolute VINDPM threshold, 400mV
  BQ25892_REG0D_VINDPM_200              = (0x02),         // Softwareby | Absolute VINDPM threshold, 200mV
  BQ25892_REG0D_VINDPM_100              = (0x01),         // Softwareby | Absolute VINDPM threshold, 100mV
  BQ25892_REG0D_VINDPM_MSK              = (0x7F),         //            | VINDPM bit mask
                                                          // ------------------------
  BQ25892_REG0D_DEFAULT                 = (0x12),         // Default state of the register
};


/* REG0E Read only */
const                                                     // Reset by | Description
{                                                         // ----------------------
  BQ25892_REG0E_FORCE_THERM_STAT        = (0x80),         // N/A      | Thermal regulation status
  BQ25892_REG0E_BATV_1280               = (0x40),         // N/A      | ADC conversion of battery voltage (VBAT), 1280mV
  BQ25892_REG0E_BATV_640                = (0x20),         // N/A      | ADC conversion of battery voltage (VBAT), 640mV
  BQ25892_REG0E_BATV_320                = (0x10),         // N/A      | ADC conversion of battery voltage (VBAT), 320mV
  BQ25892_REG0E_BATV_160                = (0x08),         // N/A      | ADC conversion of battery voltage (VBAT), 160mV
  BQ25892_REG0E_BATV_80                 = (0x04),         // N/A      | ADC conversion of battery voltage (VBAT), 80mV
  BQ25892_REG0E_BATV_40                 = (0x02),         // N/A      | ADC conversion of battery voltage (VBAT), 40mV
  BQ25892_REG0E_BATV_20                 = (0x01),         // N/A      | ADC conversion of battery voltage (VBAT), 20mV
  BQ25892_REG0E_BATV_MSK                = (0x7F),         //          | BATV bit mask
                                                          // ----------------------
  BQ25892_REG0E_DEFAULT                 = (0x00),         // Default state of the register, 2.304V
};


/* REG0F Read only */
const                                                     // Reset by | Description
{                                                         // ----------------------
  BQ25892_REG0F_RESERVED                = (0x80),         // N/A      | Reserved: always reads 0
  BQ25892_REG0F_SYSV_1280               = (0x40),         // N/A      | ADDC conversion of system voltage (VSYS), 1280mV
  BQ25892_REG0F_SYSV_640                = (0x20),         // N/A      | ADDC conversion of system voltage (VSYS), 640mV
  BQ25892_REG0F_SYSV_320                = (0x10),         // N/A      | ADDC conversion of system voltage (VSYS), 320mV
  BQ25892_REG0F_SYSV_160                = (0x08),         // N/A      | ADDC conversion of system voltage (VSYS), 160mV
  BQ25892_REG0F_SYSV_80                 = (0x04),         // N/A      | ADDC conversion of system voltage (VSYS), 80mV
  BQ25892_REG0F_SYSV_40                 = (0x02),         // N/A      | ADDC conversion of system voltage (VSYS), 40mV
  BQ25892_REG0F_SYSV_20                 = (0x01),         // N/A      | ADDC conversion of system voltage (VSYS), 20mV
  BQ25892_REG0F_SYSV_MSK                = (0x7F),         //          | BATV bit mask
                                                          // ----------------------
  BQ25892_REG0F_DEFAULT                 = (0x00),         // Default state of the register, 2.304V
};


/* REG10 Read only */
const                                                     // Reset by | Description
{                                                         // ----------------------
  BQ25892_REG10_RESERVED                = (0x80),         // N/A      | Reserved: always reads 0
  BQ25892_REG10_TSPCT_29                = (0x40),         // N/A      | ADC conversion of TS voltage (TS) as percentage of REGN, 29.76%
  BQ25892_REG10_TSPCT_14                = (0x20),         // N/A      | ADC conversion of TS voltage (TS) as percentage of REGN, 14.88%
  BQ25892_REG10_TSPCT_7                 = (0x10),         // N/A      | ADC conversion of TS voltage (TS) as percentage of REGN, 7.44%
  BQ25892_REG10_TSPCT_3                 = (0x08),         // N/A      | ADC conversion of TS voltage (TS) as percentage of REGN, 3.72%
  BQ25892_REG10_TSPCT_1                 = (0x04),         // N/A      | ADC conversion of TS voltage (TS) as percentage of REGN, 1.86%
  BQ25892_REG10_TSPCT_09                = (0x02),         // N/A      | ADC conversion of TS voltage (TS) as percentage of REGN, 0.93%
  BQ25892_REG10_TSPCT_04                = (0x01),         // N/A      | ADC conversion of TS voltage (TS) as percentage of REGN, 0.465%
  BQ25892_REG10_TSPCT_MSK               = (0x7F),         //          | TSPCT bit mask
                                                          // ----------------------
  BQ25892_REG10_DEFAULT                 = (0x00),         // Default state of the register, 21%
};


/* REG11 Read only */
const                                                     // Reset by | Description
{                                                         // ----------------------
  BQ25892_REG11_VBUS_GD                 = (0x80),         // N/A      | VBUS good status
  BQ25892_REG11_VBUSV_6400              = (0x40),         // N/A      | ADC conversion of VBUS voltage (VBUS), 6400mV
  BQ25892_REG11_VBUSV_3200              = (0x20),         // N/A      | ADC conversion of VBUS voltage (VBUS), 3200mV
  BQ25892_REG11_VBUSV_1600              = (0x10),         // N/A      | ADC conversion of VBUS voltage (VBUS), 1600mV
  BQ25892_REG11_VBUSV_800               = (0x08),         // N/A      | ADC conversion of VBUS voltage (VBUS), 800mV
  BQ25892_REG11_VBUSV_400               = (0x04),         // N/A      | ADC conversion of VBUS voltage (VBUS), 400mV
  BQ25892_REG11_VBUSV_200               = (0x02),         // N/A      | ADC conversion of VBUS voltage (VBUS), 200mV
  BQ25892_REG11_VBUSV_100               = (0x01),         // N/A      | ADC conversion of VBUS voltage (VBUS), 100mV
  BQ25892_REG11_VBUSV_MSK               = (0x7F),         //          | VBUSV bit mask
                                                          // ----------------------
  BQ25892_REG11_DEFAULT                 = (0x00),         // Default state of the register, 2.6V
};


/* REG12 Read only */
const                                                     // Reset by | Description
{                                                         // ----------------------
  BQ25892_REG12_UNUSED                  = (0x80),         // N/A      | Always reads 0
  BQ25892_REG12_ICHGR_3200              = (0x40),         // N/A      | ADC conversion of charge current (IBAT) when VBAT > VBATSHORT, 3200mA
  BQ25892_REG12_ICHGR_1600              = (0x20),         // N/A      | ADC conversion of charge current (IBAT) when VBAT > VBATSHORT, 1600mA
  BQ25892_REG12_ICHGR_800               = (0x10),         // N/A      | ADC conversion of charge current (IBAT) when VBAT > VBATSHORT, 800mA
  BQ25892_REG12_ICHGR_400               = (0x08),         // N/A      | ADC conversion of charge current (IBAT) when VBAT > VBATSHORT, 400mA
  BQ25892_REG12_ICHGR_200               = (0x04),         // N/A      | ADC conversion of charge current (IBAT) when VBAT > VBATSHORT, 200mA
  BQ25892_REG12_ICHGR_100               = (0x02),         // N/A      | ADC conversion of charge current (IBAT) when VBAT > VBATSHORT, 100mA
  BQ25892_REG12_ICHGR_50                = (0x01),         // N/A      | ADC conversion of charge current (IBAT) when VBAT > VBATSHORT, 50mA
  BQ25892_REG12_ICHGR_MSK               = (0x7F),         //          | ICHGR bit mask
                                                          // ----------------------
  BQ25892_REG12_DEFAULT                 = (0x00),         // Default state of the register, 0mA
};


/* REG13 Read only */
const                                                     // Reset by | Description
{                                                         // ----------------------
  BQ25892_REG13_VDPM_STAT               = (0x80),         // N/A      | VINDPM status
  BQ25892_REG13_IDPM_STAT               = (0x40),         // N/A      | IINDPM status
  BQ25892_REG13_IDPM_LIM_1600           = (0x20),         // N/A      | Input current limit in effect while input current optimizer (ICO) is enabled, 1600mA
  BQ25892_REG13_IDPM_LIM_800            = (0x10),         // N/A      | Input current limit in effect while input current optimizer (ICO) is enabled, 800mA
  BQ25892_REG13_IDPM_LIM_400            = (0x08),         // N/A      | Input current limit in effect while input current optimizer (ICO) is enabled, 400mA
  BQ25892_REG13_IDPM_LIM_200            = (0x04),         // N/A      | Input current limit in effect while input current optimizer (ICO) is enabled, 200mA
  BQ25892_REG13_IDPM_LIM_100            = (0x02),         // N/A      | Input current limit in effect while input current optimizer (ICO) is enabled, 100mA
  BQ25892_REG13_IDPM_LIM_50             = (0x01),         // N/A      | Input current limit in effect while input current optimizer (ICO) is enabled, 50mA
  BQ25892_REG13_IDPM_LIM_MSK            = (0x3F),         //          | IDPM_LIM bit mask
                                                          // ----------------------
  BQ25892_REG13_DEFAULT                 = (0x00),         // Default state of the register, 100mA
};


/* REG14 Read only (except REG_RST is R/W) */
const                                                     // Reset by | Description
{                                                         // ----------------------
  BQ25892_REG14_REG_RST                 = (0x80),         // N/A      | Register reset
  BQ25892_REG14_ICO_OPTIMIZED           = (0x40),         // N/A      | Input current optimizer (ICO) status
  BQ25892_REG14_PN                      = (0x00 << 3),    // N/A      | Device configuration, 000: bq25892
  BQ25892_REG14_PN_MSK                  = (0x07 << 3),    // N/A      | PN bit mask
  BQ25892_REG14_TS_PROFILE              = (0x04),         // N/A      | Temperature profile
  BQ25892_REG14_DEV_REV_MSK             = (0x03),         // N/A      | DEV_REV bit mask
};


/* Register masks */
const
{
  BQ25892_ADDR_MSK         = (0x00FF),
  BQ25892_RO               = (0x1000),
  BQ25892_RW_MSK           = (0xF000),
}

/* Available register */
const                                        // Register permissions
{
  BQ25892_REG00 = (0x00),                    // R/W  
  BQ25892_REG01 = (0x01),                    // R/W
  BQ25892_REG02 = (0x02),                    // R/W  
  BQ25892_REG03 = (0x03),                    // R/W  
  BQ25892_REG04 = (0x04),                    // R/W  
  BQ25892_REG05 = (0x05),                    // R/W  
  BQ25892_REG06 = (0x06),                    // R/W  
  BQ25892_REG07 = (0x07),                    // R/W  
  BQ25892_REG08 = (0x08),                    // R/W  
  BQ25892_REG09 = (0x09),                    // R/W  
  BQ25892_REG0A = (0x0A),                    // R/W  
  BQ25892_REG0B = (0x0B | BQ25892_RO),       // R 
  BQ25892_REG0C = (0x0C | BQ25892_RO),       // R  
  BQ25892_REG0D = (0x0D),                    // R/W
  BQ25892_REG0E = (0x0E | BQ25892_RO),       // R
  BQ25892_REG0F = (0x0F | BQ25892_RO),       // R
  BQ25892_REG10 = (0x10 | BQ25892_RO),       // R  
  BQ25892_REG11 = (0x11 | BQ25892_RO),       // R 
  BQ25892_REG12 = (0x12 | BQ25892_RO),       // R 
  BQ25892_REG13 = (0x13 | BQ25892_RO),       // R
  BQ25892_REG14 = (0x14),                    // R (except bit 7 is R/W)
  BQ25892_N_REG                              // Number of registers
}

/**
 * Initialisation of I2C communication with BQ25892
 *
 * This function copies the I2C port and address to the given structure.
 * 
 * @param handle:TBQ25892_Handle  - Structure that contains BQ25892 interface configuration
 * @param i2c:s32                 - I2C port used for communication
 * @param addr:s32                - Address of the I2C device
 */
stock BQ25892_Init(handle[TBQ25892_Handle], i2c, addr)
{
  handle.i2c  = i2c;
  handle.addr = addr;
}


/**
 * Writes register of BQ25892
 * 
 * This function checks the given register and sends 2 bytes of data to the BQ25892.
 * If the mask is set to only the bits which should be modified, the state of the other bits is preserved.
 * 
 * @param handle:TBQ25892_Handle    - Structure that contains BQ25892 interface configuration
 * @param iReg:s32                  - Address of the register
 * @param iVal:s32                  - Value to be written to the register
 * @param iMask:s32                 - Mask to preserve the state of the register. 
 *                                    Only bits for which a "1" is set in the mask are modified
 * @return s32                      - OK:    If successful
 *                                  - < OK:  An error occurred. See the manual for all error codes.
 *                                  - ERROR: If the register address is invalid or the register is read only
 */
stock BQ25892_RegWrite(handle[TBQ25892_Handle], iReg, iVal, iMask=0xFF)
{
  new iResult;
  new aData{2};
  new iTmp;
  new iAddr, iAccess;

  // Gets address and data type
  iAddr   = iReg & BQ25892_ADDR_MSK;
  iAccess = iReg & BQ25892_RW_MSK;

  if((iAddr < 0) || (iAddr >= BQ25892_N_REG))
    return(ERROR);

  // If the register is read only -> Returns "ERROR"
  if(iAccess == BQ25892_RO)
    return (ERROR);

  // If an attempt is made to write one of the read only bits of REG14 -> Return "ERROR"
  // Note: Write access is only permitted to "BQ25892_REG14_REG_RST" bit    
  if(iAddr == BQ25892_REG14 && iMask != BQ25892_REG14_REG_RST)
    return(ERROR);

  iResult = BQ25892_RegRead(handle, iAddr, iTmp);
  if (iResult >= OK)
  {
    // Ensures that only the bits specified by the mask are modified	
    aData{0} = iAddr;
    aData{1} = iTmp;
    aData{1} &= ~iMask;
    aData{1} |= (iVal & iMask);

    iResult = rM2M_I2cCom(handle.i2c, handle.addr, aData, 2, 0);
  }

  return(iResult);
}

/**
 * Reads register of BQ25892
 * 
 * Use this function to read a specific value of a BQ25892 register.
 * 
 * @param handle:TBQ25892_Handle    - Structure that contains BQ25892 interface configuration
 * @param iReg:s32                  - Address of the register
 * @param iVal:s32                  - Value to be written to the register
 * @return s32                      - OK:    If successful
 *                                  - < OK:  An error occurred. See the manual for all error codes.
 *                                  - ERROR: If the register address is invalid
 */
stock BQ25892_RegRead(handle[TBQ25892_Handle], iReg, &iVal)
{
  new iResult;
  new aData{2};
  new iAddr;

  // Gets register address
  iAddr = iReg & BQ25892_ADDR_MSK;

  if((iAddr < 0) || (iAddr >= BQ25892_N_REG))
    return(ERROR);

  aData{0} = iAddr;
  iResult  = rM2M_I2cCom(handle.i2c, handle.addr, aData, 1, 1);
  if(iResult >= OK)
  {
    // Returns read data
    iVal = aData{0};
  }

  return(iResult);
}


/**
 * Enables shipping mode of the BQ25892
 * 
 * The function enables BQ25892 shipping mode, i.e. battery is disconnected after BATFET_DLY (typ. 10-15s).
 * The system will only be powered down afterwards if no input voltage (VDC and/or VUSB) is provided. 
 * This feature is useful to save capacity, if battery is plugged and not yet in use (shipping mode).
 * The shipping mode will be exited if the POWER button is pressed (> 250ms).
 * 
 * @param handle:TBQ25892_Handle	- Structure that contains BQ25892 interface configuration
 * @return s32                 		- OK:		    If successful
 *                                - < OK:     BATFET_DIS could not be set
 */
stock BQ25892_EnableShippingMode(handle[TBQ25892_Handle])
{
  new iResult;

  // Writes REG09: set BATFET_DIS (turns off after delay)
  iResult = BQ25892_RegWrite(handle, BQ25892_REG09, BQ25892_REG09_BATFET_DIS, BQ25892_REG09_BATFET_DIS);
  if(iResult < OK)
    printf("BQ25892: set BATFET_DIS failed with %d\r\n", iResult);

  return(iResult);
}


/**
 * Resets BQ25892 registers
 * 
 * The function resets all BQ25892 registers which can be reset by REG_RST.
 * See the constant definitions or the datasheet for all registers which are reset by REG_RST.
 * The REG_RST bit is set to 0 if the reset is complete.
 * 
 * @param handle:TBQ25892_Handle	- Structure that contains BQ25892 interface configuration
 * @return s32                 		- OK:		    If successful
 *                                - < OK:     REG_RST could not be set.
 */
stock BQ25892_RegisterReset(handle[TBQ25892_Handle])
{
  new iResult;

  // Resets all registers by setting REG_RST
  iResult = BQ25892_RegWrite(handle, BQ25892_REG14, BQ25892_REG14_REG_RST, BQ25892_REG14_REG_RST);
  if(iResult < OK)
    printf("BQ25892: Register reset failed with %d\r\n", iResult);

  return(iResult);
}