/**
 * rapidM2M Mx specific include file
 */
#pragma library Mx
#include wifi

/**
 * <summary>
 *   Sys info structure.
 * </summary>
 * <param name="VIn">
 *   Input Voltage [mV]
 * </param>
 * <param name="VAux">
 *   Auxiliary voltage [mV]
 * </param>
 */
#define TMx_SysValue[.VIn, .VAux]

/**
 * <summary>
 *   Available GPIOs.
 * </summary>
 */
const
{
  GPIO_0 = 0,               
  GPIO_1,
  GPIO_2,
  GPIO_3,
  GPIO_4,
  GPIO_5,
  GPIO_NUM,
};

/**
 * <summary>
 *   Led Modes.
 * </summary>
 */
const
{
  MX_LED_MODE_INTERNAL = 0, // led's are controlled by FW
  MX_LED_MODE_SCRIPT,       // led's are controlled by Script
};

/**
 * <summary>
 *   Retrieve sys values of device.
 * </summary>
 *
 * <param name="values">
 *   Structure where sys values are filled in.
 * </param>
 * <param name="len">
 *   length of values structure, max size according to TMx_SysValue
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li><em>used size of id struct</em></li>
 *     <li><em>ERROR</em> - if address/length pair for id struct is invalid or a sys value is invalid.</li>
 *   </ul>
 * </returns>
 */
native Mx_GetSysValues(values[TMx_SysValue], len=sizeof values);

/**
 * <summary>
 *   Shutdown system.
 *   Script will be disabled, interfaces (e.g. USB, modem) will be closed.
 *   Safe power down is possible afterwards.
 *   Note: System reset is required to reenable full functionality!
 *   
 * </summary>
 *
 * <returns>
 *   <em>OK</em>.
 * </returns>
 */
native Mx_Shutdown();

/**
 * <summary>
 *   Init and configure brownout detection on VIn.
 * </summary>
 *
 * <param name="funcidx">
 *   Index of callback function. Function is called on BrownOut event.
 *   Has to be of type: public func(level);
 * </param>
 * <param name="level">
 *   Voltage level in [mV] for brownout detection.
 * </param>
 *
 * <returns>
 *   <em>OK</em> if successful, otherwise <em>ERROR</em>.
 * </returns>
 */
native Mx_BrownOutInit(funcidx, level);

/**
 * <summary>
 *   Init Led usage.
 * </summary>
 *
 * <param name="mode">
 *   Led mode (see Led Modes).
 * </param>
 *
 * <returns>
 *   <em>OK</em> if successful, <em>ERROR</em> in case of invalid parameters.
 * </returns>
 */
native MxLed_Init(mode);

/**
 * <summary>
 *   Close Led usage (neither FW nor Script control led's).
 * </summary>
 *
 * <returns>
 *   <em>OK</em>.
 * </returns>
 */
native MxLed_Close();

/**
 * <summary>
 *   Turn on led.
 * </summary>
 *
 * <param name="green">
 *   Green led on if true.
 * </param>
 *
 * <returns>
 *   <em>OK</em> if successful, <em>ERROR</em> in case of invalid parameters.
 * </returns>
 */
native MxLed_On(bool:green);

/**
 * <summary>
 *   Turn off led.
 * </summary>
 *
 * <param name="green">
 *   Green led off if true.
 * </param>
 *
 * <returns>
 *   <em>OK</em> if successful, <em>ERROR</em> in case of invalid parameters.
 * </returns>
 */
native MxLed_Off(bool:green);

/**
 * <summary>
 *   Blink with led (500ms On, 500ms Off).
 * </summary>
 *
 * <param name="green">
 *   Green led blink count or forever if 0.
 * </param>
 *
 * <returns>
 *   <em>OK</em> if successful, <em>ERROR</em> in case of invalid parameters.
 * </returns>
 */
native MxLed_Blink(green);

/**
 * <summary>
 *   Flash with led (500ms period, single flash On).
 * </summary>
 *
 * <param name="green">
 *   Green led flash count or forever if 0.
 * </param>
 *
 * <returns>
 *   <em>OK</em> if successful, <em>ERROR</em> in case of invalid parameters.
 * </returns>
 */
native MxLed_Flash(green);

/**
 * <summary>
 *   Flicker with led
 * </summary>
 *
 * <param name="green">
 *   Green led flicker count or forever if 0 .
 * </param>
 *
 * <returns>
 *   <em>OK</em> if successful, <em>ERROR</em> in case of invalid parameters.
 * </returns>
 */
native MxLed_Flicker(green);
