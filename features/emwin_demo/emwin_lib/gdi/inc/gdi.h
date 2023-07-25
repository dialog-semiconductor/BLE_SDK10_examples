/**
 * \addtogroup UI
 * \{
 * \addtogroup GDI
 *
 * \brief Graphics Device Interface
 * \{
 */
/**
 ****************************************************************************************
 *
 * @file gdi.h
 *
 * @brief Graphics Device Interface
 *
 * Copyright (C) 2019-2021 Renesas Electronics Corporation and/or its affiliates
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

#ifndef GDI_H_
#define GDI_H_

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "osal.h"
#include "ad_lcdc.h"

/**********************************************************************
 *
 *       Types
 *
 **********************************************************************
 */
typedef uint16_t gdi_coord_t;

typedef enum {
        GDI_FORMAT_L1,
        GDI_FORMAT_L4,
        GDI_FORMAT_L8,
        GDI_FORMAT_RGB332,
        GDI_FORMAT_RGB565,
        GDI_FORMAT_ABGR8888,
        GDI_FORMAT_RGBA8888,
} gdi_color_fmt_t;

/**
 * \brief Touch event data
 */
typedef struct {
        gdi_coord_t x;      /**< X position of the touch event in display's coordinates */
        gdi_coord_t y;      /**< Y position of the touch event in display's coordinates */
        uint8_t pressed;    /**< Designates whether the display is pressed or not */
} gdi_touch_data_t;

/**
 * Callback function for the asynchronous update of the LCD
 *
 * \param[in] user_data         User data as provided with the update function
 *
 * \sa gdi_display_update_async()
 */
typedef void (*draw_callback)(void *user_data);

/**
 * Callback function for storing touch events into the graphics buffers
 *
 * \param[in] touch_data      Pointer to touch data
 *
 * \sa gdi_set_callback_store_touch()
 */
typedef void (*store_touch_callback)(gdi_touch_data_t *touch_data);

/**
 * \brief GDI type definition
 */
typedef struct {
        int32_t stride;                                  /**< Stride of the frame buffer in bytes */
        size_t single_buff_sz;                           /**< Size of a single frame buffer in bytes  */
        uint8_t active_buf;                              /**< Active frame buffer */
        uint8_t bufs_num;                                /**< Number of buffers available */
        gdi_coord_t width;                               /**< Width of the LCD in pixels */
        gdi_coord_t height;                              /**< Height of the LCD in pixels */
        bool display_powered;                            /**< Display power state */
        bool display_enabled;                            /**< Display state */
        bool continuous_mode_enable;                     /**< Continuous mode state */
        OS_MUTEX block_mutex_display_status;             /**< MUTEX for display status */
        ad_lcdc_handle_t display_h;                      /**< Display handle */
        void *touch_h;                                   /**< Touch controller handle */
        gdi_color_fmt_t color_format;                    /**< Color format */
        hw_lcdc_frame_t active_frame;                    /**< Active frame dimensions */
        hw_lcdc_layer_t active_layer;                    /**< Active layer */
        OS_MUTEX block_mutex_multiplex_display_touch;    /**< MUTEX for display operations used when the display and touch controllers are multiplexed */
        OS_EVENT dma_event;                              /**< Event for DMA transactions */
        OS_EVENT draw_smphr;                             /**< Semaphore for asynchronous drawing operations */
        OS_EVENT draw_event;                             /**< Event for synchronous/asynchronous drawing operations */
        draw_callback draw_cb;
        store_touch_callback store_touch_cb;
        void *user_data;
        uint8_t *buffer[];                               /**< Frame buffer pointers */
} gdi_t;

#include "gdi_config.h"

