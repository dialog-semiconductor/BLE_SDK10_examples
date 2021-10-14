# DA1469x CMSIS NN example

______________________________________________________________________

## Example description

This example runs the ARM CMSIS NN library on the DA1469x. Four of the Tests
from the CMSIS library have been extracted and used as a sanity check. This
demonstrates the right operation of the neural network libraries on the DA1469x platform.

## HW and SW configuration

- **Hardware configuration**

  - This example runs on The DA1469x Bluetooth Smart SoC devices.
  - The Pro Development kit is needed for this example.
  - Connect the Development kit to the host computer.

- **Software configuration**

  - This example requires:

  * Smartsnippets Studio 2.0.16 or newer.
  * SDK10.0.10.x

  - **SEGGER J-Link** tools should be downloaded and installed.

## How to run the example

### Initial Setup

- Compile and launch RAM target

- Use your favorite terminal to the serial port (FTDI lower index port) with the following parameters

  - baudrate: 115200
  - data: 8 bits
  - stop: 1 bit
  - parity: None
  - flow  control: none

- The terminal displays the sin_cos example has run successfully.

```
arm_fully_connected_s16 test success
arm_fully_connected_s16_big test success
int16xint8_arm_convolve_fast_s16 test success
requantize_s64_arm_convolve_fast_s16 test success
```

## Known Limitations

- There are No known limitations for this example.
