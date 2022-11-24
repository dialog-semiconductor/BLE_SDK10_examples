# Battery Charging

This example aims to provide an in-depth description of the battery
charging feature on the DA1469x Bluetooth SoC. The charger has a
Constant-Current Constant-Voltage architecture and built in temperature
protection. With optional JEITA compliant support for an external NTC.
All charger functions are implemented in a hardware state machine and
all essential measurements needed for the charger control (Vbat, battery
temperature) are implemented in hardware, without the need to use any of
the ADC channels and/or software. The USB of the DA1469x is also
involved. In order to be able to activate the charge over 90mA, USB
enumeration needs to be activated in order to stay compliant with the
USB specification.

## HW & SW Configurations

- **Hardware Configurations**
  - This example runs on a DA1469x Bluetooth Smart SoC.
  - A DA1469x Pro Development Kit is needed for this example.
- **Software Configurations**
  - Download the latest SDK version for the DA1469x family of devices (10.0.10.x).
  - **SEGGER's J-Link** tools should be downloaded and installed.

## How to run the example

### Initial Setup

- Download the source code from the Support Website
- Import the project into your workspace.
- Connect the target device to your host PC.
- Compile the code (either in Debug or Release mode) and load it into the chip.
- Open a serial terminal (115200/8 - N - 1)

Every 10 second you should expect to get the current battery voltage and charger status

```console

[Main task] Battery level 4106 mV,charger_state  HW_CHARGER_MAIN_FSM_STATE_POWER_UP
Pre-charging
Charging
[Main task] Battery level 4214 mV,charger_state  HW_CHARGER_MAIN_FSM_STATE_CV_CHARGE
...
[Main task] Battery level 4213 mV,charger_state  HW_CHARGER_MAIN_FSM_STATE_CV_CHARGE
Battery Charged
[Main task] Battery level 4164 mV,charger_state  HW_CHARGER_MAIN_FSM_STATE_END_OF_CHARGE
[Main task] Battery level 4159 mV,charger_state  HW_CHARGER_MAIN_FSM_STATE_END_OF_CHARGE

```

## Software implementation of charging

Adding charging to an existing project takes very little code in SDK 10,
just three code blocks in main.c and a header file with the charger
settings. The following code block must be added to the list of includes
in your main.c file.

``` c
#if (dg_configUSE_SYS_USB == 1)
#include "sys_usb.h"
#endif /* (dg_configUSE_SYS_USB == 1) */
#if (dg_configUSE_SYS_CHARGER == 1)
#include "sys_charger.h"
#include "custom_charging_profile.h"
#endif /* dg_configUSE_SYS_CHARGER */
```

To start the charger, the next code block can be added. It is suggested
this is added to the system_init, directly above the first task
creation.

``` c
#if (dg_configUSE_SYS_CHARGER == 1)
        sys_usb_init();
        sys_charger_init(&sys_charger_conf);
#endif /* dg_configUSE_SYS_CHARGER */
```

The charger also uses the USB for charger detection, so the pins should
be set up. The following code block should be in the periph_init
function.

``` c
#if (dg_configUSE_SYS_CHARGER == 1)
        /* USB data pin configuration */
        hw_gpio_set_pin_function(HW_GPIO_PORT_0, HW_GPIO_PIN_14, HW_GPIO_MODE_INPUT,
                                 HW_GPIO_FUNC_USB);
        hw_gpio_set_pin_function(HW_GPIO_PORT_0, HW_GPIO_PIN_15, HW_GPIO_MODE_INPUT,
                                 HW_GPIO_FUNC_USB);
#endif /* dg_configUSE_SYS_CHARGER */
```

When all code blocks are in place, the *dg_configUSE_SYS_CHARGER* and
*dg_configUSE_SYS_USB* flag can be set in *custom_config_qspi.h* and
*custom_config_ram.h*.

As for *custom_charging_profile.h*, this file is present in a few
examples, like pxp_reporter. The best course of action would be copying
it over. The file only contains one structure declaration, which
contains all of the settings for the charger hardware. In the next
chapter the settings in this file will be described.

## Charger settings

The struct in *custom_charging_profile.h* contains the settings to tweak
the charger to your application. The file from pxp_reporter already has
good baseline values.

