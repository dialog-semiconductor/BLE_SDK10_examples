# Example demonstrating the use of UART Adapter abstraction layer

## Overview

This application demonstrates the use of all three UARTs through the Adapters abstraction layer.
The name of the project is **UART_Adapter_example**.

For all three UARTs available on DA1469x, for easy testing is implemented the echo-back functionality, which sends back to PC every character received by a UART.

For **UART1** the echo-back is implemented without flow control since the UART1 does not support CTS/RTS functionality. For the UART1 the echo implementation is using the blocking calls if the UART adater API.

A single task is used for the UART1 echo-back implementation. The task initiates a UART read for a single character which is transmitted back once received.

For **UART2** the echo-back is using HW flow control with CTS/RTS. The implementation of the echo-back is using two tasks, one for receiving a character from PC and one for transmitting a character to the UART. Also the implementation for the UART2 uses an OS Queue for passing the received characters to TX task. An OS Mutex is used for opening the UART2 and the async calls of UART adapter API.

For **UART3** the echo-back is implemented in the same way as the UART1 but is using HW flow control with CTS/RTS.

All the code is in the **main.c** file.
The function **system_init()** creates and initializes all the tasks needed, the OS Queue and the MUTEX .

The configurations of the UARTs is in the **platform_devices.c** file. There is nothing to be added in the **periph_init()** function.

**Note** 
The example can be downloaded from [Here](http://lpccs-docs.dialog-semiconductor.com/SW_Example/SDK10/DA1469x_UART_Adapter_Example.zip).


## HW and SW configuration

- **Hardware configuration**

  - This example runs on The DA14695 Bluetooth Smart SoC devices.
  - The Pro Development kit is needed for this example.
  - Connect the Development kit to the host computer.

- **Software configuration**

  - This example requires:

  - Smartsnippets Studio 2.0.16 or newer
  - SDK10.0.10.118

  - **SEGGER J-Link** SEGGER tools are normally downloaded and installed as part of the Smart Snippets Studio installation.

## How to run the example

### Pin Assignments

The pins configured in this example for each UART are:

**<u>UART-1:</u>**
RX: P0_8
TX: P0_9

**<u>UART-2:</u>**
    RX: P1_8
    TX: P1_9
    RTSn: P1_6
    CTSn: P1_7

**<u>UART-3:</u>**
    RX: P0_26
    TX: P0_27
    RTSn: P0_28
    CTSn: P0_29


### Running and testing the application

To run and test the application on a ProDK is needed the following HW and SW, except of the SamrtSnippetsStudio (SSS) and the ProDK.

**<u>HW:</u>** Need to have two USB to 3.3V TTL USB dongles with CTS/RTS support.

SW: use the Terminal within the SSS or have a terminal application like Putty installed on the PC.

**<u>Testing UART1:</u>**
Plug in the ProDK to a USB port of the PC. There will be two COM ports added. Check the Device manager to find out which ones. Of the two COM ports first UART one is the one mapped to UART1 of the DA1469x.

Start the terminal application on the PC (e.g. Putty on Windows or equivalent on other OSs) and configure it to use the COM port as mentioned above. Configure the COM port parameters as follows:

| Parameter    | Value  |
| ------------ | ------ |
| Baud Rate    | 115200 |
| Data Bits    | 8      |
| Stop Bits    | 1      |
| Parity       | None   |
| Flow Control | None   |

Do not enable any echo back feature in the terminal app. Start typing in the terminal window.  Every     character typed goes to DA1469x UART-1 and returns back so it will appear in the terminal window.

If the user click 'ESC' (ASCII=27), then the task implementing the echo on  the UART-1 will terminate and the operation will seize on this UART.

**<u>Testing UART2:</u>**

Connect an external USB to TTL UART (e.g. an FTDI USB to TTL module) to ProDK UART-2 pins as follows:

| ext UART Pin | UART-2 pin |
| ------------ | :--------- |
| RX           | TX         |
| TX           | RX         |
| CTS          | RTS        |
| RTS          | CTS        |

Plug in the USB-to-TTL dongle to a USB port of the PC. There will be a new COM port available in the system. Check the device manager to identify which one it is.

Start the terminal application on the PC (e.g. Putty on Windows or equivalent on other OSs) and configure it to use the COM port as mentioned above. Configure the COM port parameters as follows:

| Parameter    | Value                                   |
| ------------ | --------------------------------------- |
| Baud Rate    | 115200                                  |
| Data Bits    | 8                                       |
| Stop Bits    | 1                                       |
| Parity       | None                                    |
| Flow Control | RTS/CTS (or might be called 'hardware') |

Do not enable any echo back feature in the terminal app. Start typing in the terminal window.  Every     character typed goes to DA1469x UART-2 and returns back so it will appear in the terminal window.

If the user click 'ESC' (ASCII=27), then the task implementing the echo on  the UART-2 will terminate and the operation will seize on this UART.

If the user will disconnect any of the CTS/RTS connections and keep typing, the characters will not come back to terminal application until the connection on CTS/RTS is restored. For this case there can be a buffering up to 100 characters in the DA1469x for the UART-2 plus the UART HW FIFO.

It depends to whether the user disconnects the CTS or the RTS line of the DA1469x the exact behavior as in one case the characters will not be transmitted at all to DA1469x while in the other case the characters will be received by the DA1469x but will not be transmitted back to PC.

**Testing UART3:**

Connect an external USB to TTL UART (e.g. an FTDI USB to TTL module) to ProDK UART-3 pins as follows:

| ext UART Pin | UART-3 pin |
| ------------ | :--------- |
| RX           | TX         |
| TX           | RX         |
| CTS          | RTS        |
| RTS          | CTS        |

Plug in the USB-to-TTL dongle to a USB port of the PC. There will be a new COM port available in the system. Check the device manager to identify which one it is.

Start the terminal application on the PC (e.g. Putty on Windows or equivalent on other OSs) and configure it to use the COM port as mentioned above. Configure the COM port parameters as follows:

| Parameter    | Value                                   |
| ------------ | --------------------------------------- |
| Baud Rate    | 115200                                  |
| Data Bits    | 8                                       |
| Stop Bits    | 1                                       |
| Parity       | None                                    |
| Flow Control | RTS/CTS (or might be called 'hardware') |

Do not enable any echo back feature in the terminal app. Start typing in the terminal window.  Every     character typed goes to DA1469x UART-3 and returns back so it will appear in the terminal window.

If the user click 'ESC' (ASCII=27), then the task implementing the echo on  the UART-3 will terminate and the operation will seize on this UART.

If the user will disconnect any of the CTS/RTS connections and keep typing, the characters will not come back to terminal application until the connection on CTS/RTS is restored.

It depends to whether the user disconnects the CTS or the RTS line of the DA1469x the exact behavior as in one case the characters will not be transmitted at all to DA1469x while in the other case the characters will be received by the DA1469x but will not be transmitted back to PC.
## Known Limitations
There are no known limitations for this application.
