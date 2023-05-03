Dialog Serial Port Service (DSPS)
==================

## Overview

The Dialog Serial Port Service (DSPS) emulates a serial cable communication. Currently, two serial emulators are supported out of the box. That is, the asynchronous UART interface with HW and SW flow control support and the USB CDC class. The DSPS Bluetooth service is also available for Android and iOS devices allowing a serial port to be emulated using either two DA1469x development kits or a single DevKit and a smartphone. The DA1469x device, can function either in the GAP central role (scanner) or GAP advertiser. The smartphone can only function in the GAP central role. 

To achieve the maximum BLE throughput (valid for the UART interface) the following settings are defined, by default:

- UART baud rate set to 1000000 with HW flow control. 
- CPU speed set to 96 MHz.
- Bluetooth connection interval set to 15ms.
- Data length extension (DLE) set to maximum value, that is 251 bytes. The same goes for MTU.

- Usage of 2Mbps PHY.

### HW & SW Configurations

- **Hardware Configurations**
  - This example runs on a DA1469x Bluetooth Smart SoC.
  - A DA1469x Pro or USB  Development Kit is needed for this example.
- **Software Configurations**
  - Download the latest SDK version for the DA1469x family of devices.
  - SEGGER's J-Link tools should be downloaded and installed.


## How to run the example

### Initial Setup

- Download the source code from the Support Website or GitHub.

- Import the `dsps_ble_central` and `dsps_ble_peripheral` projects into your SDK10 workspace folder (there should be no path dependencies in the workspace path). 

- Connect two target devices (assuming that the DSPS APP is not used) to your host PC. In case of the USB CDC interface, the USB port (mounted on the DA1469x daughterboard) should be employed so USB enumeration and data transfer is available once connected to the host PC (USB host).  

- Compile the two mentioned projects by selecting either a release or debug build option. Once compiled, load the generated executable files into the chips by selecting the appropriate python launcher. Currently, two serial cable emulators are supported as depicted below:

  ![dsps_build_options](assets\dsps_build_options.png)

  SUOTA build options are also available for the GAP advertiser to support over-the-air firmware updates. Keep in mind that RAM build options should only be used for debugging purposes. 

- Open a serial terminal of your choice and make sure that the following settings are aligned with the application-defined settings:

  - Baudrate based on the `CFG_UART_SPS_BAUDRATE` configuration macro (valid for the UART interface only). 
  - Flow control type based on the `CFG_UART_HW_FLOW_CTRL` or `CFG_UART_SW_FLOW_CTRL.`configuration macros. (valid for the UART interface only)
  - Serial port number assigned by the host PC.

- Press the reset button on the DA1469x daughterboard to start executing the application. 

- Send data/file through the serial console. Note that the data throughput should be decreased if data are transmitted by both ends simultaneously.  

- Optionally, open the SEGGER JLink RTT Viewer (one for each device connected) to get various debug messages along with throughput measurements. To do so, the `_SEGGER_RTT` symbol address should be retrieved from the generated `.map` file (of a given project) and be filled in the address field of the viewer as depicted below:  

  

  ![initiating_rtt_viewer](assets\initiating_rtt_viewer.png)

**NOTE: In case the USB CDC interface is selected and no retarget operations (RTT) are needed, the DA1469x devices can be powered directly from the USB port mounted on the daughterboard. In that case, the latter can be detached from the motherboard, completely (given that no other I/O pins are used).    ** 

## Known Limitations

- Under high baud rates (`CFG_UART_SPS_BAUDRATE`) (> 115200) some data loss might be observed when the UART serial interface is selected and the SW flow control is utilized. The larger the baud rate the more the data loss. 
- Right after the flow control activation certain number of on-the-fly packets should be transmitted. This number can vary from 5 to 30 depending on the serial interface speed. Such a condition should cause RX queue full assertions. It is suggested that either the RX queue size (`RX_SPS_QUEUE_SIZE`) is increased or the RX high water-mark level (`RX_QUEUE_HWM`) is reduced so data transmission is forbidden earlier. 
- A deadlock can occur if two DA1469x devices are employed running at the basic clock speed (`CUSTOM_SYS_CLK`), that is 32MHz, utilizing the UART interface with the flow control activated and with data being transmitted at both sides, simultaneously. 
- Heap overflow might be observed if the DA1469x devices run at the basic clock speed, that is 32MHz, and data packets are transmitted by the peer device (over the air) at high rates. If this is the case, either increase the OS heap space (`configTOTAL_HEAP_SIZE`) (in order for all of the dynamic memory operations to be serviced) or increase the CPU clock speed by leveraging PLL96MHz (`sysclk_PLL96`).


## License

**************************************************************************************

 Copyright (c) 2023 Dialog Semiconductor. All rights reserved.

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
