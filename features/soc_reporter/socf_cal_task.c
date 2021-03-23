/**
 ****************************************************************************************
 *
 * @file soc_cal_task.c
 *
 * @brief SOC Calibration application implementation
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include "osal.h"
#include "hw_cpm.h"
#include "app_nvparam.h"
#include "ad_nvparam.h"
#include "ad_sdadc.h"
#include "ad_gpadc.h"
#include "cli.h"
#include "platform_devices.h"
#include "socf_config.h"
#include "socf_hal.h"

#if defined(SOCF_PERFORMANCE_TEST)
/**
 * CLI notification mask.
 */
#define CLI_NOTIF       (1 << 15)
#define SOCF_CAL_NOTIF   (1 << 16)
#define SOCF_CAL_TIME     1000
#define SOCF_CAL_TASK_PRIORITY   ( tskIDLE_PRIORITY + 1 )

typedef enum {
        SOCF_CAL_VOLT_3000,
        SOCF_CAL_VOLT_4200,
        SOCF_CAL_TEMP_0,
        SOCF_CAL_TEMP_25,
} SOCF_CAL_TYPE;

PRIVILEGED_DATA static OS_TASK xPtTaskHandle;
PRIVILEGED_DATA static TimerHandle_t xCalTimer;
static SOCF_CAL_TYPE socf_cal_type;
static int16_t socf_cal_count;
static int32_t socf_cal_total_val;
static int32_t socf_cal_backup_val;
#if SOCF_USE_OFFSET_COMP
extern int16_t socf_temp_offset_0;
extern int16_t socf_temp_offset_25;
extern int16_t socf_volt_offset_3000;
extern int16_t socf_volt_offset_4200;
#endif

static void cal_temp_0(int argc, const char *argv[], void *user_data)
{
        socf_cal_type = SOCF_CAL_TEMP_0;
        printf("Starting temperature calibration at 0C \r\n");
        socf_cal_count = 0;
        socf_cal_total_val = 0;
        socf_temp_offset_0 = 0;
        socf_cal_backup_val = socf_temp_offset_25;
        socf_temp_offset_25 = 0;
        OS_TIMER_START(xCalTimer, OS_TIMER_FOREVER);
}

static void cal_temp_25(int argc, const char *argv[], void *user_data)
{
        socf_cal_type = SOCF_CAL_TEMP_25;
        printf("Starting temperature calibration at 25C \r\n");
        socf_cal_count = 0;
        socf_cal_total_val = 0;
        socf_temp_offset_25 = 0;
        socf_cal_backup_val = socf_temp_offset_0;
        socf_temp_offset_0 = 0;
        OS_TIMER_START(xCalTimer, OS_TIMER_FOREVER);
}

static void cal_volt_4200(int argc, const char *argv[], void *user_data)
{
        socf_cal_type = SOCF_CAL_VOLT_4200;
        printf("Starting voltage calibration at 4200mV \r\n");
        socf_cal_count = 0;
        socf_cal_total_val = 0;
        socf_volt_offset_4200 = 0;
        socf_cal_backup_val = socf_volt_offset_3000;
        socf_volt_offset_3000 = 0;
        OS_TIMER_START(xCalTimer, OS_TIMER_FOREVER);
}

static void cal_volt_3000(int argc, const char *argv[], void *user_data)
{
        socf_cal_type = SOCF_CAL_VOLT_3000;
        printf("Starting voltage calibration at 3000mV \r\n");
        socf_cal_count = 0;
        socf_cal_total_val = 0;
        socf_volt_offset_3000 = 0;
        socf_cal_backup_val = socf_volt_offset_4200;
        socf_volt_offset_4200 = 0;
        OS_TIMER_START(xCalTimer, OS_TIMER_FOREVER);
}

static void cal_volt_reset(int argc, const char *argv[], void *user_data)
{
        nvparam_t param_area;

        OS_TIMER_STOP(xCalTimer, OS_TIMER_FOREVER);

        printf("Saving offset 0 at 4200mV, 0 at 3000mV \r\n");
        socf_volt_offset_4200 = 0;
        socf_volt_offset_3000 = 0;
        param_area = ad_nvparam_open("ble_app");
        ad_nvparam_write(param_area, TAG_SOC_BATT_HVOL_GAIN_CAL, sizeof(int16_t), &socf_volt_offset_4200);
        ad_nvparam_write(param_area, TAG_SOC_BATT_LVOL_GAIN_CAL, sizeof(int16_t), &socf_volt_offset_3000);
        ad_nvparam_close(param_area);

        printf("DONE voltage adc offset reset! \r\n");
}

static void cal_temp_reset(int argc, const char *argv[], void *user_data)
{
        nvparam_t param_area;

        OS_TIMER_STOP(xCalTimer, OS_TIMER_FOREVER);

        printf("Saving offset 0 at 0C, 0 at 25C \r\n");
        socf_temp_offset_0 = 0;
        socf_temp_offset_25 = 0;
        param_area = ad_nvparam_open("ble_app");
        ad_nvparam_write(param_area, TAG_SOC_BATT_LTMP_GAIN_CAL, sizeof(int16_t), &socf_temp_offset_0);
        ad_nvparam_write(param_area, TAG_SOC_BATT_LVOL_GAIN_CAL, sizeof(int16_t), &socf_volt_offset_3000);
        ad_nvparam_close(param_area);

        printf("DONE temperature adc offset reset! \r\n");
}

static void clicmd_default_handler(int argc, const char *argv[], void *user_data)
{
        printf("Valid commands:\r\n");
        printf("cal_volt_4200\r\n");
        printf("cal_volt_3000\r\n");
        printf("cal_volt_reset\r\n");
        printf("cal_temp_0\r\n");
        printf("cal_temp_25\r\n");
        printf("cal_temp_reset\r\n");
}

