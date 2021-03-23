# Cycling Power collector demo application

## Example description

This application is a sample implementation of Cycling Power collector as defined by CPP specification
1.1. It supports the mandatory Cycling Power Profile Collector features only.

The application is controlled using command line interface exposed over UART.

The application supports up to 8 simultaneous connections.

## HW and SW configuration

- **Hardware configuration**

  - This example runs on The DA1469x Bluetooth Smart SoC devices.
  - The Pro Development kit is needed for this example.
  - Connect the Pro Development kit to the host computer with default jumper settings.

- **Software configuration**

  - This example requires:

  * Smartsnippets Studio 2.0.16.
  * SDK10.0.10

  - **SEGGER's J-Link** tools should be downloaded and installed.

## How to run the example

### Initial Setup

- Compile the QSPI version
- Download the binary to flash and execute

The application is controlled using command line interface (CLI) which can be accessed using serial
port available when platform is connected to PC using USB2 connector (e.g. \b `/dev/ttyUSBx` or \b `COMx`).

### Quick start

To quickly scan and connect to sensor device:

1. Type <b>`scan start`</b> to start scanning for CPP Sensor devices.
1. When device is found, type <b>`scan stop`</b> to stop scanning.

```
Scanning...
[01] Device found: public  80:EA:CA:80:88:88
[01] Device found: public  80:EA:CA:80:88:88 (Dialog CP Sensor)
[02] Device found: private 5E:12:81:51:A6:9B (Cycling Power)
[02] Identity Address: public C4:61:8B:80:2B:5E
Scan stopping...
Scan stopped

```

3. Now you can connect to a device by using one of following methods:

- type <b>`connect 1`</b> to connect to 1st device found
- type <b>`connect 80:EA:CA:80:88:88`</b> to connect to device with given address

4. The connection is established and device is automatically discovered:

   \\note
   The collector stores services of bonded sensors. Any subsequent connection to already bonded
   sensor will not generate additional GATT traffic. The sensor information is to be read from storage.
   Application verifies bond status on reconnection, thus any lose of encryption keys information
   will trigger pairing procedure and re-discovery of all services.

```
Device connected
        Connection index: 0
        Address: private 73:62:DF:9C:3B:F5
Passkey notification
        Connection index: 0
        Passkey: 164514
Security level changed
        Connection index: 0
        Security level: 4
Browsing...
Pair completed
        Connection index: 0
        Status: 0x00
        Bond: true
        MITM: true
Browse completed
        Connection index: 0
        Cycling Power service found
        GATT Service found
Set Cycling Power Measurement Notifications completed
        Status: 0x00
        Connection index: 0
Set Cycling Power Vector Notifications completed
        Status: 0x00
        Connection index: 0
Set Control Point indications completed
        Status: 0x00
        Connection index: 0
Ready.
```

As can be seen above, discovery includes following actions:
\- search for Cycling Power Service and read available characteristics,
\- enable notifications for measurement characteristic.
5\. Initialization is finished when <b>`Ready.`</b> message is received, as above. You should see
incoming measurement notifications as below.

```
CP Measurement notification received
        Instantaneous power: 15276 [W]
        Crank revolution cumulative value 0x4eeb
        Crank revolution last event time 0xdb61 [1/1024 s]
        Maximal torque magnitude: -923.156 [Nm]
        Minimal torque magnitude: -104.218 [Nm]
        Maximal angle: 3553 [deg]
        Minimal angle: 3555 [deg]
        Top dead spot angle: 15841 [deg]
        Bottom dead spot angle: 48350 [deg]
```

6. To use full functionality of the application, check the complete list of commands below.

### Available commands

#### `scan <start|stop> [any]`

Start and stop scanning procedure.

By default, the application scans for devices which include Heart Rate service UUID (0x180D) in
advertising data. To scan for any device in range use optional <b>`any`</b> parameter, i.e.
<b>`scan start any`</b>.

The returned list of devices includes an index, a device address and a device name (if available).

\\note
The application can cache up to 25 devices. When this limit is reached, application stops filtering
duplicated devices and any new devices found are reported with index `00`.

#### `connect <address [public|private] | index>`

Connect to peripheral device.

The device can be specified by either providing an address or an index of device found during last
scan session.

The optional address type applies only when connecting to a device using an address. If not specified,
the address type is searched on results from previous scan session or <b>`public`</b> is used when not
available.

#### `connect cancel`

Cancel ongoing connection attempt.

```
Connection completed
        Status: 0x0c
```

#### `cp_notif <conn_idx> <on|off>`

Enable and disable Cycling Power Measurement notifications on device with specified `<conn_idx>`.

This command is provided for debugging purposes. Notifications are always enabled after sensor is
connected and there is no need to enable them explicitly.

#### `disconnect [<conn_idx>]`

Disconnect sensor with specified `<conn_idx>`. If `<conn_idx>` is not specified
disconnect with first sensor on list of connected devices

##### note
To see that list use command: `show connected`.

```
Disconnected from C3:26:2B:63:B1:11
Device disconnected
        Connection index: 0
        Reason: 0x16
```

#### `update_sensor_loc <conn_idx> <loc>`

Update sensor location with given location for specified conn_idx.

#### `get_sup_sensor_loc <conn_idx>`

Get supported sensor locations for given conn_idx.

#### `set <conn_idx> <param> <value>`

Set specified parameter for given conn_idx. Available parameters:

- Crank length

- Chain length

- Chain weight

- Span length

#### `unbond [[public|private] <address> | all]`

Unbond sensor with specified address or unbond all bonded sensors

\\note
Write `show bonded` to see list of bonded sensors to unbond
