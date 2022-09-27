USB CDC application {#usb_cdc}
======================================================================
## Overview
This application is an example app about how to implement USB CDC.

## USB feature

- To enable the USB-Data functionality it is mandatory to define the next macro in custom_config_qspi.h:
~~~{.c}
  #define dg_configUSE_USB_ENUMERATION 1
~~~
- The USB stack framework included in the SDK is the Segger emUSB-Device. 
  Please refer to SEGGER emUSB documentation for more information at link:
  https://www.segger.com/emusb.html

**Notes:**

   - The charger functionality can be also enabled along with the USB-Data functionality.
     To enable the USB-Charger functionality define the next macro in custom_config_qspi.h:
~~~{.c}
     #define dg_configUSE_SYS_CHARGER 1
~~~
   - The emUSB stack framework is provided in object code in the 'sdk/interfaces/usb' library.
     It is required the library to be included in the project to be able to build the project with USB-Data functionality.
   - The emUSB stack framework is NOT needed for USB-Charger only functionality.

## Configurable parameters

- The Renesas/Dialog default values for USB PID/VID used in the example can be used during development period. 
  They should be changed to PID/VID of the product developer company's for the end-user product.
  - USB PID/VID and com port name in usb_cdc_vmsd.c
  - Windows driver(dialog_usb.inf file)

## Operation of CDC

- Connect the USB1 on a proDK motherboard
- Write the application to the device QSPI FLASH.
- Run the example application by pressing once the K2(RST) button on the daughterboard.
- Connect USB on the daughterboard to Host-PC (Windows, Linux, OS X, Android, etc).
- Confirm that a new COMxx port appears at the Host-PC device manager.
	* For MS Windows, if asked for a driver, use the `utilities/windows/cdc/dialog_usb.inf` to install it.
	* `USB CDC serial port emulation (COMxx)` in Ports(COM & LPT) of device manager should appear.
- Open the new COMxx port with a terminal emulation application (i.e. Tera Term, Putty, RealTerm, etc).
- Type any text on the terminal program. The example application will echo back whatever received over the COMxx port.
- Confirm the typed text is echoed back in the terminal emulation application window.


