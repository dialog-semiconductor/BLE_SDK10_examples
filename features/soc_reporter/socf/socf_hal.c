/**
 ****************************************************************************************
 *
 * @file socf_hal.c
 *
 * @brief SOC function
 *
 * Copyright (C) 2015-2021 Renesas Electronics Corporation and/or its affiliates
 * The MIT License (MIT)
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
 * OR OTHER DEALINGS IN THE SOFTWARE.
 ****************************************************************************************
 */

#include "sdk_defs.h"
#include "osal.h"
#include "sys_charger.h"
#include "sys_power_mgr.h"
#include "sys_timer.h"
#include "timers.h"
#include "ad_gpadc.h"
#include "ad_nvparam.h"
#include "platform_devices.h"
#include "app_nvparam.h"
#include "socf_config.h"
#include "socfi.h"
#include "socf_hal.h"
#include "socf_client.h"
#include "sys_usb.h"
#include "hw_sys.h"
#include "hw_pdc.h"
#include "hw_rtc.h"
#include "ad_sdadc.h"
#include "ad_snc.h"
#include "socf_ucodes.h"

#if (dg_configUSE_LP_CLK == LP_CLK_32000)
#define SOCF_REF_LP_FACTOR ((1000 << 20) / 32000)
#elif (dg_configUSE_LP_CLK == LP_CLK_32768)
#define SOCF_REF_LP_FACTOR ((1000 << 20) / 32768)
#elif (dg_configUSE_LP_CLK == LP_CLK_RCX)
#define SOCF_REF_LP_FACTOR ((1000 << 20) / rcx_clock_hz)
#elif (dg_configUSE_LP_CLK == LP_CLK_ANY)
// Must be defined in the custom_config_<>.h file.
#else
# error "SOCF_REF_LP_FACTOR is not defined!"
#endif

#define socf_cal_TASK_PRIORITY      ( tskIDLE_PRIORITY)
#if (SOCF_USE_SNC == 0)
#define SOCF_FROM_WAKEUP_CAL_BIT    ( 1 << 2 )
#endif
#define SOCF_FROM_ACTIVE_CAL_BIT    ( 1 << 3 )

PRIVILEGED_DATA static OS_TASK socf_cal_threadId = NULL;
#if (SOCF_USE_SNC == 0)
PRIVILEGED_DATA static void *socfHalTimer;
PRIVILEGED_DATA static int32_t socf_random_seed;
#endif
PRIVILEGED_DATA static bool socf_charging_state;
#if SOCF_USE_OFFSET_COMP
#define SOCF_LOW_TEMP_COMP      0
#define SOCF_HIGH_TEMP_COMP     25
#define SOCF_LOW_VOLT_COMP      3000
#define SOCF_HIGH_VOLT_COMP     4200
PRIVILEGED_DATA int16_t socf_temp_offset_0;
PRIVILEGED_DATA int16_t socf_temp_offset_25;
PRIVILEGED_DATA int16_t socf_volt_offset_3000;
PRIVILEGED_DATA int16_t socf_volt_offset_4200;
#endif

void usb_charger_battery_full ()
{
        socfi_full_charged_notification();
}

void sys_usb_ext_hook_attach(void)
{
        socf_charging_state = true;
}

void sys_usb_ext_hook_detach(void)
{
        socf_charging_state = false;
}

static bool ad_socf_prepare_for_sleep(void)
{
        socf_hal_calcaulation_before_sleep();
        return true;
}

static void ad_socf_wake_up_ind(bool flag)
{
        socf_hal_calcaulation_after_wakeup();
}

static const adapter_call_backs_t ad_socf_pm_call_backs = {
        .ad_prepare_for_sleep = ad_socf_prepare_for_sleep,
        .ad_sleep_canceled = NULL,
        .ad_wake_up_ind = ad_socf_wake_up_ind,
        .ad_xtalm_ready_ind = NULL,
        .ad_sleep_preparation_time = 0
};

