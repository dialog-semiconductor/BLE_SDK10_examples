# USB CDC HID

## Example description

This application is an example application implementing the CDC + HID USB profiles

## HW and SW configuration

* **Hardware configuration**

  * This example runs on the DA1469x Bluetooth Smart SoC devices.
  * The DA1469x Pro Development or USB kit is needed for this example.
  * Connect the Development kit to the host computer.

* **Software configuration**
  * This example requires:
  * Smartsnippets Studio V2.0.18 or greater.
  * SDK 10.0.12.x
  * **SEGGER’s J-Link** tools should be downloaded and installed.

## How to run the example

* build the example
* Flash the example on the board

The device will be detected as both a mouse and a CDC device.

The CDC will echo any character sent to it. For testing, open your favorite serial terminal application (Pyserial, Putty, teraterm...)

Open the *COM<Number>* on windows, or */dev/cu.usbmodem123456781* in macOS, or */dev/ttyACM0* on linux. Type some character.

For the HID device, by default the application is doing nothing. It's possible to generate mouse movement by uncommenting the following statement in the usb_cdc_hid.c file:

```c
USBD_HID_Write(usb_hid_h, movex, sizeof(movex), 0);
```

This will send a mouse cursor right and down movement every 50ms. The function will have to be adapted for movement sensor to generator real input.
