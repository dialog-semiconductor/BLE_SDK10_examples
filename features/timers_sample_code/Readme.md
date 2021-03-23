# Input Capture & PWM Timer Functionality

This application demonstrates using TIMER1. The DA1469x family of devices incorporates four identical HW timer blocks. In this demonstration, TIMER1 (HW_TIMER) is configured to Input Capture mode and PWM functionality is enabled, as well. To facilitate the developer to confirm the correct behavior of the code the **K1** push button on Pro DevKit has been selected as the input capture source of TIMER1. The application measures the time duration the button is kept pressed and based on that time, PMW functionality is changed accordingly. By default, the PWM functionality is mapped on RED LED1 on DA1469x daughterboard. The developer is free to customize the application using the appropriate macros as provided in `timer_functionality.h` file.

### HW & SW Configurations

- **Hardware Configurations**
  - This example runs on a DA1469x Bluetooth Smart SoC.
  - A DA1469x Pro Development Kit is needed for this example.
- **Software Configurations**
  - Download the latest SDK version for the DA1469x family of devices (10.0.10)
  - **SEGGER's J-Link** tools should be downloaded and installed.

## How to run the example

### Initial Setup

- Download the source code from the Support Website
- Import the project into your workspace.
- Connect the target device to your host PC.
- Compile the code (either in Release or Debug mode) and load it into the chip.
- Open a serial terminal (115200/8 - N - 1)
- Press the reset button on DA1469x daughterboard to start executing the application.

Press and then release the **K1** push button on Pro DevKit. Depending on the time duration the button is kept pressed, the PWM duty cycle that drives LED1 on Pro DevKit should change according to the following time intervals:
`Short press < 0.5 seconds (~3%)`
`Medium press >= 0.5 seconds and < 3 seconds (25%)`
`Long press >= 3 seconds (100%)`

![TIMER Serial Console](assets/timer1_serial_console.png)

You may observe more than one triggers when the K1 button is pressed. This is because the mechanical parts usually generate spikes that can trigger fake capture events.

&#160;

**Note 1:** The Power Domain Controller (PDC) cannot be triggered by timer capture events and thus, waking up the ARM M33 core while in sleep mode. Only TIMERx overflow events can trigger the PDC to wake up the M33 core. This is why the selected sleep mode is set to active mode.

**Note 2** Only TIMER1 (HW_TIMER) can trigger input capture IRQs.

## Known Limitations
