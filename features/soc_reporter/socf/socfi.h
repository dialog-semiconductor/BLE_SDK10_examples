/**
 ****************************************************************************************
 *
 * @file socfi.h
 *
 * @brief SOC function
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef SOCFI_H_
#define SOCFI_H_

#define VOL2SOC_LUT_SIZE        21
#define SOCF_M_CAL_NUM          6

typedef struct {
        /* the number of temperature table */
        int16_t socf_temp_num;
        /* the capacitance of a battery (mAh) */
        const int16_t *socf_temp_p;
        /* the capacitance of a battery (mAh) */
        const int16_t *socf_cap_p;
        /* low voltage tables */
        const int16_t(*socf_lluts_p)[VOL2SOC_LUT_SIZE];
        /* high voltage tables */
        const int16_t(*socf_hluts_p)[VOL2SOC_LUT_SIZE];
        /* charge voltage tables */
        const int16_t(*socf_cluts_p)[VOL2SOC_LUT_SIZE];
        /* the charging current (mA) */
        const int16_t *socf_chg_cc;
        /* the EOC current (mA) */
        int16_t socf_chg_eoc;
        /* the charging voltage (mV) */
        int16_t socf_chg_cv;
        /* the high current (mA) at battery profile*/
        int16_t socf_dis_high;
        /* the low current (mA) at battery profile */
        int16_t socf_dis_low;
        /* current compensation values */
        const int16_t (*socf_cur_comp)[SOCF_M_CAL_NUM];
        /* array number for 25C */
        int16_t socf_25c_num;
        /* shunt resistor (mOHM)*/
        int16_t socf_shunt;
        /* cutoff voltage*/
        int16_t cutoff_voltage;
} socf_parameter_t;

/**
 * \brief Get SOC value.
 *
 * \details Application can get SOC from this function any time.
 *        Return value is from 0(0%) to 1000(100.0%).
 *        So proper SOC(0 ~ 100%) for BAS profile would be (soc_get_soc() + 5) / 10.
 *
 * \return SOC with range from 0 to 1000
 *
 */
int16_t socfi_get_soc(void);

/**
 * \brief Calculate SOC value.
 *
 * \param [in] duration between calculation.
 * \param [in] voltage of battery.
 * \param [in] current of system.
 * \param [in] charging state.
 * \param [in] temperature of battery.
 *
 * \return changed SOC by temperature change
 *
 */
int16_t socf_process_fg_cal(uint32_t duration, uint16_t voltage, int16_t current,
        bool is_charging, int16_t deg);

/**
 * \brief Calculate initial SOC value.
 *
 * \param [in] voltage of battery.
 * \param [in] current of system.
 * \param [in] charging state.
 * \param [in] initial SOC value if it is known.
 * \param [in] temperature of battery.
 *
 */
void socfi_soc_init_calculation(int16_t voltage, int16_t current,
        bool is_charging, int16_t soc, int16_t deg);

/**
 * \brief Initialize socfi.
 *
 */
void socfi_init(void);

/**
 * \brief Give a notification of full charging from charger driver to socf.
 *
 * \details The SOC is set to 100% forcibly. This function can be used according to use requirement.
 *
 */
void socfi_full_charged_notification(void);

/**
 * \brief Get a aged percent of the battery.
 *
 * \param [in] time duration during charging.
 * \param [in] soc interval during charging.
 *
 * \return aged percent.
 */
int16_t socfi_get_aged_percent(int32_t time_duration, int16_t soc_interval);

/**
 * \brief Get time(sec) to full charged state.
 *
 * \details Application can get time to charged state during charging state.
 *
 * \return time(sec).
 */
int32_t socfi_soc_get_sec_to_charged(void);
#endif /* SOCFI_H_ */
