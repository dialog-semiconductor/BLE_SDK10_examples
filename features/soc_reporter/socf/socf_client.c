/**
 ****************************************************************************************
 *
 * @file socf_client.c
 *
 * @brief SOC function
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include "sdk_defs.h"
#include "socf_config.h"
#include "socfi.h"
#include "socf_hal.h"
#include "socf_client.h"
#include "sys_usb.h"

#define SOCF_PUT_AGED_COUNT             5
#define SOCF_PUT_AGED_TH                10 //1% SOC
#define SOCF_TH_AGED_CYCLE              1000 //100% SOC
#define SOCF_CHG_ROOM_TEMP_TH           20
#define SOCF_CC_LOW_TH                  100
#define SOCF_CC_HIGH_TH                 700
__RETAINED static uint16_t socf_client_voltage;
__RETAINED static uint64_t socf_pre_time;
#if (SOCF_USE_AGED_RATE == 1)
__RETAINED static uint64_t socf_chg_start_time;
__RETAINED static int16_t socf_chg_start_soc;
__RETAINED static int32_t socf_chg_time_duration;
__RETAINED static int16_t socf_chg_soc_interval;
#endif
#if (SOCF_USE_AGED_CYCLE == 1)
__RETAINED static int16_t socf_pre_soc;
__RETAINED static int16_t socf_accumulated_soc_nand;
__RETAINED static int16_t socf_accumulated_soc_count;
__RETAINED static int16_t socf_accumulated_soc;
__RETAINED static int32_t socf_aged_cycle;
#endif

uint64_t socf_get_pre_time()
{
        return socf_pre_time;
}

void socf_set_pre_time(uint64_t time)
{
        socf_pre_time = time;
}

void socf_set_client_voltage(uint16_t voltage_mV)
{
        socf_client_voltage = voltage_mV;
}

uint16_t socf_get_client_voltage()
{
        return socf_client_voltage;
}

int16_t socf_get_soc(void)
{
        return ((socfi_get_soc() + 5) / 10);
}

#if (SOCF_USE_AGED_RATE == 1)
int16_t socf_get_aged_percent(void)
{
        return socfi_get_aged_percent(socf_chg_time_duration, socf_chg_soc_interval);
}
#endif

#if (SOCF_USE_AGED_CYCLE == 1)
int16_t socf_get_aged_cycle_number(void)
{
        return socf_aged_cycle;
}
#endif

void socf_calculation(bool vbat_available, bool task_suspended)
{
        uint32_t now_time, duration;
        int16_t vbat;
        int16_t degree;

        now_time = socf_hal_get_time();
        duration = socf_hal_get_duration(now_time, socf_get_pre_time());

        if (duration >= SOCF_SAMPLING_TIME) {
                if (vbat_available == false) {
                        vbat = socf_hal_measure_vbat(task_suspended);
                        if (vbat > 0) {
                                socf_set_client_voltage(vbat);
                        }
                }
                degree = socf_hal_get_degree(task_suspended);

                socf_set_pre_time(now_time);
                if ( socf_hal_is_charging() == false) {
#if (SOCF_USE_AGED_RATE == 1)
                        socf_chg_start_time = 0;
#endif
#if (SOCF_USE_AGED_CYCLE == 1)
                        socf_pre_soc = socf_pre_soc
                                - socf_process_fg_cal(duration, socf_get_client_voltage(), 0, false, degree);
                        socf_accumulated_soc += socf_pre_soc - socfi_get_soc();
                        if (socf_accumulated_soc > SOCF_PUT_AGED_TH) {
                                socf_accumulated_soc_count++;
                                socf_accumulated_soc_nand += socf_accumulated_soc;
                                socf_accumulated_soc = 0;
                                if (socf_accumulated_soc_nand > SOCF_TH_AGED_CYCLE) {
                                        socf_accumulated_soc_nand -= SOCF_TH_AGED_CYCLE;
                                        socf_aged_cycle++;
                                        socf_hal_put_aged_cycle_to_nand(socf_aged_cycle);
                                }
                                if (socf_accumulated_soc_count > SOCF_PUT_AGED_COUNT) {
                                        socf_accumulated_soc_count = 0;
                                        socf_hal_put_accumulated_soc_to_nand(socf_accumulated_soc_nand);
                                }
                        } else if (socf_accumulated_soc < -SOCF_PUT_AGED_TH) {
                                socf_accumulated_soc = 0;
                                socf_accumulated_soc_count = 0;
                        }
                        socf_pre_soc = socfi_get_soc();
#else
                        socf_process_fg_cal(duration, socf_get_client_voltage(), 0, false, degree);
#endif
                } else {
#if (SOCF_USE_AGED_RATE == 1)
                        if (socf_hal_is_cc_level() == true && degree > SOCF_CHG_ROOM_TEMP_TH) {
                                if (socf_chg_start_time == 0
                                        && (socfi_get_soc() > SOCF_CC_LOW_TH
                                                && socf_get_soc() < SOCF_CC_HIGH_TH)) {
                                        socf_chg_start_time = now_time;
                                        socf_chg_start_soc = socfi_get_soc();
                                }

                                if (socf_chg_start_time > 0
                                        && (socfi_get_soc() - socf_chg_start_soc) > 200)  {
                                        socf_chg_time_duration = socf_hal_get_duration(now_time, socf_chg_start_time);
                                        socf_chg_soc_interval = socfi_get_soc() - socf_chg_start_soc;
                                 }
                        } else {
                                socf_chg_start_time = 0;
                                socf_chg_start_soc = 0;
                        }
#endif
                        socf_process_fg_cal(duration, socf_get_client_voltage(), 0, true, degree);
#if (SOCF_USE_AGED_CYCLE == 1)
                        socf_accumulated_soc = 0;
                        socf_accumulated_soc_count = 0;
                        socf_pre_soc = socfi_get_soc();
#endif
                }
        }
}

void socf_init(int16_t soc, int16_t ibat)
{
        int16_t vbat=0, ret=0;
        int16_t degree;

#if (SOCF_USE_AGED_RATE == 1)
        socf_chg_soc_interval = 0;
        socf_chg_start_soc = 0;
        socf_chg_start_time = 0;
        socf_chg_time_duration = 0;
#endif
#if (SOCF_USE_AGED_CYCLE == 1)
        socf_accumulated_soc = 0;
        socf_accumulated_soc_count = 0;
        socf_accumulated_soc_nand = socf_hal_get_accumulated_soc_from_nand();
        socf_aged_cycle = socf_hal_get_aged_cycle_from_nand();
#endif
        socfi_init();
        vbat = socf_hal_measure_vbat(true);
        if (ret != 0) {
                ASSERT_ERROR(0);
        }

        socf_set_client_voltage(vbat);
        degree = socf_hal_get_degree(true);
        if (hw_usb_is_powered_by_vbus() == false) {
                socfi_soc_init_calculation(vbat, ibat, false, soc, degree);
        } else {
                socfi_soc_init_calculation(vbat, ibat, true, soc, degree);
        }
        socf_pre_time = socf_hal_get_time();
#if (SOCF_USE_AGED_CYCLE == 1)
        socf_pre_soc = socfi_get_soc();
#endif
        socf_hal_create_thread();
}
