/**
 ****************************************************************************************
 *
 * @file socf.c
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "socfi.h"
#include "socf_config.h"
#include "socf_profile_data.h"
#include "sdk_defs.h"

//#define SOCF_ENABLE_DEBUG_COFF
#define SOCF_ALLOW_SOC_UP_DIS

#define INTERVAL_SOC            5
#define FACTOR_INT              (10 / INTERVAL_SOC)
#define SOCF_MAX_SOC_VAL        1000
#define SOCF_MIN_SOC_VAL        0
#define SOCF_NORM_VAL           500000
#define SOCF_MAX_CURRENT        (4000)
#define SOCF_TIME_NORM          100
#define SOCF_SYS_MIN_VOLTAGE    1800
#define SOCF_REF_TEMP           25
#define SOCF_MAX_TEMP           45
#define SOCF_MIN_TEMP           0
#define SOCF_MS_TIME            (1000 / SOCF_TIME_NORM)
#define SOCF_MIN_SOC_CAP        20
#define SOCF_1_HOUR_SEC         3600
#define SOCF_INTERPOL(Xlow,Ylow,Xhigh,Yhigh,Xvalue)     (Ylow + ((Yhigh - Ylow) * (Xvalue - Xlow)) / (Xhigh - Xlow))
#define SOCF_M_CAL(Imin,Imax,Vmin,Vmax,Vdiff)   (Imin - (Vdiff * (Imax - Imin))/(Vmax - Vmin))

__RETAINED static int64_t socfi_coulomb;
__RETAINED static int16_t socfi_now_cap;
__RETAINED static int16_t socfi_soc = -1;
__RETAINED static int16_t socfi_lv_e;
__RETAINED static int16_t socfi_pre_deg;
__RETAINED static int16_t socfi_estimated_cap;
__RETAINED static int16_t socfi_rlut[VOL2SOC_LUT_SIZE];
__RETAINED static int16_t socfi_cur_comp_lut[SOCF_M_CAL_NUM];
static const int16_t socfi_ref[VOL2SOC_LUT_SIZE] = {
        0, 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900,
        950, 1000
};

#ifdef SOCF_ENABLE_DEBUG_COFF
__RETAINED static int16_t socfi_vbat, socfi_temp;
__RETAINED static int32_t socfi_dv, socfi_curr, socfi_ddv, socfi_dvolt;
#endif

static int16_t socfi_chk_lut(int16_t* x, int16_t* y, int16_t v, int16_t l)
{
        int16_t i;
        int16_t ret;

        for (i = 0; i < l; i++) {
                if (v < x[i]) {
                        break;
                }
        }
        i--;

        if (i < 0) {
                i = 0;
        }

        ret = y[i] + (int16_t)(((int32_t)(v - x[i])
                * (int32_t)(y[i + 1] - y[i]))
                / (int32_t)(x[i + 1] - x[i]));

        return ret;
}

static int16_t socfi_chk_lut_inv(int16_t* x, int16_t* y, int16_t v, int16_t l)
{
        int16_t i;
        int16_t ret;

        for (i = l - 1; i > -1; i--) {
                if (v < x[i]) {
                        break;
                }
        }
        i++;

        if (i > l - 2) {
                i = l - 2;
        }

        ret = y[i] + (int16_t)(((int32_t)(v - x[i])
                * (int32_t)(y[i + 1] - y[i]))
                / (int32_t)(x[i + 1] - x[i]));

        return ret;
}

//time for 1%  = (capacitance *3600 * 1 / 100) / 200
int32_t socfi_soc_get_sec_to_charged()
{
        if (socfi_soc <= 900) {
                return ((socfi_estimated_cap * 36 * 200 + socfi_estimated_cap * 36 * (900 - socfi_soc))
                        / (socf_conf.socf_chg_cc[socf_conf.socf_25c_num] * 10));
        } else {
                return ((socfi_estimated_cap * 36 * (1000 - socfi_soc))
                        / (socf_conf.socf_chg_cc[socf_conf.socf_25c_num] * 5));
        }
}

int16_t socfi_get_aged_percent(int32_t time_duration, int16_t soc_interval)
{
        if (soc_interval > SOCF_MIN_SOC_CAP
                && time_duration > 1) {
                socfi_estimated_cap = (socf_conf.socf_chg_cc[socf_conf.socf_25c_num] * time_duration)
                        / (3600 * soc_interval);
        }

        if (socfi_estimated_cap >= socf_conf.socf_cap_p[socf_conf.socf_25c_num]) {
                return 0;
        } else {
                return ((socf_conf.socf_cap_p[socf_conf.socf_25c_num] - socfi_estimated_cap) * 100)
                        / socf_conf.socf_cap_p[socf_conf.socf_25c_num];
        }
}

void socfi_init(void)
{
        socfi_estimated_cap = socf_conf.socf_cap_p[socf_conf.socf_25c_num];
}

static int16_t socfi_get_rlut()
{
        int i;

        for (i = 0; i < VOL2SOC_LUT_SIZE; i++) {
                socfi_rlut[i] = socf_conf.socf_lluts_p[0][i];
        }
        for (i = 0; i < SOCF_M_CAL_NUM; i++) {
                socfi_cur_comp_lut[i] = socf_conf.socf_cur_comp[0][i];
        }
        return 0;
}

static int16_t socfi_get_soc_from_vbat_profile_with_no_temp(int16_t vbat,
    uint16_t duration_ms, bool is_charging)
{
    int32_t now_diff, index, now_m, dvolt;

    duration_ms = duration_ms / SOCF_TIME_NORM;

    if ((socfi_soc == SOCF_MAX_SOC_VAL) && (vbat >= socfi_rlut[VOL2SOC_LUT_SIZE - 1])) {
        socfi_lv_e = socfi_rlut[VOL2SOC_LUT_SIZE - 1];
        socfi_soc = SOCF_MAX_SOC_VAL;

        return 0;
    }

    now_diff = socfi_lv_e - vbat;

    if (socfi_soc < SOCF_MIN_SOC_VAL) {
        index = 0;
    } else if (socfi_soc > SOCF_MAX_SOC_VAL) {
        index = VOL2SOC_LUT_SIZE - 1;
    } else {
        index = socfi_soc / (100 / FACTOR_INT);
    }

    if (vbat >= socfi_lv_e) {
        if (socfi_soc < 0) {
            dvolt = socf_conf.socf_cluts_p[0][0];

            if (vbat >= dvolt)  {
                socfi_soc = 0;
                socfi_lv_e = socfi_rlut[0];
                socfi_coulomb = socfi_now_cap * 3600000;
            }
        }

        dvolt = socf_conf.socf_cluts_p[0][index];

        if (socf_conf.socf_cluts_p[0][index] > (socf_conf.socf_chg_cv - 5)) {
            for (now_m = VOL2SOC_LUT_SIZE - 1; now_m >= 0; now_m--)     {
                if (socf_conf.socf_cluts_p[0][now_m] < (socf_conf.socf_chg_cv - 5)) {
                    break;
                }
            }

            now_m = (socf_conf.socf_chg_cc[0] * (dvolt - socfi_rlut[index]))
                / ((socf_conf.socf_cluts_p[0][now_m] - socf_conf.socf_lluts_p[0][now_m]));
        } else {
            now_m = socf_conf.socf_chg_cc[0];
        }

        now_m = SOCF_M_CAL(socf_conf.socf_dis_low,
            now_m,
            socfi_rlut[index],
            dvolt,
            now_diff);

        if (vbat >= (socf_conf.socf_chg_cv - 2)) {
            now_m = (now_m * 4) / 5;
        }

        if (socf_conf.socf_shunt != 0) {
            now_diff = socfi_lv_e
                - (vbat - (now_m * socf_conf.socf_shunt) / 1000);
            now_m = SOCF_M_CAL(socf_conf.socf_dis_low,
                socf_conf.socf_chg_cc[0],
                socfi_rlut[index],
                dvolt,
                now_diff);
        }
        if (now_m > SOCF_MAX_CURRENT) {
            now_m = SOCF_MAX_CURRENT;
        }
    }
    else {

        dvolt = socf_conf.socf_hluts_p[0][index];

        if (socfi_rlut[index] == dvolt) {
            now_m = 0;
        } else if (socfi_rlut[index] < dvolt) {
            now_m = SOCF_M_CAL((int32_t)socf_conf.socf_dis_low,
                socf_conf.socf_dis_high,
                0,
                1,
                now_diff);
        } else {
            now_m = SOCF_M_CAL(socf_conf.socf_dis_low,
                socf_conf.socf_dis_high,
                socfi_rlut[index],
                dvolt,
                now_diff);
        }

        if (socf_conf.socf_shunt != 0) {
            now_diff = socfi_lv_e
                - (vbat - (now_m * socf_conf.socf_shunt) / 1000);
            if ((int32_t)(socfi_rlut[index] - dvolt) == 0) {
                now_m = SOCF_M_CAL((int32_t)socf_conf.socf_dis_low,
                    socf_conf.socf_dis_high,
                    0,
                    1,
                    now_diff);
            } else {
                now_m = SOCF_M_CAL((int32_t)socf_conf.socf_dis_low,
                    socf_conf.socf_dis_high,
                    (int32_t)socfi_rlut[index],
                    dvolt,
                    now_diff);
            }
        }

        index = -(now_m * 200) / socf_conf.socf_cap_p[0];

        now_diff = index / 100;
        if ((now_diff) > SOCF_M_CAL_NUM - 2)
        {
            index = socfi_cur_comp_lut[SOCF_M_CAL_NUM - 1];
        } else {
            index = socfi_cur_comp_lut[now_diff]
                + ((socfi_cur_comp_lut[now_diff + 1]
                - socfi_cur_comp_lut[now_diff]) * (index % 100)) / 100;
        }

        if (now_m < -SOCF_MAX_CURRENT) {
            now_m = -SOCF_MAX_CURRENT;
        }
    }

#ifdef SOCF_ENABLE_DEBUG_COFF
    socfi_vbat = vbat;
    socfi_curr = now_m;
#endif

    socfi_coulomb -= (now_m * duration_ms * SOCF_TIME_NORM);
    socfi_soc = (((socfi_now_cap * 3600000) - socfi_coulomb)) / (socfi_now_cap * 3600);
    socfi_lv_e = socfi_chk_lut((int16_t *)socfi_ref,
        (int16_t *)socfi_rlut, socfi_soc, VOL2SOC_LUT_SIZE);

    if (socfi_lv_e < SOCF_SYS_MIN_VOLTAGE) {
        socfi_lv_e = SOCF_SYS_MIN_VOLTAGE;
        socfi_soc = SOCF_MIN_SOC_VAL;
    }

    return 0;
}

static int16_t socfi_get_soc_from_vbat_ibat_profile(int16_t vbat,
        int16_t current, bool is_charging, int16_t deg)
{
        int16_t i;
        int16_t vol[VOL2SOC_LUT_SIZE];
        int16_t *max_volt_table, max_cur;

        socfi_get_rlut();

        if (is_charging == false) {
                max_volt_table = (int16_t *)socf_conf.socf_hluts_p[0];
                max_cur = socf_conf.socf_dis_high;
        } else {
                max_volt_table = (int16_t *)socf_conf.socf_cluts_p[0];
                max_cur = socf_conf.socf_chg_cc[0];
        }
        for (i = 0; i < VOL2SOC_LUT_SIZE; i++) {
                vol[i] = socfi_rlut[i]
                        + ((current - socf_conf.socf_dis_low)
                        * (max_volt_table[i]
                        - socfi_rlut[i]))
                        / (max_cur - socf_conf.socf_dis_low);
        }
        return (socfi_chk_lut(vol, (int16_t *)socfi_ref, vbat,
                VOL2SOC_LUT_SIZE));
}

void socfi_soc_init_calculation(int16_t voltage, int16_t current,
        bool is_charging, int16_t soc, int16_t deg)
{
        socfi_pre_deg = deg;

#ifdef SOCF_ENABLE_DEBUG_COFF
        socfi_vbat = voltage;
#endif

        if (soc != -1) {
                socfi_get_rlut();
                socfi_soc = soc;
                socfi_lv_e = socfi_chk_lut((int16_t *)socfi_ref,
                        (int16_t *)socfi_rlut, socfi_soc, VOL2SOC_LUT_SIZE);

                goto InitCal;
        }

        if (voltage < SOCF_SYS_MIN_VOLTAGE) {
                socfi_get_rlut();
                socfi_soc = 0;
                socfi_lv_e = voltage;
                goto InitCal;
        }

        socfi_soc = socfi_get_soc_from_vbat_ibat_profile(voltage,
                current, is_charging, deg);

        if (socfi_soc == 0) {
                socfi_lv_e = voltage;
        } else {
                socfi_lv_e = socfi_chk_lut((int16_t *)socfi_ref,
                (int16_t *)socfi_rlut, socfi_soc, VOL2SOC_LUT_SIZE);
        }

InitCal:
        socfi_now_cap = socfi_chk_lut_inv((int16_t *)socf_conf.socf_temp_p,
                (int16_t *)socf_conf.socf_cap_p, deg, socf_conf.socf_temp_num);
        socfi_coulomb = (1000 - socfi_soc) * socfi_now_cap * 3600; // (cap * 3600 * 1000)
}

int16_t socf_process_fg_cal(uint32_t duration_ms, uint16_t voltage,
        int16_t current, bool is_charging, int16_t deg)
{
        if (voltage < SOCF_SYS_MIN_VOLTAGE) {
                return 0;
        }

        if (duration_ms > 1800000) { //30min
                socfi_soc_init_calculation(voltage,
                        current, is_charging, -1, deg);
                return 0;
        }

        return (socfi_get_soc_from_vbat_profile_with_no_temp(voltage,
        duration_ms, is_charging));

}

void socfi_full_charged_notification(void)
{
        if (socfi_soc < SOCF_MAX_SOC_VAL) {
                socfi_soc = SOCF_MAX_SOC_VAL;
                socfi_lv_e = socf_conf.socf_lluts_p[0][VOL2SOC_LUT_SIZE - 1];
        }
}

int16_t socfi_get_soc(void)
{
        if (socfi_soc < 0) {
                return 0;
        } else if (socfi_soc > 1000) {
                return 1000;
        } else {
                return socfi_soc;
        }
}

#ifdef SOCF_ENABLE_DEBUG_COFF
int16_t socfi_get_lv_e(void)
{
        return socfi_lv_e;
}

int32_t socfi_get_curr(void)
{
        return socfi_curr;
}

int16_t socfi_get_vbat(void)
{
        return socfi_vbat;
}

int32_t socfi_get_dv(void)
{
        return socfi_dv;
}

int16_t socfi_get_temp(void)
{
        return socfi_temp;
}

int32_t socfi_get_ddv(void)
{
        return socfi_ddv;
}

int32_t socfi_get_dvolt(void)
{
        return socfi_dvolt;
}
#endif
