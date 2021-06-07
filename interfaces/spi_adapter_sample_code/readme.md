# SPI Adapter Sample Code 

## Example description

This application demonstrates using the SPI adapter concept. The sample code makes use of an SPI-compatible module, that is the MCP4822 DAC (digital-to-analog converter). A digital multimeter is used to verify the analog output generated.

### HW & SW Configurations

- **Hardware Configurations**
    - This example runs on a DA1469x Bluetooth Smart SoC.
    - A DA1469x Pro DevKit is needed for this example.
- **Software Configurations**
    - Download the latest SDK version (10.0.10.x)
    - **SEGGER's J-Link** tools should be downloaded and installed.



## How to run the example

### Initial Setup

- Download the source code from the Support Website.

- Import the project into your workspace.

- Connect the target device to your host PC.

- Connect the DAC module to the Pro DevKit as illustrated below.

  ![MCP4822 Module Setup](assets\mcp4822_module_setup.png)

- Compile the code and load it into the chip.

- Open a serial terminal (115200/8 - N - 1)

- Press the reset button on DevKit to start executing the application.

- Press the **K1** button on the Pro DevKit.  A debugging message is displayed on the console indicating the status of the current SPI operation as well as the data (2 bytes) sent to the DAC module. The analog output value of the selected DAC channel, by default channel B, should be changed randomly.

![Debug Console](assets\debug_console.PNG)


## Known Limitations
There are no known limitations for this application.


## License
**************************************************************************************

 Copyright (c) 2021 Dialog Semiconductor. All rights reserved.

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