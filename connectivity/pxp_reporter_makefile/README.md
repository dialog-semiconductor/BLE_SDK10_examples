Proximity Reporter demo application {#pxp}
===================================

## Overview

This application is a sample implementation of Proximity Reporter of the Proximity Profile,
as defined by the Bluetooth Special Interest Group.

It supports the mandatory Link Loss Service and both optional Immediate Alert and Tx Power Services.

Features:

- Advertising is started automatically once application is started

- Alerting is done using white LED available on ProDK:

 * for medium alert level LED blinks at slow rate
 * for high alert level LED blinks at fast rate

- For LLS, alert is automatically disabled after 15 seconds of inactivity or
  upon reconnection of client which lost connection

- Tx Power Level is hardcoded to static value

- Device name can be read from flash

- Application supports software update over the air; for details look at [SUOTA in Proximity Reporter](@ref pxp_suota) document

## Installation procedure

The project is located in the \b `projects/dk_apps/demos/pxp_reporter` folder.

To install the project follow the [General Installation and Debugging Procedure](@ref install_and_debug_procedure).

## Suggested Configurable parameters

- Default device name
  In pxp_reporter_config.h, put the device name in PX_REPORTER_DEFAULT_NAME macro

- Advertising Interval
  In pxp_reporter_task.c, set the advertising interval (adv_intervals[]).

- Alert Actions (use the alert callbacks in pxp_reporter_task.c):
 * ias_alert_cb
 * lls_alert_cb

## PTS testing

Application can be readily used for executing PTS test cases against LLS, IAS and TPS.
It does not require any user intervention during testing.

## Manual testing

- build the demo for execution from flash
- download to flash and execute
- write device name to flash at address 0x80019 preceded by two bytes of its length (optional)
- scan for the device by name "Dialog PX Reporter" and connect
- write value to Alert Level characteristic in Immediate Alert service:

 * 0x00 (No alert) - the white LED on ProDK device does not blink
 * 0x01 (Mild alert) - the white LED on ProDK blinks slow immediately
 * 0x02 (High Alert) - the white LED on ProDK blinks fast immediately
 
- write value to Alert Level characteristic in Link Loss service:

 * 0x00 (No alert) - the white LED on ProDK device does not blink
 * 0x01 (Mild alert) - the white LED on ProDK blinks slow after link loss
 * 0x02 (High Alert) - the white LED on ProDK blinks fast after link loss
 * to trigger link loss event put Android/Apple device in RF Shield box or significantly increase
   the distance between the two devices

- read value of Tx Power Level characteristic in Tx Power service:
 * at each reading should be given the same value (0 dBm hardcoded in application)
