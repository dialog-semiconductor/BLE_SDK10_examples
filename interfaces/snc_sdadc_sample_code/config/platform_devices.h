/**
 ****************************************************************************************
 *
 * @file platform_devices.h
 *
 * @brief Configuration of devices connected to board
 *
 * Copyright (C) 2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef PLATFORM_DEVICES_H_
#define PLATFORM_DEVICES_H_

#include "ad_gpadc.h"
#include <ad_sdadc.h>

#ifdef __cplusplus
extern "C" {
#endif

#if (dg_configGPADC_ADAPTER == 1)
/*
 * Define sources connected to GPADC
 */
const ad_gpadc_controller_conf_t GPADC_VBAT;

#endif /* dg_configGPADC_ADAPTER */

#if (dg_configSDADC_ADAPTER == 1)
/*
 * Define sources connected to SDADC
 */
const ad_sdadc_controller_conf_t SDADC_VBAT;

#endif /* dg_configSDADC_ADAPTER */

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_DEVICES_H_ */