int16_t socf_get_lut(int16_t* x, int16_t* y, int16_t v, int16_t l)
{
        int16_t i;
        int16_t ret;

        if (v < x[0]) {
                ret = y[0];
        } else if (v >= x[l - 1]) {
                ret = y[l - 1];
        } else {
                for (i = 1; i < l; i++) {
                        if (v < x[i]) {
                                break;
                        }
                }
                ret = y[i - 1];
                ret = ret + (int16_t)((int32_t)(v - x[i - 1])
                        * (int32_t)(y[i] - y[i - 1]))
                        / (int32_t)(x[i] - x[i - 1]);
        }
        return ret;
}

#if USE_SDADC_FOR_VBAT
int16_t socf_hal_measure_vbat_sd(ad_sdadc_controller_conf_t *conf, bool task_suspended)
{
        int32_t vbat = 0;
        ad_sdadc_handle_t handle;

        if (task_suspended == true) {
                sdadc_config sdadc_config;

                sdadc_config.clock = BATTERY_LEVEL_SD.drv->clock;
                sdadc_config.input_mode = BATTERY_LEVEL_SD.drv->input_mode;
                sdadc_config.inn = BATTERY_LEVEL_SD.drv->inn;
                sdadc_config.inp = BATTERY_LEVEL_SD.drv->inp;
                sdadc_config.continuous = BATTERY_LEVEL_SD.drv->continuous ;
                sdadc_config.over_sampling = BATTERY_LEVEL_SD.drv->over_sampling ;
                sdadc_config.vref_selection = BATTERY_LEVEL_SD.drv->vref_selection;
                sdadc_config.vref_voltage = BATTERY_LEVEL_SD.drv->vref_voltage ;
#if (dg_configDEVICE == DEVICE_DA1469x)
                sdadc_config.freq = BATTERY_LEVEL_SD.drv->freq;
#endif
                sdadc_config.use_dma = BATTERY_LEVEL_SD.drv->use_dma;
                sdadc_config.mask_int = BATTERY_LEVEL_SD.drv->mask_int ;

                hw_sdadc_reset();
                hw_sdadc_configure(&sdadc_config);
                vbat = hw_sdadc_get_voltage(&sdadc_config);
        } else {
                handle = ad_sdadc_open(&BATTERY_LEVEL_SD);
                if (handle != NULL) {
                        ad_sdadc_read(handle, &vbat);
                        ad_sdadc_close(handle, false);
                }
        }
#if SOCF_USE_OFFSET_COMP
        vbat = vbat + socf_volt_offset_4200 - ((SOCF_HIGH_VOLT_COMP - vbat) * (socf_volt_offset_4200 - socf_volt_offset_3000))
                / (SOCF_HIGH_VOLT_COMP - SOCF_LOW_VOLT_COMP);
#endif
        return (int16_t)vbat;
}
#else
static int16_t socf_hal_measure_vbat_gp(ad_gpadc_controller_conf_t *conf, bool task_suspended)
{
        uint16_t vbat = 0;
        ad_gpadc_handle_t handle;

        if (task_suspended == true) {
                hw_gpadc_reset();
                hw_gpadc_set_input(BATTERY_LEVEL_GP.drv->input);
                hw_gpadc_set_input_mode(BATTERY_LEVEL_GP.drv->input_mode);
                hw_gpadc_set_sample_time(BATTERY_LEVEL_GP.drv->sample_time);
                hw_gpadc_set_chopping(BATTERY_LEVEL_GP.drv->chopping);
                hw_gpadc_set_oversampling(BATTERY_LEVEL_GP.drv->oversampling);
                hw_gpadc_adc_measure();
                vbat = hw_gpadc_get_value();
        } else {
                handle = ad_gpadc_open(&BATTERY_LEVEL_GP);
                if (handle != NULL) {
                        ad_gpadc_read(handle, &vbat);
                        ad_gpadc_close(handle, false);
                }
        }
        vbat = ad_gpadc_conv_to_batt_mvolt(BATTERY_LEVEL_GP.drv, vbat);
#if SOCF_USE_OFFSET_COMP
        vbat = vbat + socf_volt_offset_4200 - ((SOCF_HIGH_VOLT_COMP - vbat) * (socf_volt_offset_4200 - socf_volt_offset_3000))
                / (SOCF_HIGH_VOLT_COMP - SOCF_LOW_VOLT_COMP);
#endif

        return (uint16_t)vbat;
}

