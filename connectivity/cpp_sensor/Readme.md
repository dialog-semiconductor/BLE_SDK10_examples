# Cycling Power Sensor demo application

## Example description

This application is a sample implementation of Cycling Power Sensor that is defined
by Cycling Power Profile (https://www.bluetooth.com/specifications/adopted-specifications).

It supports the mandatory Cycling Power Service.

Features:

- Advertising is started automatically once application is started

- Cycling Power Measurements are sent (about every second)

- One client can be connected at the time

- Demo supports Sensor Measurement Context (Force  or Torque based measurements), Instantaneous
  Power Measurement.

## HW and SW configuration

- **Hardware configuration**

  - This example runs on The DA1469x Bluetooth Smart SoC devices.
  - The Pro Development kit is needed for this example.
  - Connect the Pro Development kit to the host computer with default jumper settings.

- **Software configuration**

  - This example requires:

  * Smartsnippets Studio 2.0.8.
  * SDK10.0.4

  - **SEGGER's J-Link** tools should be downloaded and installed.

## Installation procedure

- Build the demo for execution from flash

- Download the binary to flash and execute

- Scan for the device by name "Black Orca CP Sensor" and connect

- Register to receive notifications from Cycling Power Measurement characteristic in Cycling Power service

  - New cycling power measurement should be received about every second:

  * values that are generated randomly are as follows:
    - Instantaneous Power

- Read the CP Feature characteristic in Cycling Power service:

  - CP Feature is set to support only mandatory features

- Read the Sensor Location characteristic in Cycling Power service:

  - Sensor location should be set to 'Other' by default

## PTS testing

- The application can be readily used for executing PTS.
