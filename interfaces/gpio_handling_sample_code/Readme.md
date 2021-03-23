# GPIO Pins Handling

The GPIO peripheral block is powered by the COM power domain (PD_COM) and thus, it should be enabled when performing GPIO related operations. However, this power domain is not always enabled during ARM M33 run time and hence, the application is responsible for activating/deactivating it.
GPIO Pins must be set to latch disabled state before the ARM M33 enters sleep and activated at M33 wakeup. Please note that the developer does not have to control the GPIO pins used by adapters (e.g. I2C pins). This example demonstrates two possible mechanisms for handling GPIO pins that are not handled by adapters. User can switch between the two mechanisms using the `GPIO_HANDLING_DYNAMICALLY` macro in `main.c` file.

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

LED1 (red color) on Pro DevKit should start blinking every 1 second.

## Known Limitations

There are no known limitations for this sample code.
