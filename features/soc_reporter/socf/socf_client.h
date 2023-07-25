/**
 ****************************************************************************************
 *
 * @file socf_client.h
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

#ifndef SOCF_CLIENT_H_
#define SOCF_CLIENT_H_

/**
 * \brief Initialization of SOCF function and library.
 *
 * \details This function must be called during boot.
 *
 * \in soc if soc is set from saved value.
 * \in ibat during boot .
 */
void socf_init(int16_t soc, int16_t ibat);

/**
 * \brief Get SOC value.
 *
 * \details Application can get SOC from this function any time.
 *        Return value is from 0(0%) to 100(100%).
 *
 * \return SOC with range from 0 to 100
 *
 */
int16_t socf_get_soc(void);

/**
 * \brief Get an aged percent.
 *
 * \details 0 means no aged. 100 means aged completely.
 *
 * \return percent with range from 0 to 100
 *
 */
int16_t socf_get_aged_percent(void);

/**
 * \brief Get an aged cycle number.
 *
 * \details full cycle number of charging and discharging.
 *
 * \return aged cycle number
 *
 */
int16_t socf_get_aged_cycle_number(void);

/**
 * \brief Function for SOC calculation.
 *
 * \details It is called from thread or before sleep.
 *
 * \in condition for reading voltage and current.
 */
void socf_calculation(bool vbat_available, bool task_suspended);

/**
 * \brief Set the measured voltage.
 *
 * \details the measured value in hal is transferred to client
 *
 * \in voltage as mV
 */
void socf_set_client_voltage(uint16_t voltage_mV);

/**
 * \brief Get the measured voltage.
 *
 * \details the measured value in client is transferred to hal
 *
 * \return voltage as mV
 */
uint16_t socf_get_client_voltage(void);

/**
 * \brief Get the measured current.
 *
 * \details the measured value in client is transferred to hal
 *
 * \return current as mA
 */
int16_t socf_get_client_current(void);

/**
 * \brief Get the previous sample time.
 *
 * \details the previous sample time in client is transferred to hal
 *
 * \return time
 */
uint64_t socf_get_pre_time(void);

/**
 * \brief Set the previous sample time.
 *
 * \details the previous sample time in hal is transferred  to client
 *
 * \in time
 */
void socf_set_pre_time(uint64_t time);
#endif /* SOCF_CLIENT_H_ */
