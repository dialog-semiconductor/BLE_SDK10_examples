/**
 ****************************************************************************************
 *
 * @file socf_config.h
 *
 * @brief SOC function
 *
 * Copyright (C) 2015-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef SOCF_CONFIG_H_
#define SOCF_CONFIG_H_

#define SOCF_USE_SNC            ( 0 )
#define SOCF_USE_OFFSET_COMP    ( 1 )
#define SOCF_USE_AGED_CYCLE     ( 1 )
#define SOCF_USE_AGED_RATE      ( 0 )

/* TIMER time for SOC calculation at active state */
#define SOCF_HAL_TIMER_TIME     1500
/* Sampling interval for SOC calculation (msec) */
#define SOCF_SAMPLING_TIME      1000
/* Current at the point for calculating first SOC from boot (mA)*/
#define SOCF_IBAT_BOOT          -2


/* the number of LUTs at each temperature */
#define SOCF_TEMP_NUM           1

/* When SDADC is used for the voltage of battery */
#define USE_SDADC_FOR_VBAT      ( 0 )

#if (SOCF_USE_SNC == 1) && (USE_SDADC_FOR_VBAT == 1)
#error "SDADC is not supported when SNC is used"
#endif

#if (SOCF_HAL_TIMER_TIME < SOCF_SAMPLING_TIME)
#error "Sampling time must be smaller than the period of HAL timer"
#endif
#endif
