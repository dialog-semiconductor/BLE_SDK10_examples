I2C request response example application.
======================================================

## Overview

This example implements a simple request/response communication scenario over 
the I2C interface using the I2C adapter layer.

Interface I2C1 is configured as I2C master and I2C2 as slave, so there is the  
option to connect the two interfaces in loopback, or connect two SDKs together.

The I2C master initiates a write-then-read transaction sending a request string 
to the slave. 
The I2C slave handles the response with callback functions in ISR context. 
The response message consists of statistics about how many times each callback 
has been called.
Both request and response are printed on the UART terminal by the I2C master.

## HW and SW configuration
* **Hardware configuration**

    - This example runs on the DA1469x Bluetooth Smart SoC devices.
    - The DA1469x Pro Development kit is needed for this example.
    - Connect the Development kit to the host computer.

**UART terminal with the following settings is needed to observe the master task printouts**

        | Setting      | Value    |
        |:------------:|:--------:|
        | Baudrate     | 115200   |
        | Data bits    | 8        |
        | Stop bits    | 1        |
        | Parity       | None     |

**The following wiring/pairs (loopback wiring from MASTER to SLAVE) are needed before starting the app. The pins settings are located in `peripheral_setup.h`**

        | DA1469x     |             |
        |:-----------:|:-----------:|
        | Master      | Slave       |
        | P0_29 (SDA) | P0_19  (SDA) |
        | P0_28 (SCL) | P0_18  (SCL) |


                                    MASTER          SLAVE (loopback) 
                                     |  |           |  |             
        .---------.                  |  |           |  |
        |      SDA|---------------------|-----------'  |
        |      SCL|---------------------'--------------'
        |_________|                                     

* **Software configuration**
  - Smartsnippets Studio V2.0.14 or greater.
  - SDK 10.0.10.x
  - **SEGGER J-Link** tools should be downloaded and installed.

## How to run the example

### Initial Setup

- Download the source code from the Support Website.
- Import the project into your workspace.
- Compile and launch RAM or QSPI target
- Run from RAM or load in the flash


## Known Limitations

- There are No known limitations for this example. But you can check and refer to the following application note for
  [known hardware limitations](https://www.dialog-semiconductor.com/products/da1469x-product-family "known hardware limitations").
- Dialog Software [Forum link](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software "Forum link").
- you can Refer also for the Troubleshooting section in the DA1585x Getting Started with the Development Kit UM-B-049.