#endif // USE_SDADC_FOR_VBAT

int16_t socf_hal_measure_vbat(bool task_suspended)
{
#if USE_SDADC_FOR_VBAT
        return (socf_hal_measure_vbat_sd((ad_sdadc_controller_conf_t *)&BATTERY_LEVEL_SD, task_suspended));
#else
        return (socf_hal_measure_vbat_gp((ad_gpadc_controller_conf_t *)&BATTERY_LEVEL_GP, task_suspended));
#endif
}

int16_t socf_hal_get_degree(bool taskended)
{
        return 25; //room temperature
}

#if (SOCF_USE_AGED_CYCLE == 1)
int16_t socf_hal_get_accumulated_soc_from_nand(void)
{
        nvparam_t param_area;
        uint16_t len;
        int16_t soc;

        param_area = ad_nvparam_open("ble_app");
        len = ad_nvparam_read(param_area, TAG_SOC_ACCUMULATED_SOC, sizeof(int16_t), &soc);
        if (len == 0) {
                soc = 0;
        }
        ad_nvparam_close(param_area);

        return soc;
}

int16_t socf_hal_get_aged_cycle_from_nand(void)
{
        nvparam_t param_area;
        uint16_t len;
        int16_t cycle;

        param_area = ad_nvparam_open("ble_app");
        len = ad_nvparam_read(param_area, TAG_SOC_AGED_CYCLE, sizeof(int16_t), &cycle);
        if (len == 0) {
                cycle = 0;
        }
        ad_nvparam_close(param_area);

        return cycle;
}

void socf_hal_put_accumulated_soc_to_nand(int16_t soc)
{
        nvparam_t param_area;

        param_area = ad_nvparam_open("ble_app");
        if (param_area != NULL) {
                ad_nvparam_write(param_area, TAG_SOC_ACCUMULATED_SOC, sizeof(int16_t), &soc);
                ad_nvparam_close(param_area);
        }
}

void socf_hal_put_aged_cycle_to_nand(int16_t cycle)
{
        nvparam_t param_area;

        param_area = ad_nvparam_open("ble_app");
        if (param_area != NULL) {
                ad_nvparam_write(param_area, TAG_SOC_AGED_CYCLE, sizeof(int16_t), &cycle);
                ad_nvparam_close(param_area);
        }
}
#endif

void socf_hal_init(void)
{
#if SOCF_USE_OFFSET_COMP
        nvparam_t param_area;
        uint16_t len;
#endif

        pm_register_adapter(&ad_socf_pm_call_backs);

#if SOCF_USE_OFFSET_COMP
        param_area = ad_nvparam_open("ble_app");
        len = ad_nvparam_read(param_area, TAG_SOC_BATT_LVOL_GAIN_CAL, sizeof(int16_t), &socf_volt_offset_3000);
        if (len == 0) {
                socf_volt_offset_3000 = 0;
        }

        len = ad_nvparam_read(param_area, TAG_SOC_BATT_HVOL_GAIN_CAL, sizeof(int16_t), &socf_volt_offset_4200);
        if (len == 0) {
                socf_volt_offset_4200 = 0;
        }

        len = ad_nvparam_read(param_area, TAG_SOC_BATT_LTMP_GAIN_CAL, sizeof(int16_t), &socf_temp_offset_0);
        if (len == 0) {
                socf_temp_offset_0 = 0;
        }

        len = ad_nvparam_read(param_area, TAG_SOC_BATT_HTMP_GAIN_CAL, sizeof(int16_t), &socf_temp_offset_25);
        if (len == 0) {
                socf_temp_offset_25 = 0;
        }
        ad_nvparam_close(param_area);
#endif

        socf_init(-1, SOCF_IBAT_BOOT);
}

bool socf_hal_is_charging(void)
{
        return socf_charging_state;
}

uint64_t socf_hal_get_time(void)
{
        return sys_timer_get_uptime_ticks();
}

#if (SOCF_USE_AGED_RATE == 1)
bool socf_hal_is_cc_level()
{
        if (hw_charger_is_const_current_mode_on() == true) {
                return true;
        } else {
                return false;
        }
}
#endif

