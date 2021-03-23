/**
 ****************************************************************************************
 *
 * @file socf_hal.h
 *
 * @brief SOC function
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef SOCF_HAL_H_
#define SOCF_HAL_H_

#if (SOCF_USE_AGED_CYCLE == 1)
/**
 * \brief Get accumulated soc for aged cycle.
 *
 * \details Get accumulated soc recorded for calculation of aged cycle.
 *
 * \return previous soc.
 */
int16_t socf_hal_get_accumulated_soc_from_nand(void);
/**
 * \brief Put accumulated soc to nand.
 *
 * \in accumulated soc
 *
 * \details Put accumulated soc recorded for calculation of aged cycle.
 *
 */
void socf_hal_put_accumulated_soc_to_nand(int16_t soc);
/**
 * \brief Get aged cycle number.
 *
 * \details Get aged cycle number.
 *
 * \return aged cycle number.
 */
int16_t socf_hal_get_aged_cycle_from_nand(void);
/**
 * \brief Put aged cycle number to nand.
 *
 * \in aged cycle number
 *
 * \details Put aged cycle number to nand.
 *
 */
void socf_hal_put_aged_cycle_to_nand(int16_t cycle);
#endif

/**
 * \brief Check the charging state.
 *
 * \details whether the charging or not must be returned.
 *
 * \return whether the charging or not.
 */
bool socf_hal_is_charging(void);

/**
 * \brief Check cc level and state.
 *
 * \details whether cc level and state is proper for aging test.
 *
 * \return whether cc level and state is proper for aging test.
 */
bool socf_hal_is_cc_level(void);

/**
 * \brief Get now time.
 *
 * \details Get current time.
 *
 * \return now time
 */
uint64_t socf_hal_get_time(void);

/**
 * \brief Get duration time.
 *
 * \details Get duration between two times as millie second.
 *
 * \in now time, previous time
 *
 * \return duration
 */
int32_t socf_hal_get_duration(uint64_t now, uint64_t pre);

/**
 * \brief Measure the voltage and current of battery.
 *
 * \in task suspended or not
 *
 * \details Measure the voltage and current through ADC.
 *
 * \return measured voltage (mV)
 */
int16_t socf_hal_measure_vbat(bool task_suspended);

/**
 * \brief Measure the temperatrue.
 *
 * \in whether task is suspended.
 *
 * \details Measure the temperature.
 *
 * \return measured temperature(degree).
 */

int16_t socf_hal_get_degree(bool task_suspended);
/**
 * \brief Stop a timer.
 *
 * \details Stop socf HAL timer.
 *
 */
void socf_hal_stop_timer(void);

/**
 * \brief Start a timer.
 *
 * \in time for timeout
 *
 * \details Start socf HAL timer.
 *
 */
void socf_hal_start_timer(uint32_t time);

/**
 * \brief Create a timer for calculating SOC.
 *
 * \details This timer must be created before socf_init.
 *
 */
void socf_hal_create_timer(void);

/**
 * \brief A function for measuring voltage and current just after sleep.
 *
 * \details This must be called as soon as possible like in PostSleepProcessing after sleep.
 *
 */
void socf_hal_calcaulation_after_wakeup(void);

/**
 * \brief A function for measuring voltage and current just before sleep.
 *
 * \details This must be called as soon as possible like in PreSleepProcessing before sleep.
 *
 */
void socf_hal_calcaulation_before_sleep(void);

/**
 * \brief Create a thread for calculating SOC.
 *
 * \details This thread must be created before socf_init.
 *
 */
void socf_hal_create_thread(void);

/**
 * \brief A function for initializing HAL functions.
 *
 * \details It must be called before HAL functions are used.
 *
 */
void socf_hal_init(void);
#endif /* SOCF_HAL_H_ */