``` c
typedef struct {
     HW_CHARGER_CTRL                 ctrl_flags;                     /**< Control flags for influencing charging profile. */

     HW_CHARGER_TBAT_MONITOR_MODE    tbat_monitor_mode;              /**< Monitor mode for battery temperature. */

     /* IRQ parameters */

     HW_CHARGER_FSM_IRQ_OK           irq_ok_mask;                    /**< IRQ's the charging profile is interested in (non error cases). */
     HW_CHARGER_FSM_IRQ_NOK          irq_nok_mask;                   /**< IRQ's the charging profile is interested in (error cases). */

     /* Voltage parameters */

     HW_CHARGER_V_LEVEL              ovp_level;                      /**< Over voltage level. */
     HW_CHARGER_V_LEVEL              replenish_v_level;              /**< Replenish voltage level. */
     HW_CHARGER_V_LEVEL              precharged_v_thr;               /**< Pre-charged voltage threshold. */
     HW_CHARGER_V_LEVEL              cv_level;                       /**< Constant Voltage level. */

     HW_CHARGER_V_LEVEL              jeita_ovp_warm_level;           /**< Over voltage level set by JEITA FSM if Tbat found in warm zone. */
     HW_CHARGER_V_LEVEL              jeita_ovp_cool_level;           /**< Over voltage level set by JEITA FSM if Tbat found in cool zone. */
     HW_CHARGER_V_LEVEL              jeita_replenish_v_warm_level;   /**< Replenish voltage level set by JEITA FSM if Tbat found in warm zone. */
     HW_CHARGER_V_LEVEL              jeita_replenish_v_cool_level;   /**< Replenish voltage level set by JEITA FSM if Tbat found in cool zone. */
     HW_CHARGER_V_LEVEL              jeita_precharged_v_warm_thr;    /**< Pre-charged voltage threshold set by JEITA FSM if Tbat found in warm zone. */
     HW_CHARGER_V_LEVEL              jeita_precharged_v_cool_thr;    /**< Pre-charged voltage threshold set by JEITA FSM if Tbat found in cool zone. */
     HW_CHARGER_V_LEVEL              jeita_cv_warm_level;            /**< Constant Voltage level set by JEITA FSM if Tbat found in warm zone. */
     HW_CHARGER_V_LEVEL              jeita_cv_cool_level;            /**< Constant Voltage level set by JEITA FSM if Tbat found in cool zone. */

     /* Current parameters */

     HW_CHARGER_I_EOC_PERCENT_LEVEL  eoc_i_thr;                      /**< End of charge current threshold. */
     HW_CHARGER_I_PRECHARGE_LEVEL    precharge_cc_level;             /**< Pre-charged constant current level. */
     HW_CHARGER_I_LEVEL              cc_level;                       /**< Constant Current level. */

     HW_CHARGER_I_PRECHARGE_LEVEL    jeita_precharge_cc_warm_level;  /**< Pre-charged constant current level set by JEITA FSM if Tbat found in warm zone. */
     HW_CHARGER_I_PRECHARGE_LEVEL    jeita_precharge_cc_cool_level;  /**< Pre-charged constant current level set by JEITA FSM if Tbat found in cool zone. */
     HW_CHARGER_I_LEVEL              jeita_cc_warm_level;            /**< Constant Current level set by JEITA FSM if Tbat found in warm zone. */
     HW_CHARGER_I_LEVEL              jeita_cc_cool_level;            /**< Constant Current level set by JEITA FSM if Tbat found in cool zone. */

     /* Temperature parameters */

     HW_CHARGER_DIE_TEMP_LIMIT       die_temp_limit;                 /**< Die's temperature limit. */

     HW_CHARGER_BAT_TEMP_LIMIT       bat_temp_hot_limit;             /**< Tbat hot limit.  */
     HW_CHARGER_BAT_TEMP_LIMIT       bat_temp_warm_limit;            /**< Tbat warm limit. */
     HW_CHARGER_BAT_TEMP_LIMIT       bat_temp_cool_limit;            /**< Tbat cool limit. */
     HW_CHARGER_BAT_TEMP_LIMIT       bat_temp_cold_limit;            /**< Tbat cold limit. */

     /* Charging timeout parameters */

     uint16_t                        max_precharge_timeout;          /**< Timeout for Pre-charge state (in secs). */
     uint16_t                        max_cc_charge_timeout;          /**< Timeout for Constant Current state (in secs). */
     uint16_t                        max_cv_charge_timeout;          /**< Timeout for Constant Voltage state (in secs). */
     uint16_t                        max_total_charge_timeout;       /**< Timeout for total charging (in secs). */
} hw_charger_charging_profile_t;
```

All of these types are defined and described in *hw_charger_da1469x.h*,
please refer to this file for information on the possible values for the
variables.

Refer to the datasheet of the used battery for recommended settings.


## Known Limitations

There are no known limitations for this demonstration example.
