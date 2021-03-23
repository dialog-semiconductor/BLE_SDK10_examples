/*
 * @file user_power_measurement_configurations.h
 *
 * @brief Header file that contains all the parameters relevant to power consumption and
 *        operation modes
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 * This software ("Software") is owned by Dialog Semiconductor. By using this Software
 * you agree that Dialog Semiconductor retains all intellectual property and proprietary
 * rights in and to this Software and any use, reproduction, disclosure or distribution
 * of the Software without express written permission or a license agreement from Dialog
 * Semiconductor is strictly prohibited. This Software is solely for use on or in
 * conjunction with Dialog Semiconductor products.
 *
 * EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR AS
 * REQUIRED BY LAW, THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. EXCEPT AS OTHERWISE PROVIDED
 * IN A LICENSE AGREEMENT BETWEEN THE PARTIES OR BY LAW, IN NO EVENT SHALL DIALOG
 * SEMICONDUCTOR BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE SOFTWARE.
 *
 ****************************************************************************************
 */


#ifndef CONFIG_CUSTOM_CONFIG_POWER_CONSUMPTION_H_
#define CONFIG_CUSTOM_CONFIG_POWER_CONSUMPTION_H_


#include "bsp_definitions.h"

/*=================================================AdvertisementParameters=================================================*/


/* BLE peripheral advertising data - Sequence of single characters encased in ' ' separated by comma. Maximum 26 characters*/
#define USER_ADV_DATA_CHARS                    'T','e','s','t',' ','C','u','s','t','o','m',' ','S','e','r','v','i','c','e',' ','C','h','.',' ','1','A'

#define USER_ADV_SIZE                           sizeof((uint8_t[]){USER_ADV_DATA_CHARS})+1

/* Advertisement Interval measured in milliseconds */
#define USER_MIN_ADV_INTV_MS                    (90)
#define USER_MAX_ADV_INTV_MS                    (100)

/* Advertisement Type
 * GAP_CONN_MODE_NON_CONN,         ///< Non-connectable mode
 * GAP_CONN_MODE_UNDIRECTED,       ///< Undirected mode
 * GAP_CONN_MODE_DIRECTED,         ///< Directed mode
 * GAP_CONN_MODE_DIRECTED_LDC,     ///< Directed Low Duty Cycle mode
 */

#define USER_ADV_TYPE                           (GAP_CONN_MODE_UNDIRECTED)

/*==================================================Connection Parameters==================================================*/

/* Payload in Octets maximum 512 */
#define USER_MTU                                (65)

/*
 * Macro used for setting the maximum length, expressed in bytes,
 * of Characteristic Attributes. The current Maximum allowed by ATT protocol is 512 Bytes.
 *
 * The remote device must not exceed the max value when updating the
 * Characteristic Attribute value. Otherwise, the system might crush.
 *
 **/
#define USER_CHARACTERISTIC_VALUE_SIZE          (20)

/*=====================================================Low Power Clock=====================================================*/

/* Symbols Definition */
#define USER_LP_CLK_EXTERNAL_XTAL               (0x01)
#define USER_LP_CLK_XTAL32K                     (0x02)
#define USER_LP_CLK_RCX                         (0x03)

/* Low PowerClock Parameters
 * USER_LP_CLK_EXTERNAL_XTAL            - External 32K Clock (Accurate crystal but needs to wait to settle - Provided by the client)
 * USER_LP_CLK_XTAL32M                  - External XTAL32k Clock (Accurate crystal but needs to wait to settle)
 * USER_LP_CLK_RCX                      - Internal RC Clock (Limited Accuracy, requires trimming but does not need to wait)
 *
 */
#define USER_LP_CLOCK                           USER_LP_CLK_XTAL32K


/*===============================================XTAL32M Settle Time Feature===============================================*/

/* Clock Re-stabilization Mode
 * pm_sys_wakeup_mode_slow              - No power is provided to the crystals during sleep mode so they settle from scratch.
 *                                        This mode consumes no extra power during sleep mode(recommended when sleep duration is
 *                                        longer than 8s). Spends some extra cycles to make sure that the power rails are ready.
 *
 * pm_sys_wakeup_mode_fast              - DEFAULT: Does not check for power rails status and takes for granted that they are active.
 *                                        This mode consumes no extra power during sleep (recommended for the average usage. Sleep
 *                                        duration from 100ms up to 8s)
 *
 * pm_sys_wakeup_mode_ultra_fast        - Power is provided to the crystals during sleep mode and requires 0.9V on sleep instead of 0.75V
 *                                        They settle very fast.This mode consumes a significant amount of extra power during sleep
 *                                        (recommended for sleep durations shorter than 100ms)
 */
#define USER_SYS_WAKEUP_MODE                    pm_sys_wakeup_mode_fast

/*========================================================SleepMode========================================================*/

/* Symbols Definition based on enum sleep_mode_t (Do not alter their value)*/
#define ACTIVE_MODE                             (0)     // Always active
#define IDLE_MODE                               (1)     // WFI (Everything is working except the processor - e.g. a DMA transaction is taking place but M33 is in SLEEP)
#define EXTENDED_SLEEP_MODE                     (2)     // Extended Sleep (Retained Memory and several peripherals active)
#define DEEP_SLEEP_MODE                         (3)     // Deep Sleep ( LP clock + Interrupt can wake it up. Starts as if cold reset.)
#define HIBERNATION_MODE                        (4)     // Hibernation( Wakes up only from interrupt - Clockless mode. Starts from cold reset.)

/*
 * User defined sleep mode (pick the corresponding value from the above).
 */
#define USER_SLEEP_MODE                         EXTENDED_SLEEP_MODE


/*==========================================================FLASH==========================================================*/

/* Flash Powered on Sleep ON/OFF
 *      FLASH_POWERED_DOWN              - Flash is switched to low power mode during sleep
 *      FLASH_POWERED_UP                - Flash stays on during sleep
 *
 *      This should be set to FLASH_POWERED_DOWN unless the user has a specific purpose for which Flash needs to be powered
 *      on or off when the system goes to sleep.( The events that require the system to wakeup after a very long time might benefit
 *      from powering the Flash off but in general the default should be FLASH_POWERED_DOWN).
 */
#define USER_FLASH_POWERED_DOWN

/*=======================================================COMPILATION=======================================================*/

/* Compilation Mode
 *      (0)     DEVELOPMENT_MODE        - Various debugging options are included.
 *      (1)     PRODUCTION_MODE         - All code used for debugging is removed. Recommended
 */
#define USER_PRODUCTION_MODE_EN                 (0)

/*========================================================SUP_LOGS=========================================================*/

/*
 * Suppress Messages Mode : Disabled (0), Enabled(1)
 */
#define USER_SUPPRESS_LOGS_EN                   (1)

/*===================================================AUTO_UPD_AND_NOTIFY===================================================*/

#define USER_AUTO_UPDATE_AND_NOTIFY_EN          (0)

/*
 * Value auto update interval in ms.
 */
#define USER_VALUE_AUTO_UPDATE_INTV             (2000)

/*===========================================================END===========================================================*/


#endif /* CONFIG_CUSTOM_CONFIG_POWER_CONSUMPTION_H_ */
