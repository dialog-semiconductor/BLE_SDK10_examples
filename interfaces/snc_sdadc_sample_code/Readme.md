# VBAT Monitoring with Sensor Node Controller

This sample code aims at demonstrating the use of the Sensor Node Controller (SNC) to operate the Sigma-Delta ADC (SDADC) to measure the battery voltage (VBAT).
It adds an SNC driver not present in the current SDK release for this purpose.

- The SNC is configured by the SNC adapter to measure the battery voltage
- The sampling trigger event is the RTC, at an interval of 1 second
- The actual measurement is performed by SNC using the SDADC
- If the example were merged into a BLE example, it would use the system configured RTC interval (for RFTEMP monitoring)
- The VBAT samples are collected into a queue
- The host is notified to display the queue contents, after collection of each batch of 4 samples

### HW & SW Configurations

- **Hardware Configurations**
  - This example runs on a DA1469x Bluetooth Smart SoC.
  - A DA1469x Pro Development Kit is needed for this example.
- **Software Configurations**
  - Download the latest SDK version for the DA1469x family of devices (10.0.10.x)
  - **SEGGER's J-Link** tools should be downloaded and installed.

## How to run the example

### Initial Setup

- Download the source code from the Support Website
- Import the project into your workspace.
- Connect the target device to your host PC.
- Compile the code (either in Release or Debug mode) and load it into the chip.
- Open a serial terminal (115200/8 - N - 1)
- Press the reset button on DA1469x daughterboard to start executing the application.
- VBAT Readings will appear, every four seconds

## Known Limitations

There are no known limitations for this sample code.