/**********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#ifndef GDI_TOUCH_ENABLE
#define GDI_TOUCH_ENABLE                (0)
#endif

#define CF_NATIVE_AUTO                  (0)
#define CF_NATIVE_RGB332                (3)
#define CF_NATIVE_RGB565                (4)
#define CF_NATIVE_RGBA8888              (5)

#ifndef USE_COLOR_FORMAT
#define USE_COLOR_FORMAT                (CF_NATIVE_AUTO)
#endif

#define GDI_TOUCH_INTERFACE_I2C         (0)
#define GDI_TOUCH_INTERFACE_SPI         (1)

#if USE_COLOR_FORMAT == CF_NATIVE_AUTO
#undef USE_COLOR_FORMAT
#define ___GDI_EXTEND_COLOR(x)          _GDI_##x
#define __GDI_EXTEND_COLOR(x)           ___GDI_EXTEND_COLOR x
#define _GDI_EXTEND_COLOR(x)            __GDI_EXTEND_COLOR(x)

#define _GDI_HW_LCDC_OCM_8RGB111_1      (0)
#define _GDI_HW_LCDC_OCM_8RGB111_2      (1)
#define _GDI_HW_LCDC_OCM_RGB111         (2)
#define _GDI_HW_LCDC_OCM_L1             (3)
#define _GDI_HW_LCDC_OCM_8RGB332        (4)
#define _GDI_HW_LCDC_OCM_8RGB444        (5)
#define _GDI_HW_LCDC_OCM_8RGB565        (6)
#define _GDI_HW_LCDC_OCM_8RGB666        (7)
#define _GDI_HW_LCDC_OCM_8RGB888        (8)
#define _GDI_HW_LCDC_OCM_RGB222         (9)

#if _GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_L1))
#define USE_COLOR_FORMAT                (CF_NATIVE_RGB332)
#elif (_GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB111_1))) \
        || (_GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB111_2))) \
        || (_GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_RGB111))) \
        || (_GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_RGB222))) \
        || (_GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB332)))
#define USE_COLOR_FORMAT                (CF_NATIVE_RGB332)
#elif _GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB444)) \
        || _GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB565))
#define USE_COLOR_FORMAT                (CF_NATIVE_RGB565)
#elif _GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB666)) \
        || _GDI_EXTEND_COLOR(GDI_DISP_COLOR) == _GDI_EXTEND_COLOR((HW_LCDC_OCM_8RGB888))
#define USE_COLOR_FORMAT                (CF_NATIVE_RGBA8888)
#endif /* GDI_DISP_COLOR */
#endif /* USE_COLOR_FORMAT */

#ifndef GDI_SINGLE_FB_NUM
#define GDI_SINGLE_FB_NUM               (2)
#endif

#ifndef GDI_GUI_HEAP_SIZE
#define GDI_GUI_HEAP_SIZE               (0)
#endif

#if USE_COLOR_FORMAT == CF_NATIVE_RGB332

#define GDI_COLOR_BYTES                 (1)

#define GDI_RED_POS                     (5)
#define GDI_GREEN_POS                   (2)
#define GDI_BLUE_POS                    (0)
#define GDI_ALPHA_POS                   (0)

#define GDI_RED_WIDTH                   (3)
#define GDI_GREEN_WIDTH                 (3)
#define GDI_BLUE_WIDTH                  (2)
#define GDI_ALPHA_WIDTH                 (0)

#elif USE_COLOR_FORMAT == CF_NATIVE_RGB565

#define GDI_COLOR_BYTES                 (2)

#define GDI_RED_POS                     (11)
#define GDI_GREEN_POS                   (5)
#define GDI_BLUE_POS                    (0)
#define GDI_ALPHA_POS                   (0)

#define GDI_RED_WIDTH                   (5)
#define GDI_GREEN_WIDTH                 (6)
#define GDI_BLUE_WIDTH                  (5)
#define GDI_ALPHA_WIDTH                 (0)

#elif USE_COLOR_FORMAT == CF_NATIVE_RGBA8888

#define GDI_COLOR_BYTES                 (4)

#define GDI_RED_POS                     (0)
#define GDI_GREEN_POS                   (8)
#define GDI_BLUE_POS                    (16)
#define GDI_ALPHA_POS                   (24)

#define GDI_RED_WIDTH                   (8)
#define GDI_GREEN_WIDTH                 (8)
#define GDI_BLUE_WIDTH                  (8)
#define GDI_ALPHA_WIDTH                 (8)

