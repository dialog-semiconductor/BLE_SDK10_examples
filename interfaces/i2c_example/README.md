I2C request response example application. {#I2C_example}
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

## Requirements
* **Hardware configuration**

    - This example runs on the DA1469x Bluetooth Smart SoC devices.
    - The DA1469x Pro Development kit is needed for this example.
    - Connect the Development kit to the host computer.

* **UART terminal with the following settings is needed to observe the master task
printouts**

| Setting      | Value    |
|:------------:|:--------:|
| Baudrate     | 115200   |
| Data bits    | 8        |
| Stop bits    | 1        |
| Parity       | None     |


* **The following wiring/pairs (loopback wiring from MASTER to SLAVE) are needed 
before starting the app. The pins settings are located in `peripheral_setup.h` **

| DA1469x     |             |
|:-----------:|:-----------:|
| Master      | Slave       |
| P0_31 (SDA) | P0_19  (SDA) |
| P0_30 (SCL) | P0_18  (SCL) |

                                    MASTER          SLAVE (loopback) 
                                     |  |           |  |             
        .---------.                  |  |           |  |
        |      SDA|---------------------|-----------'  |
        |      SCL|---------------------'--------------'
        |_________|                                     

* **Software configuration**
  - This example requires:
  - Smartsnippets Studio V2.0.14 or greater.
  - SDK 10.0.10.x
  - **SEGGER J-Link** tools should be downloaded and installed.

## Installation procedure

Download the source code and import the project into your workspace.

To install the project follow the [General Installation and Debugging Procedure](@ref install_and_debug_procedure).

## File structure

The following file structure will be created:

* projects/dk_apps/texamples/i2c_example
        * config
                * custom_config_qspi.h
                * custom_config_ram.h
        * include
                * i2c_task.h       
        * sdk
        * startup
        * i2c_task.c 
        * main.c

## Existing build configurations

The template contains build configurations for executing it from RAM or QSPI. 

- `DA1469X-00-Debug_RAM`. The project is built to be run from RAM. The executable is built with debug (-Og) information.
- `DA1469X-00-Debug_QSPI`. The project is built to be run from QSP. The executable is built with debug (-Og) information.
- `DA1469X-00-Release_RAM`. The project is built to be run from RAM. The executable is built with no debug information and size optimization (-Os).
- `DA1469X-00-Release_QSPI`. The project is built to be run from QSPI. The executable is built with no debug information and size optimization (-Os).

## Dependencies / Considerations
- This project can not be shared outside Dialog before checking if the customer has signed the SLA. 


## License

**************************************************************************************

 Copyright (c) 2020 Dialog Semiconductor. All rights reserved.

 This software ("Software") is owned by Dialog Semiconductor. By using this Software
 you agree that Dialog Semiconductor retains all intellectual property and proprietary
 rights in and to this Software and any use, reproduction, disclosure or distribution
 of the Software without express written permission or a license agreement from Dialog
 Semiconductor is strictly prohibited. This Software is solely for use on or in
 conjunction with Dialog Semiconductor products.

 EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.

**************************************************************************************