static void pt_start(int argc, const char *argv[], void *user_data)
{
        hw_cpm_reboot_system();
        while (1);
}

static const cli_command_t cli_commands[] = {
        { .name = "cal_volt_4200",      .handler = cal_volt_4200, },
        { .name = "cal_volt_3000",      .handler = cal_volt_3000, },
        { .name = "cal_volt_reset",     .handler = cal_volt_reset, },
        { .name = "cal_temp_0",         .handler = cal_temp_0, },
        { .name = "cal_temp_25",        .handler = cal_temp_25, },
        { .name = "cal_temp_reset",     .handler = cal_temp_reset, },
        { .name = "pt_start",           .handler = pt_start, },
        {},
};

static void socf_cal_cb(TimerHandle_t timer)
{
        OS_TASK task = (OS_TASK)pvTimerGetTimerID(timer);
        OS_TASK_NOTIFY(task, SOCF_CAL_NOTIF, eSetBits);
}

static void socf_cal_process()
{
        int16_t val;
        int16_t offset;
        nvparam_t param_area;

        switch (socf_cal_type) {
        case SOCF_CAL_TEMP_0:
                val = socf_hal_get_degree(false);
                printf("Collecting value %d at 0C \r\n", val );
                socf_cal_total_val += val;
                break;
        case SOCF_CAL_TEMP_25:
                val = socf_hal_get_degree(false);
                printf("Collecting value %d at 25C \r\n", val );
                socf_cal_total_val += val;
                break;
        case SOCF_CAL_VOLT_4200:
                val = socf_hal_measure_vbat(false);
                printf("Collecting value %d at 4200mV \r\n", val );
                socf_cal_total_val += val;
                break;
        case SOCF_CAL_VOLT_3000:
                val = socf_hal_measure_vbat(false);
                printf("Collecting value %d at 3000mV \r\n", val );
                socf_cal_total_val += val;
                break;
        default:
                break;
        }
        socf_cal_count ++;
        if (socf_cal_count < 10) {
                OS_TIMER_START(xCalTimer, OS_TIMER_FOREVER);
                return;
        }

        param_area = ad_nvparam_open("ble_app");
        switch (socf_cal_type) {
        case SOCF_CAL_TEMP_0:
                offset = 0 - socf_cal_total_val / 10;
                printf("Saving offset %d at 0C \r\n", offset);
                socf_temp_offset_0 = offset;
                socf_temp_offset_25 = socf_cal_backup_val;
                ad_nvparam_write(param_area, TAG_SOC_BATT_LTMP_GAIN_CAL, sizeof(int16_t), &socf_temp_offset_0);
                break;
        case SOCF_CAL_TEMP_25:
                offset = 25 - socf_cal_total_val / 10;
                printf("Saving offset %d at 25C \r\n", offset);
                socf_temp_offset_25 = offset;
                socf_temp_offset_0 = socf_cal_backup_val;
                ad_nvparam_write(param_area, TAG_SOC_BATT_HTMP_GAIN_CAL, sizeof(int16_t), &socf_temp_offset_25);
                break;
        case SOCF_CAL_VOLT_4200:
                offset = 4200 - socf_cal_total_val / 10;
                printf("Saving offset %d at 4200mV \r\n", offset);
                socf_volt_offset_4200 = offset;
                socf_volt_offset_3000 = socf_cal_backup_val;
                ad_nvparam_write(param_area, TAG_SOC_BATT_HVOL_GAIN_CAL, sizeof(int16_t), &socf_volt_offset_4200);
                break;
        case SOCF_CAL_VOLT_3000:
                offset = 3000 - socf_cal_total_val / 10;
                printf("Saving offset %d at 3000mV \r\n", offset);
                socf_volt_offset_3000 = offset;
                socf_volt_offset_4200 = socf_cal_backup_val;
                ad_nvparam_write(param_area, TAG_SOC_BATT_LVOL_GAIN_CAL, sizeof(int16_t), &socf_volt_offset_3000);
                break;
        default:
                break;
        }
        ad_nvparam_close(param_area);
        printf("DONE calibration! \r\n");
}

void socf_cli_task(void *params)
{
        cli_t cli;

        cli = cli_register(CLI_NOTIF, cli_commands, clicmd_default_handler);

        xCalTimer = OS_TIMER_CREATE("Cal Timer",
                                OS_MS_2_TICKS(SOCF_CAL_TIME),
                                OS_TIMER_FAIL,
                                (void *) OS_GET_CURRENT_TASK(),
                                socf_cal_cb);

        for (;;) {
                OS_BASE_TYPE ret;
                uint32_t notif;

                ret = OS_TASK_NOTIFY_WAIT(0, (uint32_t) -1, &notif, portMAX_DELAY);
                OS_ASSERT(ret == OS_TASK_NOTIFY_SUCCESS);

                if (notif & CLI_NOTIF) {
                        cli_handle_notified(cli);
                }

                if (notif & SOCF_CAL_NOTIF) {
                        socf_cal_process();
                }
        }
}

void socf_cal_start(void)
{
        OS_BASE_TYPE status;

        if (xPtTaskHandle == NULL) {
                status = OS_TASK_CREATE("SOC_log",
                                        socf_cli_task,
                                        ( void * ) NULL,
                                        1024,
                                        ( SOCF_CAL_TASK_PRIORITY ),
                                        xPtTaskHandle);
                OS_ASSERT(status == OS_OK);

                (void)status;
        }
}
#endif