int32_t socf_hal_get_duration(uint64_t now, uint64_t pre)
{
        return (int32_t)((((now) - (pre)) * (int64_t)SOCF_REF_LP_FACTOR) >> 20);
}

#if (SOCF_USE_SNC == 0)
static void socf_hal_timer_cb(OS_TIMER timer)
{
        OS_TASK_NOTIFY(socf_cal_threadId, SOCF_FROM_ACTIVE_CAL_BIT, eSetBits);
}

void socf_hal_stop_timer(void)
{
        if (socfHalTimer == NULL) {
                return;
        }
        OS_TIMER_STOP(socfHalTimer, OS_TIMER_FOREVER);
}

void socf_hal_start_timer(uint32_t time)
{
        if (socfHalTimer == NULL) {
                return;
        }
        OS_TIMER_STOP(socfHalTimer, OS_TIMER_FOREVER);
        OS_TIMER_CHANGE_PERIOD(socfHalTimer, OS_MS_2_TICKS(time), OS_TIMER_FOREVER);
        OS_TIMER_START(socfHalTimer, OS_TIMER_FOREVER);
}

void socf_hal_create_timer(void)
{
        if (socfHalTimer == NULL) {
                socfHalTimer = OS_TIMER_CREATE("SOC_Timer",
                        OS_MS_2_TICKS(SOCF_HAL_TIMER_TIME),
                        OS_TIMER_FAIL,
                        (void *)0,
                        socf_hal_timer_cb);
                OS_ASSERT(socfHalTimer != NULL);
        }
}
#endif
void socf_hal_calcaulation_after_wakeup(void)
{
#if (SOCF_USE_SNC == 0)

        if (socf_hal_get_duration(socf_hal_get_time(), socf_get_pre_time())
                >= SOCF_SAMPLING_TIME) {
                OS_TASK_NOTIFY(socf_cal_threadId, SOCF_FROM_ACTIVE_CAL_BIT, eSetBits);
        }
#endif
}

void socf_hal_calcaulation_before_sleep(void)
{
}

#if (SOCF_USE_SNC == 1)
/**
 * @brief RTC initialization function
 *
 * This function initializes RTC to produce PDC events periodically in a fixed interval
 *
 * \param [in] pdcIntervalms The PDC event interval in ms (must be multiple of 10)
 *
 */
static void socf_rtc_init(uint32_t pdcIntervalms)
{
        ASSERT_WARNING((pdcIntervalms > 0) && ((pdcIntervalms % 10) == 0));

        rtc_config_pdc_evt rtc_cfg = { 0 };

        /* Enable the RTC PDC event */
        rtc_cfg.pdc_evt_en = true;

        /* Set the RTC PDC event period */
        rtc_cfg.pdc_evt_period = (pdcIntervalms / 10) - 1;

        /* Initialize the RTC peripheral with the given configuration */
        hw_rtc_config_RTC_to_PDC_evt(&rtc_cfg);

        /* Enable the RTC peripheral clock */
        hw_rtc_clock_enable();

        /* Start the RTC */
        hw_rtc_time_start();
}

uint32_t soc_snc_init(ad_snc_interrupt_cb _socf_snc_cb, uint32_t notify_samples)
{
        uint32_t ucode_id;
        ad_snc_ucode_cfg_t cfg = { 0 };
        snc_ucode_context_t *ucode_ctx;

        // Configure the PDC event and uCode priorities
        cfg.pdc_evt_pr = AD_SNC_PDC_EVT_PR_0;
        cfg.ucode_pr = AD_SNC_UCODE_PR_1;

        // Set the SNC to CM33 notification callback
        cfg.cb = _socf_snc_cb;

        // The BH1750 uCode is executed on RTC-to-PDC event interrupt
        cfg.pdc_entry = HW_PDC_LUT_ENTRY_VAL(HW_PDC_TRIG_SELECT_PERIPHERAL,
                HW_PDC_PERIPH_TRIG_ID_RTC_TIMER, HW_PDC_MASTER_SNC, HW_PDC_LUT_ENTRY_EN_PER);

        ucode_socf_set_notify_samples(notify_samples);
        ucode_ctx = SNC_UCODE_CTX(ucode_socf_collect_samples_queue);

        cfg.snc_to_cm33_queue_cfg.enable_data_timestamp = false;
        cfg.snc_to_cm33_queue_cfg.element_weight = SNC_QUEUE_ELEMENT_SIZE_WORD;
        cfg.snc_to_cm33_queue_cfg.swap_popped_data_bytes = false;
        cfg.snc_to_cm33_queue_cfg.num_of_chunks = notify_samples;
        cfg.snc_to_cm33_queue_cfg.max_chunk_bytes = SOCF_SNC_GPADC_QUEUE_CHUNK_SIZE;

        // Register uCode
        ucode_id = ad_snc_ucode_register(&cfg, ucode_ctx);

        // Enable uCode
        ad_snc_ucode_enable(ucode_id);

        return ucode_id;
}

