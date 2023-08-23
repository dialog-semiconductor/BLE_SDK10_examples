USB-C power delivery using STUSB1602
======================================================

## Example Description

This project demonstrates USB-C power delivery using the STUSB1602 USB-C PD 3.0 analog front-end controller.
It is based on the STUSB1602 SW library package for STM32L4R5 (STSW-STUSB015_v1.0):
https://github.com/usb-c/STUSB1602

Features:
* Includes software USB-PD stack (partially as a linked library). The STUSB1602 is just an analog front-end controller, the USB-C power delivery handshake is done in software on the DA1469x.
* Supports two configurable USB-C ports, in this project configured as dockingstation in pass-through mode:
   - Port 0: USB-C sink, accepts up to 65W (20V, 3.25A)
   - Port 1: USB-C source, supplies what is supplied to the sink
* See [STUSB1602 SW drivers](https://github.com/usb-c/STUSB1602) for full list of features
* UART debugging output

## HW and SW configuration
* **Hardware configuration**

    - This example runs on DA1469x Bluetooth Smart SoC devices.
    - A custom board is required for this example. It may also be posssible to use development boards for the DA1469x and STUSB1602, but this is not documented here. The board requires:
       - 2 x STUSB1602 (sink and source): each STUSB1602 requires 1x SPI + 1x I2C + 3x GPIO, the I2C bus is shared between both chips
       - Power supply (5V buck converter) to run the board from the sink voltage (20V)
       - Power switch (mosfet) controlled by VIN_ENA to enabled forwarding of the sink voltage (20V) to the source, instead of the 5V normally provided.
       - Please refer to the [STUSB1602 datasheet](https://www.st.com/en/interfaces-and-transceivers/stusb1602.html) for example schematics.

        **The following wiring is needed before starting the app. The pins settings are located in `peripheral_setup.h`**
    
        | Signal    | DA1469x from Sink    | DA1469x from Source | DA1469x shared |
        |:---------:|:--------------------:|:-------------------:|:--------------:|
        | VIN_ENA   |                      |                     | P0_5           |
        | SCL       |                      |                     | P0_29          |
        | SDA       |                      |                     | P0_31          |
        | MOSI      | P1_5                 | P1_2                |                |
        | MISO      | P0_30                | P0_27               |                |
        | SCLK      | P1_4                 | P1_6                |                |
        | CS_N      | P0_17                | P1_3                |                |
        | TX_EN     | P1_7                 | P1_11               |                |
        | ALERT_N   | P0_28                | P1_10               |                |
        | RESET     | P0_26                | P1_8                |                |
      
    - For debugging output connect the UART to the host computer.
  
        **UART terminal with the following settings is needed to observe the debugging output**

        | Setting      | Value    |
        |:------------:|:--------:|
        | Baudrate     | 115200   |
        | Data bits    | 8        |
        | Stop bits    | 1        |
        | Parity       | None     |

* **Software configuration**
  - Smartsnippets Studio V2.0.16 or greater.
  - Download the latest SDK version (10.0.12.146).
  - SEGGER J-Link tools should be downloaded and installed.
  - USB-PD stack settings are located in `usbpd_def.h`

## How to run the example

### Initial Setup

- Download the source code from the Support Website.
- Import the project into your workspace.
- Compile and launch RAM or QSPI target
- Run from RAM or load in the flash

### Connect USB-C charger and consumer

- Connect USB-C Power Delivery charger to port 0 (sink)
- Connect USB-C Power Delivery consumer to port 1 (source)

Example output:
```
USB power delivery
usbpd (0): device id 4
usbpd (1): device id 4
usbpd core mem: 1576
usbpd (0): enabled
usbpd (0): cable attach
usbpd (0): vol = 5000
usbpd (1): cable detach
usbpd (0): request 20000 mV, 3000 mA
usbpd (0): vol = 20000
usbpd (0): cur = 3000
usbpd (1): enabled
usbpd (1): cable attach
usbpd (1): cur = 3000
usbpd (1): accept 20000 mV 3000 mA for operating current from 3000 to 3000 mA
usbpd (1): vol = 20000
usbpd (1): cur = 3000
```

## Known Limitations

- Some workarounds for issues in the USB-PD stack can be disabled in `usbpd_def.h`
- Further information at page [STUSB1602 SW drivers](https://github.com/usb-c/STUSB1602)
- Please check and refer to the following application note for [known hardware limitations](https://www.dialog-semiconductor.com/products/da1469x-product-family "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software "Forum link").
- You can refer also for the Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.

## License

The STUSB1602 SW drivers are licensed by ST under Ultimate Liberty license (SLA0044).
You may obtain a copy of the License at https://www.st.com/SLA0044
