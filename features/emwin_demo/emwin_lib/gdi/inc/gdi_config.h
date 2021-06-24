/**
 * \addtogroup UI
 * \{
 * \addtogroup GDI
 *
 * \brief GDI configuration
 * \{
 */
/**
 ****************************************************************************************
 *
 * @file gdi_config.h
 *
 * @brief Configures GDI parameters
 *
 * Copyright (C) 2019 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#ifndef GDI_CONFIG_H_
#define GDI_CONFIG_H_

#if dg_configUSE_HM80160A090
#include "hm80160a090.h"
#elif dg_configUSE_E1394AA65A
#include "e1394aa65a.h"
#elif dg_configUSE_LPM012M134B
#include "lpm012m134b.h"
#elif dg_configUSE_NHD43480272EFASXN
#include "nhd43480272efasxn.h"
#elif dg_configUSE_LS013B7DH06
#include "ls013b7dh06.h"
#elif dg_configUSE_LS013B7DH03
#include "ls013b7dh03.h"
#elif dg_configUSE_DT280QV10CT
#include "dt280qv10ct.h"
#elif dg_configUSE_T1D3BP006
#include "t1d3bp006.h"
#elif dg_configUSE_T1D54BP002
#include "t1d54bp002.h"
#elif dg_configUSE_PSP27801
#include "psp27801.h"
#elif dg_configUSE_MCT024L6W240320PML
#include "mct024l6w240320pml.h"
#elif dg_configUSE_LPM013M091A
#include "lpm013m091a.h"
#endif

#define GDI_SINGLE_FB_NUM       (2)
#define GDI_GUI_HEAP_SIZE       (0x2500)

#define USE_COLOR_FORMAT        (CF_NATIVE_AUTO)

/*
 * \note Printing log messages will adversely affect system's performance
 */
#define GDI_CONSOLE_LOG         (0)

#endif /* GDI_CONFIG_H_ */

/**
 * \}
 * \}
 */