static void socf_snc_cb(void)
{
        OS_TASK_NOTIFY(socf_cal_threadId, SOCF_FROM_ACTIVE_CAL_BIT, eSetBits);
}
#endif

static void socf_cal_thread(void *pvParameters)
{
        (void)pvParameters;

        uint32_t ulNotifiedValue;
        OS_BASE_TYPE xResult __attribute__((unused));
#if (SOCF_USE_SNC == 1)
        uint32_t bytes_num;
        uint32_t vbat;
        uint32_t now_time;
        int16_t degree;
#endif

#if (SOCF_USE_SNC == 1)
        hw_pd_power_up_com();
        uint32_t __UNUSED socf_ucode_id = soc_snc_init(socf_snc_cb , SOCF_SNC_NUM_OF_SAMPLES);
        socf_rtc_init(SOCF_SAMPLING_TIME);
#else
        socf_random_seed = 1;
        socf_hal_create_timer();
        socf_hal_start_timer(SOCF_HAL_TIMER_TIME);
#endif

        for (;;) {
                xResult = OS_TASK_NOTIFY_WAIT(0x0, 0xFFFFFFFF, &ulNotifiedValue, portMAX_DELAY);

#if (SOCF_USE_SNC == 0)
                if (ulNotifiedValue & SOCF_FROM_WAKEUP_CAL_BIT) {
                        socf_calculation(true, false);
                        socf_random_seed = socf_random_seed * 1103515245 + 12345;
                        socf_hal_start_timer(SOCF_HAL_TIMER_TIME + socf_random_seed % 500);
                } else
#endif
                if (ulNotifiedValue & SOCF_FROM_ACTIVE_CAL_BIT) {
#if (SOCF_USE_SNC == 1)
                        degree = socf_hal_get_degree(false);
                        while (ad_snc_queue_pop(socf_ucode_id, (uint8_t*)&vbat, &bytes_num,
                                                       NULL)) {
                                vbat = (uint32_t)ad_gpadc_conv_to_batt_mvolt(BATTERY_LEVEL_GP.drv,
                                        vbat >> (6 - MIN(6, BATTERY_LEVEL_GP.drv->oversampling)));
                                socf_process_fg_cal(SOCF_SAMPLING_TIME, (uint16_t)vbat, 0, socf_hal_is_charging(),
                                        degree);
                        }
                        now_time = socf_hal_get_time();
                        socf_set_pre_time(now_time);
#else
                        socf_calculation(false, false);
                        socf_random_seed = socf_random_seed * 1103515245 + 12345;
                        socf_hal_start_timer(SOCF_HAL_TIMER_TIME + socf_random_seed % 500);
#endif
                }
        }
}

void socf_hal_create_thread(void)
{
        OS_BASE_TYPE status;

        if (socf_cal_threadId == NULL) {
                status = OS_TASK_CREATE("SOCF_CAL",
                        socf_cal_thread,
                        ( void * ) 0,
                        1024,
                        ( socf_cal_TASK_PRIORITY ),
                        socf_cal_threadId);

                OS_ASSERT(status == OS_OK);

                (void)status;
        }
        if (socf_cal_threadId == NULL) {
                ASSERT_ERROR(0);
        }
}

ADAPTER_INIT_DEP1(ad_socf_adapter, socf_hal_init, ad_nvms_adapter)
