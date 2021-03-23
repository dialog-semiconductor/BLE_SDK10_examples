# WeChat Profile application

\===================================

## Example description

This application is a sample implementation of WeChat Profile as is defined by (http://iot.weixin.qq.com/wiki/new/index.html).

It supports WeChat BLE profile with two data encryption types: md5AndNoEnrypt and md5AndAesEnrypt.
HW crypto engine is used for both MD5 and AES operation.

More info can be found in UM-B-058 WeChat IoT Software Development Kit v1.0.

Features:

- Link creation between WeChat public account and WeChat BLE device

- Communication between WeChat public account and WeChat BLE device

* For the handling of the commands from the smartphone wechat side, using mpbledemo2_cmd_handler.
* In order to send message to the smartphone wechat side, using mpbledemo2_send_string.

## HW and SW configuration

- **Hardware configuration**

  - This example runs on The DA1469x Bluetooth Smart SoC devices.
  - The Pro Development kit is needed for this example.
  - Connect the Pro Development kit to the host computer with default jumper settings.

- **Software configuration**

  - This example requires:

  * Smartsnippets Studio 2.0.8.
  * SDK10.0.4

  - **SEGGER J-Link** tools should be downloaded and installed.

## How to run the example

### Initial Setup

- Install AirSyncDebugger debug application tool to the android smart phone supporting BLE
  protocol (It can be downloaded from http://iot.weixin.qq.com/)

- In order to test md5AndNoEnrypt mode change the encryption type config in mpbledemo2.h as following:

  \#define EAM_md5AndNoEnrypt 1
  // #define EAM_md5AndAesEnrypt 1

- Build the demo for execution from flash or RAM

- Download to flash and execute it on the devkit board

- Open AirSyncDebugger on the smartphone, click to enter the scanned BLE device

- Choose AirSync as the test protocol

- Click "Setting" on the top-right of the view, set up Device ID, Device Type as the
  value DEVICE_ID and DEVICE_TYPE defined in mpbledemo2.h

- Click "Automatic Test"

- When the test runs into the fifth step "Device sends data", press the K1 button

- Totally six test step, when the test is passed, it means the firmware has fulfilled all
  the requirement needed for the communication with WeChat public account

## Known Limitations

- There are No known limitations for this example. But you can check and refer to the following application note for
  [known hardware limitations](https://www.dialog-semiconductor.com/bluetooth-low-energy "known hardware limitations").
- Dialog Software \[Forum link\](https://support.dialog-semiconductor.com/forums/dialog-smartbond-bluetooth-low-energy-%E2%80%93-software"Forum link").