#endif /* USE_COLOR_FORMAT */

#define GDI_FIELD(c)                    (((1 << GDI_## c ##_WIDTH) - 1) << GDI_## c ##_POS)

#define GDI_RED_FIELD                   (GDI_FIELD(RED))
#define GDI_GREEN_FIELD                 (GDI_FIELD(GREEN))
#define GDI_BLUE_FIELD                  (GDI_FIELD(BLUE))
#define GDI_COLOR_FIELD                 (GDI_FIELD(RED) | GDI_FIELD(GREEN) | GDI_FIELD(BLUE))
#define GDI_ALPHA_FIELD                 (GDI_FIELD(ALPHA))

/**
 * \brief GDI initialization
 *
 * Initializes the GDI instance, allocate memory and set default background color
 */
void gdi_init(void);

/**
 * \brief GDI task notification
 *
 * Sends a notification to the GDI task. This routine should be called from within an ISR.
 */
void gdi_touch_event(void);

/**
 * \brief Set callback function used to store touch events into the graphics buffers.
 *
 * \param[in] cb  User-defined callback function
 */
void gdi_set_callback_store_touch(store_touch_callback cb);

/**
 * \brief Power on the display
 */
void gdi_display_power_on(void);

/**
 * \brief Power off the display
 */
void gdi_display_power_off(void);

/**
 * \brief Checks if display is powered.
 *
 * \return True if display is powered, false otherwise.
 */
bool gdi_display_is_powered(void);

/**
 * \brief Enable the display
 */
void gdi_display_enable(void);

/**
 * \brief Disable the display
 */
void gdi_display_disable(void);

/**
 * \brief Checks if display is enabled.
 *
 * \return True if display is enabled, false otherwise.
 */
bool gdi_display_is_enabled(void);

/**
 * \brief Update the display (synchronously)
 *
 * Move data from the active frame buffer to the display's internal memory.
 */
void gdi_display_update(void);

/**
 * \brief Update the display (asynchronously)
 *
 * Move data from the active frame buffer to the display's internal memory. User can register
 * their own callback function and pass their own data.
 *
 * \param[in] cb          User-defined callback function to be called once the frame update is done.
 *
 * \param[in] user_data   User-defined data to be passed in the registered callback function.
 */
void gdi_display_update_async(draw_callback cb, void *user_data);

/**
 * \brief Set area of the LCD that will be updated
 *
 * \param[in] x0     First column of the area to be updated
 * \param[in] y0     First row of the area to be updated
 * \param[in] x1     Last column of the area to be updated
 * \param[in] y1     Last row of the area to be updated
 */
void gdi_set_partial_update_area(gdi_coord_t x0, gdi_coord_t y0, gdi_coord_t x1, gdi_coord_t y1);

/**
 * \brief Clear display
 *
 * Clear the display
 */
void gdi_display_clear(void);

/**
 * \brief Select the next available frame buffer
 *
 * \return Number of the currently selected frame buffer
 */
uint8_t gdi_set_next_frame_buffer(void);

/**
 * \brief Get the number of the current frame buffer
 *
 * \return Number of the currently selected frame buffer
 */
uint8_t gdi_get_current_frame_buffer(void);

/**
 * \brief Set active frame buffer
 *
 * \param [in] frame            number of frame buffer
 */
void gdi_set_frame_buffer(uint8_t frame);

/**
 * \brief Get the address of the assigned frame buffers area
 */
void *gdi_get_frame_buffer_addr(void);

/**
 * \brief Get the starting address of assigned heap area
 */
void *gdi_get_gui_heap_addr(void);

/**
 * \brief Copies the contents of the \ref src buffer to the \ref dst buffer.
 *
 * \param[in] dst   Destination buffer
 * \param[in] src   Source buffer
 */
void gdi_buffer_memcpy(uint8_t dst, uint8_t src);

#endif /* GDI_H_ */

/**
 * \}
 * \}
 */
