/**
 ****************************************************************************************
 *
 * @file pxp_reporter_config.h
 *
 * @brief Proximity Reporter configuration
 *
 * Copyright (C) 2016-2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef PXP_REPORTER_CONFIG_H_
#define PXP_REPORTER_CONFIG_H_

/* Name can not be longer than 29 bytes (BLE_SCAN_RSP_LEN_MAX - 2 bytes)*/
#define PX_REPORTER_DEFAULT_NAME        "Dialog DA1469X     "

/* Include Battery Service */
#define PX_REPORTER_INCLUDE_BAS                 ( 0 )

/* LED blinking periods in ms (applicable only in DA1469x) */
#define ALIVE_BLINKING   (1500)
#define SLOW_BLINKING    (750)
#define FAST_BLINKING    (250)


#endif /* PXP_REPORTER_CONFIG_H_ */
