#pragma library usbh

/**
 * <summary>
 *   USB Host events.
 * </summary>
 */
const
{
  USBH_EVENT_NONE                 = 0,
  USBH_EVENT_SELECT_CONFIGURATION = 1, // USB device configuration has to be selected
  USBH_EVENT_CLASS_ACTIVE         = 2, // USB device class active
  USBH_EVENT_CLASS_SELECTED       = 3, // USB device class selected
  USBH_EVENT_CONNECTION           = 4, // new device connection detected
  USBH_EVENT_DISCONNECTION        = 5, // device disconnection detected
  USBH_EVENT_UNRECOVERED_ERROR    = 6, // unrecoverable error
  USBH_EVENT_CLASS_ABORTED        = 7, // USB device class unknown

  /* class specific events */
  USBH_EVENT_CDC_RX               = 100, // CDC class data received
  USBH_EVENT_CDC_TXDONE           = 101, // CDC class data transmitted
};

/**
 * <summary>
 *   USB Host device descriptor  structure (USBH_EVENT_CLASS_ACTIVE & USBH_EVENT_CLASS_ABORTED).
 * </summary>
 * <param name="VID">
 *   Vendor ID
 * </param>
 * <param name="PID">
 *   Product ID
 * </param>
 * <param name="Class">
 *   Interface Class
 * </param>
 * <param name="SubClass">
 *   Interface SubClass
 * </param>
 * <param name="Protocol">
 *   Interface Protocol
 * </param>
 * <param name="Mfc">
 *   Device Manufacturer String
 * </param>
 * <param name="Prd">
 *   Device Product String
 * </param>
 * <param name="SN">
 *   Device SerialNumber String
 * </param>
 */
#define TUSBH_DevDesc[.VID, .PID, .Class, .SubClass, .Protocol, .Mfc{32}, .Prd{32}, .SN{32}]

/**
 * <summary>
 *   Init and configure USB Host interface.
 * </summary>
 *
 * <param name="funcidx">
 *   Index of event callback function. Function is called on USB Host events.
 *   Has to be of type: public func(addr, event, const data{}, len);
 * </param>
 *
 * <returns>
 *   <em>OK</em> if successful, otherwise <em>ERROR</em>.
 * </returns>
 */
native USBH_Init(funcidx);

/**
 * <summary>
 *   Close and deactivate USB Host interface.
 * </summary>
 *
 * <returns>
 *   <em>OK</em> if successful, otherwise <em>ERROR</em>.
 * </returns>
 */
native USBH_Close();

/**
 * <summary>
 *   Send CDC data to an USB device.
 *   Event 'USBH_EVENT_CDC_TXDONE' is fired when data was sent
 * </summary>
 *
 * <param name="addr">
 *   USB device address.
 * </param>
 * <param name="data">
 *   data buffer for send operation.
 * </param>
 * <param name="len">
 *   number of bytes to send, max=1024.
 * </param>
 *
 * <returns>
 *   Returns:
 *   <ul>
 *     <li>number of consumed bytes if successful.</li>
 *     <li><em>ERROR</em> - if any other error occured</li>
 *   </ul>
 * </returns>
 */
native USBH_CDCTx(addr, const data{}, len);
