/**
 * \addtogroup UI
 * \{
 * \addtogroup GDI
 * \{
 */
/**
 ****************************************************************************************
 *
 * @file gdi.c
 *
 * @brief Basic graphic functions implementation
 *
 * Copyright (C) 2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "gdi.h"
#include "osal.h"
#include "sys_watchdog.h"
#include "sys_power_mgr.h"
#if dg_configUSE_HW_QSPI2
#include "hw_qspi.h"
#include "qspi_automode.h"
#endif
#include "hw_dma.h"
#include "resmgmt.h"
#include "hw_led.h"
#include "sys_timer.h"
#include "ad_i2c.h"
#include "ad_spi.h"

/**********************************************************************
 *
 *       Defines
 *
 **********************************************************************
 */
#define DEV_DRAW_TIMEOUT_MS                     (200)
#define DEV_CLOSE_TIMEOUT_MS                    (10)

#ifndef GDI_TOUCH_INTERFACE
#define GDI_TOUCH_INTERFACE                     GDI_TOUCH_INTERFACE_I2C
#endif

#ifndef GDI_CONSOLE_LOG
#define GDI_CONSOLE_LOG                         (0)
#endif

#ifndef GDI_FB_USE_QSPI_RAM
#define GDI_FB_USE_QSPI_RAM                     (dg_configUSE_HW_QSPI2)
#endif

#ifndef GDI_HEAP_USE_QSPI_RAM
#define GDI_HEAP_USE_QSPI_RAM                   (0)
#endif

#ifndef GDI_QSPI_RAM_OFFSET
#define GDI_QSPI_RAM_OFFSET                     (0)
#endif

#ifndef HW_LCDC_FIFO_PREFETCH_LVL
#define HW_LCDC_FIFO_PREFETCH_LVL               (HW_LCDC_FIFO_PREFETCH_LVL_44_BYTES)
#endif

#ifndef GDI_USE_OS_TIMER
#define GDI_USE_OS_TIMER                        (0)
#endif

#ifndef GDI_PWMLED_BACKLIGHT
#define GDI_PWMLED_BACKLIGHT                    (0)
#endif

#if GDI_PWMLED_BACKLIGHT
#ifndef GDI_PWMLED_DUTY_CYCLE_1
#define GDI_PWMLED_DUTY_CYCLE_1                 (0)
#endif

#ifndef GDI_PWMLED_DUTY_CYCLE_2
#define GDI_PWMLED_DUTY_CYCLE_2                 (0)
#endif
#endif /* GDI_PWMLED_BACKLIGHT */

#ifndef GDI_DISP_OFFSETX
#define GDI_DISP_OFFSETX                        (0)
#endif

#ifndef GDI_DISP_OFFSETY
#define GDI_DISP_OFFSETY                        (0)
#endif

#ifndef GDI_USE_CONTINUOUS_MODE
#define GDI_USE_CONTINUOUS_MODE                 (0)
#endif

#ifndef GDI_MULTIPLEX_TOUCH_DISPLAY
#define GDI_MULTIPLEX_TOUCH_DISPLAY             (0)
#endif

/* Notification bit-masks */
#define DEV_DRAW_ASYNC_EVT                      (1 << 0)
#define DEV_DRAW_ASYNC_CMPL_EVT                 (1 << 1)
#define DEV_TOUCH_EVT                           (1 << 2)

#if GDI_USE_OS_TIMER
#define GDI_GET_TIME                            OS_GET_TICK_COUNT
#define GDI_TIME_MULTIPLIER                     1000
#define GDI_TIME_DIVIDER                        1
typedef OS_TICK_TIME GDI_TICK_TIME;
#else
#define GDI_GET_TIME                            get_sys_timestamp
#define GDI_TIME_MULTIPLIER                     1000000UL
#define GDI_TIME_DIVIDER                        configSYSTICK_CLOCK_HZ
typedef uint32_t GDI_TICK_TIME;
#endif

#if GDI_USE_CONTINUOUS_MODE
#define DISPLAY_MUTEX_GET() OS_MUTEX_GET(gdi->block_mutex_display_status, OS_MUTEX_FOREVER)
#else
#define DISPLAY_MUTEX_GET()
#endif

#if GDI_USE_CONTINUOUS_MODE
#define DISPLAY_MUTEX_PUT() OS_MUTEX_PUT(gdi->block_mutex_display_status)
#else
#define DISPLAY_MUTEX_PUT()
#endif

#if (dg_configUSE_HW_QSPI2 == 0) && (GDI_FB_USE_QSPI_RAM != 0 || GDI_HEAP_USE_QSPI_RAM != 0)
#error "QSPI RAM must be enabled in order to place frame buffer or heap!"
#endif

#if !GDI_FB_USE_QSPI_RAM
PRIVILEGED_DATA static uint8_t frame_buffer[GDI_DISP_RESX * GDI_DISP_RESY * GDI_COLOR_BYTES * GDI_SINGLE_FB_NUM];
#endif

#if !GDI_HEAP_USE_QSPI_RAM
PRIVILEGED_DATA static uint32_t gui_heap_area[GDI_GUI_HEAP_SIZE / sizeof(uint32_t)];
#endif

PRIVILEGED_DATA static gdi_t *gdi;

PRIVILEGED_DATA static OS_TASK task_h;

#if GDI_CONSOLE_LOG
PRIVILEGED_DATA static GDI_TICK_TIME frame_timetstamp, time_start;
#endif

#if GDI_PWMLED_BACKLIGHT
__STATIC_INLINE void hw_led_set_led1_load(uint8_t level)
{
        REG_SETF(PWMLED, PWMLED_CTRL_REG, LED1_LOAD_SEL, level);
}

__STATIC_INLINE void hw_led_set_led2_load(uint8_t level)
{
        REG_SETF(PWMLED, PWMLED_CTRL_REG, LED2_LOAD_SEL, level);
}

void pwmled_backlight_init(uint8_t duty_cycle1, uint8_t duty_cycle2)
{
        bool led1_enable = duty_cycle1 > 0 ? true : false;
        bool led2_enable = duty_cycle2 > 0 ? true : false;

        hw_led_pwm_duty_cycle_t cycle = {
                .hw_led_pwm_start = 0,
        };

        if (led1_enable || led2_enable) {
                hw_led_pwm_set_frequency(100);
        }

        if (led1_enable) {
                duty_cycle1 = MIN(duty_cycle1, 100);
                cycle.hw_led_pwm_end = duty_cycle1;
                hw_led1_pwm_set_duty_cycle(&cycle);
                hw_led_set_led1_load(7);
        }
        hw_led_enable_led1(led1_enable);

        if (led2_enable) {
                duty_cycle2 = MIN(duty_cycle2, 100);
                cycle.hw_led_pwm_end = duty_cycle2;
                hw_led2_pwm_set_duty_cycle(&cycle);
                hw_led_set_led2_load(7);
        }
        hw_led_enable_led2(led2_enable);
}

void pwmled_backlight_set(bool enable)
{
        hw_led_set_pwm_state(enable);
        if (enable) {
                pm_sleep_mode_request(pm_mode_idle);
        } else {
                pm_sleep_mode_release(pm_mode_idle);
        }
}

#endif /* GDI_PWMLED_BACKLIGHT */

uint64_t get_sys_timestamp(void)
{
        return (in_interrupt() ? sys_timer_get_uptime_ticks_fromISR() : sys_timer_get_uptime_ticks());
}

#if GDI_CONSOLE_LOG
static void console_log(void)
{
        int fps;
        int frame_duration_us, transfer_duration_us;
        GDI_TICK_TIME time_stop;

        time_stop = GDI_GET_TIME();
        frame_duration_us = (time_stop - frame_timetstamp) * GDI_TIME_MULTIPLIER / GDI_TIME_DIVIDER;
        transfer_duration_us = (time_stop - time_start) * GDI_TIME_MULTIPLIER / GDI_TIME_DIVIDER;
        fps = 10000000UL / frame_duration_us;
        printf("FPS: %d.%d (frame: %d.%d ms, transfer: %d.%d ms)\r\n", fps / 10, fps % 10, (frame_duration_us / 1000),
                                                                                           (frame_duration_us / 100) % 10,
                                                                                           (transfer_duration_us / 1000),
                                                                                           (transfer_duration_us / 100) % 10);
        frame_timetstamp = time_stop;
}
#endif

static void dev_open_display(void)
{
        if (gdi->continuous_mode_enable) {
                /* The device must already be opened */
                OS_ASSERT(gdi->display_h != NULL);
                return;
        }

#if GDI_MULTIPLEX_TOUCH_DISPLAY
        OS_MUTEX_GET(gdi->block_mutex_multiplex_display_touch, OS_MUTEX_FOREVER);
#endif
        ad_lcdc_handle_t dev = ad_lcdc_open(GDI_LCDC_CONFIG);
        OS_ASSERT(dev != NULL);
        gdi->display_h = dev;
}

static void dev_close_display(void)
{
        /* If continuous mode is enabled just return. */
        if (gdi->continuous_mode_enable) {
                return;
        }

        OS_TICK_TIME timeout = OS_GET_TICK_COUNT() + OS_MS_2_TICKS(DEV_CLOSE_TIMEOUT_MS);
        while(ad_lcdc_close(gdi->display_h, false) != AD_LCDC_ERROR_NONE) {
                if (timeout <= OS_GET_TICK_COUNT()) {
                        ad_lcdc_close(gdi->display_h, true);
                        break;
                }
                OS_DELAY(1);
        }
#if GDI_MULTIPLEX_TOUCH_DISPLAY
        OS_MUTEX_PUT(gdi->block_mutex_multiplex_display_touch);
#endif
}

#if GDI_TOUCH_ENABLE
static void dev_open_touch(void)
{
#if GDI_MULTIPLEX_TOUCH_DISPLAY
        OS_MUTEX_GET(gdi->block_mutex_multiplex_display_touch, OS_MUTEX_FOREVER);
#endif
#if (GDI_TOUCH_INTERFACE == GDI_TOUCH_INTERFACE_I2C)
     void * dev = ad_i2c_open(GDI_TOUCH_CONFIG);
#elif (GDI_TOUCH_INTERFACE == GDI_TOUCH_INTERFACE_SPI)
     void * dev = ad_spi_open(GDI_TOUCH_CONFIG);
#endif
     OS_ASSERT(dev != NULL);
     gdi->touch_h = dev;
}

static void dev_close_touch(void)
{
        OS_TICK_TIME timeout = OS_GET_TICK_COUNT() + OS_MS_2_TICKS(DEV_CLOSE_TIMEOUT_MS);

#if (GDI_TOUCH_INTERFACE == GDI_TOUCH_INTERFACE_I2C)
        while (ad_i2c_close(gdi->touch_h, false) != AD_I2C_ERROR_NONE) {
                if (timeout <= OS_GET_TICK_COUNT()) {
                        ad_i2c_close(gdi->touch_h, true);
                        break;
                }
                OS_DELAY(1);
        }
#elif (GDI_TOUCH_INTERFACE == GDI_TOUCH_INTERFACE_SPI)
        while (ad_spi_close(gdi->touch_h, false) != AD_SPI_ERROR_NONE) {
                if (timeout <= OS_GET_TICK_COUNT()) {
                        ad_spi_close(gdi->touch_h, true);
                        break;
                }
                OS_DELAY(1);
        }
#endif
#if GDI_MULTIPLEX_TOUCH_DISPLAY
        OS_MUTEX_PUT(gdi->block_mutex_multiplex_display_touch);
#endif
}
#endif /* GDI_TOUCH_ENABLE */

static void screen_setup(void)
{
        dev_open_display();
        ad_lcdc_execute_cmds(gdi->display_h, screen_init_cmds, sizeof(screen_init_cmds));
        ad_lcdc_set_display_offset(gdi->display_h, GDI_DISP_OFFSETX, GDI_DISP_OFFSETY);
        dev_close_display();
}

static void screen_power_on(void)
{
        dev_open_display();
        ad_lcdc_execute_cmds(gdi->display_h, screen_power_on_cmds, sizeof(screen_power_on_cmds));
        dev_close_display();
}

static void screen_enable(void)
{
#if GDI_PWMLED_BACKLIGHT
        pwmled_backlight_set(true);
#endif /* GDI_PWMLED_BACKLIGHT */
        dev_open_display();
        ad_lcdc_execute_cmds(gdi->display_h, screen_enable_cmds, sizeof(screen_enable_cmds));
        dev_close_display();
}

static void screen_disable(void)
{
        dev_open_display();
        ad_lcdc_execute_cmds(gdi->display_h, screen_disable_cmds, sizeof(screen_disable_cmds));
        dev_close_display();
#if GDI_PWMLED_BACKLIGHT
        pwmled_backlight_set(false);
#endif /* GDI_PWMLED_BACKLIGHT */
}

static void screen_power_off(void)
{
        dev_open_display();
        ad_lcdc_execute_cmds(gdi->display_h, screen_power_off_cmds, sizeof(screen_power_off_cmds));
        dev_close_display();
}

static void screen_clear(void)
{
        dev_open_display();
        ad_lcdc_execute_cmds(gdi->display_h, screen_clear_cmds, sizeof(screen_clear_cmds));
        dev_close_display();
}

static void frame_update_cb(void *cb_data)
{
        gdi_t *data = (gdi_t *)cb_data;
        OS_EVENT_SIGNAL_FROM_ISR(data->draw_event);
}

static void frame_update_async_cb(void *cb_data)
{
        OS_TIMER timer = (OS_TIMER)cb_data;
        OS_TIMER_STOP_FROM_ISR(timer);

        OS_TASK_NOTIFY_FROM_ISR(task_h, DEV_DRAW_ASYNC_CMPL_EVT, OS_NOTIFY_SET_BITS);
}

#if GDI_USE_CONTINUOUS_MODE
static void dev_continuous_mode_enable(void)
{
        DISPLAY_MUTEX_GET();
        if (!gdi->continuous_mode_enable) {
                dev_open_display();
                /* Set the flag so that, the display handle is no longer updated */
                gdi->continuous_mode_enable = true;
                ad_lcdc_continuous_update_start(gdi->display_h, frame_update_cb, gdi);
        }
        DISPLAY_MUTEX_PUT();
}

static void dev_continuous_mode_disable(void)
{
      DISPLAY_MUTEX_GET();
      if (gdi->continuous_mode_enable) {
              ad_lcdc_continuous_update_stop(gdi->display_h);
              /* Zero the flag so that, the device can close */
              gdi->continuous_mode_enable = false;
              dev_close_display();
      }
      DISPLAY_MUTEX_PUT();
}
#endif /* GDI_USE_CONTINUOUS_MODE */

static void dev_draw(gdi_t *gdi)
{
        if (gdi->continuous_mode_enable) {
                /* The new layer settings will be applied once the current frame cycle is complete */
                ad_lcdc_continuous_update_draw(gdi->display_h, &gdi->active_layer);
                /* Event might already be sent multiple times by LCDC */
                OS_EVENT_CHECK(gdi->draw_event);
                /* Wait for the start of a new frame cycle */
                if (OS_EVENT_SIGNALED != OS_EVENT_WAIT(gdi->draw_event, OS_MS_2_TICKS(DEV_DRAW_TIMEOUT_MS))) {
#if GDI_CONSOLE_LOG
                        printf("FRAME ERROR - ");
#endif
                 }
                /* Wait for the completion of the previously tracked frame cycle */
                if (OS_EVENT_SIGNALED != OS_EVENT_WAIT(gdi->draw_event, OS_MS_2_TICKS(DEV_DRAW_TIMEOUT_MS))) {
#if GDI_CONSOLE_LOG
                        printf("FRAME ERROR - ");
#endif
                 }
        }
        else {
#if GDI_CONSOLE_LOG
                        time_start = GDI_GET_TIME();
#endif
#ifdef GDI_DISP_PRE_DRAW
                        GDI_DISP_PRE_DRAW(gdi, gdi->display_h, &gdi->active_layer);
#endif /* GDI_DISP_PRE_DRAW */
                        ad_lcdc_draw_screen_async(gdi->display_h, &gdi->active_layer, frame_update_cb, gdi);
                        if (OS_EVENT_SIGNALED != OS_EVENT_WAIT(gdi->draw_event, OS_MS_2_TICKS(DEV_DRAW_TIMEOUT_MS))) {
#if GDI_CONSOLE_LOG
                                printf("FRAME ERROR - ");
#endif
                        }
#ifdef GDI_DISP_POST_DRAW
                        GDI_DISP_POST_DRAW(gdi, gdi->display_h, &gdi->active_layer);
#endif /* GDI_DISP_POST_DRAW */
#if GDI_CONSOLE_LOG
                        console_log();
#endif /* GDI_CONSOLE_LOG */
          }
}

static void dev_set_partial_update(void)
{
        ad_lcdc_set_partial_update(gdi->display_h, &gdi->active_frame);
#ifdef GDI_DISP_SET_PARTIAL_UPDATE
        GDI_DISP_SET_PARTIAL_UPDATE(gdi, gdi->display_h, &gdi->active_frame);
#endif
}

static HW_LCDC_LAYER_COLOR_MODE gdi_to_layer_color_format(gdi_color_fmt_t format)
{
        switch (format) {
        case GDI_FORMAT_RGB332:   return HW_LCDC_LCM_RGB332;
        case GDI_FORMAT_RGB565:   return HW_LCDC_LCM_RGB565;
        case GDI_FORMAT_RGBA8888: return HW_LCDC_LCM_RGBA8888;
        default:                  ASSERT_ERROR(0);
        }
}

static void dev_draw_async(draw_callback cb, void *user_data)
{
        OS_EVENT_WAIT(gdi->draw_smphr, OS_EVENT_FOREVER);
        gdi->draw_cb = cb;
        gdi->user_data = user_data;
        gdi->active_layer.baseaddr = (uint32_t)gdi->buffer[gdi->active_buf];
        gdi->active_layer.dma_prefetch_lvl = HW_LCDC_FIFO_PREFETCH_LVL;
        gdi->active_layer.format = gdi_to_layer_color_format(gdi->color_format);
        gdi->active_layer.resx = gdi->width;
        gdi->active_layer.resy = gdi->height;
        gdi->active_layer.startx = 0;
        gdi->active_layer.starty = 0;
        gdi->active_layer.stride = gdi->stride;

        OS_TASK_NOTIFY(task_h, DEV_DRAW_ASYNC_EVT, OS_NOTIFY_SET_BITS);
}

static uint32_t calc_stride(gdi_coord_t width)
{
        uint32_t stride = hw_lcdc_stride_size(gdi_to_layer_color_format(gdi->color_format), width);
        return (stride + 0x3) & (~0x3);
}

#if GDI_TOUCH_ENABLE
static void dev_notify_touch(void)
{
        /* Make sure that GDI is already up and running */
        if (task_h) {
                in_interrupt() ?
                                OS_TASK_NOTIFY_FROM_ISR(task_h, DEV_TOUCH_EVT, OS_NOTIFY_SET_BITS) :
                                OS_TASK_NOTIFY(task_h, DEV_TOUCH_EVT, OS_NOTIFY_SET_BITS);
        }
}

static void dev_init_touch(void)
{
        dev_open_touch();
#ifdef GDI_TOUCH_INIT
        /* Initialize the target touch controller */
        GDI_TOUCH_INIT(gdi->touch_h);
#endif
        dev_close_touch();
}

static void dev_read_data_touch(void)
{
        gdi_touch_data_t touch_data;

        dev_open_touch();
#ifdef GDI_TOUCH_READ_EVENT
        GDI_TOUCH_READ_EVENT(gdi->touch_h, &touch_data);
#endif
        dev_close_touch();

        /* Push touch events into the graphics buffers */
        if (gdi->store_touch_cb) {
                gdi->store_touch_cb(&touch_data);
        } else {
                /* Set a callback to store the touch events */
                OS_ASSERT(0);
        }
#if GDI_CONSOLE_LOG
        printf("\n\rX: %d\t\r  Y: %d\t\r  Pressed: %d\n\r", touch_data.x, touch_data.y, touch_data.pressed);
#endif
}
#endif /* GDI_TOUCH_ENABLE */

static void dev_draw_async_signal(void)
{
        if (gdi->draw_cb) {
                gdi->draw_cb(gdi->user_data);
        }
        OS_EVENT_SIGNAL(gdi->draw_smphr);
}

static void draw_async_timer_cb(OS_TIMER timer)
{
        OS_TASK task = (OS_TASK)OS_TIMER_GET_TIMER_ID(timer);

        OS_TASK_NOTIFY(task, DEV_DRAW_ASYNC_CMPL_EVT, OS_NOTIFY_SET_BITS);
}

static void gdi_task(void *pvParameters)
{
        int8_t wdog_id;
        bool draw_async_cmpl_en;
        OS_TIMER draw_async_timer_h;

        draw_async_timer_h = OS_TIMER_CREATE("DRAW_TIMEOUT", OS_MS_2_TICKS(DEV_DRAW_TIMEOUT_MS), OS_TIMER_FAIL,
                                                                                 (void *)OS_GET_CURRENT_TASK(),
                                                                                          draw_async_timer_cb);
        OS_ASSERT(draw_async_timer_h);


        /* Register Wearable Sleep Quality task to be monitored by watchdog */
        wdog_id = sys_watchdog_register(false);
        draw_async_cmpl_en = 0;

        for (;;) {
                OS_BASE_TYPE ret;
                uint32_t notif;

                /* Notify watchdog on each loop */
                sys_watchdog_notify(wdog_id);

                /* Suspend watchdog while blocking on OS_TASK_NOTIFY_WAIT() */
                sys_watchdog_suspend(wdog_id);

                ret = OS_TASK_NOTIFY_WAIT(0, OS_TASK_NOTIFY_ALL_BITS, &notif, portMAX_DELAY);
                OS_ASSERT(ret == OS_TASK_NOTIFY_SUCCESS);

                /* Resume watchdog */
                sys_watchdog_notify_and_resume(wdog_id);

                if (notif & DEV_DRAW_ASYNC_EVT) {
                        /* Display state must not change while draw operations are in progress */
                        DISPLAY_MUTEX_GET();

                        if (!gdi->continuous_mode_enable) {
                                dev_open_display();
#if GDI_CONSOLE_LOG
                                time_start = GDI_GET_TIME();
#endif /* GDI_CONSOLE_LOG */
#ifdef GDI_DISP_PRE_DRAW
                                GDI_DISP_PRE_DRAW(gdi, gdi->display_h, &gdi->active_layer);
#endif /* GDI_DISP_PRE_DRAW */
                                draw_async_cmpl_en = 1;
                                ad_lcdc_draw_screen_async(gdi->display_h, &gdi->active_layer, frame_update_async_cb,
                                                                                                draw_async_timer_h);
                                /*
                                 * Protect the LCD controller from hanging. There might be the case in which the
                                 * TE signal is enabled and the display is turned off while a frame update is in
                                 * progress. The result would be for LCDC to infinitely wait for the TE signal.
                                 */
                                OS_TIMER_RESET(draw_async_timer_h, OS_TIMER_FOREVER);
                        }
                        else {
                                /* The new layer settings will be applied once the current frame cycle is complete */
                                ad_lcdc_continuous_update_draw(gdi->display_h, &gdi->active_layer);
                                /* Event might already be sent multiple times by LCDC */
                                OS_EVENT_CHECK(gdi->draw_event);
                                /* Wait for the start of a new frame cycle */
                                if (OS_EVENT_SIGNALED != OS_EVENT_WAIT(gdi->draw_event, OS_MS_2_TICKS(DEV_DRAW_TIMEOUT_MS))) {
#if GDI_CONSOLE_LOG
                                        printf("FRAME ERROR - ");
#endif
                                 }
                                /* Wait for the completion of the previously tracked frame cycle */
                                if (OS_EVENT_SIGNALED != OS_EVENT_WAIT(gdi->draw_event, OS_MS_2_TICKS(DEV_DRAW_TIMEOUT_MS))) {
#if GDI_CONSOLE_LOG
                                        printf("FRAME ERROR - ");
#endif
                                }
                                DISPLAY_MUTEX_PUT();
                                dev_draw_async_signal();
                        }
                }

                if (notif & DEV_DRAW_ASYNC_CMPL_EVT) {
                        /*
                         * Protect the following code block from being executed twice.
                         * There might be the case in which both the timer and the
                         * drawing operation are complete simultaneously.
                         */
                        if (draw_async_cmpl_en) {
                                draw_async_cmpl_en = 0;
#ifdef GDI_DISP_POST_DRAW
                                GDI_DISP_POST_DRAW(gdi, gdi->display_h, &gdi->active_layer);
#endif /* GDI_DISP_POST_DRAW */
#if GDI_CONSOLE_LOG
                                console_log();
#endif /* GDI_CONSOLE_LOG */
                                dev_close_display();

                                DISPLAY_MUTEX_PUT();
                                dev_draw_async_signal();
                        }
                }

#if GDI_TOUCH_ENABLE
                if (notif & DEV_TOUCH_EVT) {
                        /* Read touch events */
                        dev_read_data_touch();
                }
#endif /* GDI_TOUCH_ENABLE */
        }
}

void gdi_init(void)
{
        uint8_t i;

        if (gdi) {
                return;
        }

        gdi = OS_MALLOC(sizeof(gdi_t) + sizeof(uint8_t *) * GDI_SINGLE_FB_NUM);
        OS_ASSERT(gdi != NULL);

        memset(gdi, 0, sizeof(gdi_t) + sizeof(uint8_t *) * GDI_SINGLE_FB_NUM);

#if USE_COLOR_FORMAT == CF_NATIVE_RGB332
        gdi->color_format = GDI_FORMAT_RGB332;
#elif USE_COLOR_FORMAT == CF_NATIVE_RGB565
        gdi->color_format = GDI_FORMAT_RGB565;
#elif USE_COLOR_FORMAT == CF_NATIVE_RGBA8888
        gdi->color_format = GDI_FORMAT_RGBA8888;
#endif

        OS_EVENT_CREATE(gdi->draw_event);
        OS_EVENT_CREATE(gdi->dma_event);
#if GDI_MULTIPLEX_TOUCH_DISPLAY
        OS_MUTEX_CREATE(gdi->block_mutex_multiplex_display_touch);
#endif
#if GDI_USE_CONTINUOUS_MODE
        OS_MUTEX_CREATE(gdi->block_mutex_display_status);
#endif

        gdi->draw_smphr = xSemaphoreCreateCounting(1, 1);
        gdi->width = GDI_DISP_RESX;
        gdi->height = GDI_DISP_RESY;
        gdi->stride = calc_stride(gdi->width);

        gdi_display_power_on();
        screen_setup();
#if GDI_TOUCH_ENABLE
        dev_init_touch();
#endif

        gdi->single_buff_sz = gdi->stride * gdi->height;
        gdi->bufs_num = GDI_SINGLE_FB_NUM;
        for (i = 0; i < gdi->bufs_num; i++) {
                gdi->buffer[i] = (uint8_t *)(i * gdi->single_buff_sz
                        + (uint32_t)gdi_get_frame_buffer_addr());
        }
#if dg_configUSE_HW_QSPI2
        size_t qspi_area = GDI_QSPI_RAM_OFFSET;
#if GDI_FB_USE_QSPI_RAM
        if (qspi_is_valid_addr((uint32_t)gdi_get_frame_buffer_addr())) {
                qspi_area += GDI_DISP_RESX * GDI_DISP_RESY * GDI_COLOR_BYTES * GDI_SINGLE_FB_NUM;
        }
#endif /* GDI_FB_USE_QSPI_RAM */
#if GDI_HEAP_USE_QSPI_RAM
        if (qspi_is_valid_addr((uint32_t)gdi_get_gui_heap_addr())) {
                qspi_area += GDI_GUI_HEAP_SIZE;
        }
#endif /* GDI_HEAP_USE_QSPI_RAM */
        OS_ASSERT(qspi_get_device_size(HW_QSPIC2) >= qspi_area);
#endif /* dg_configUSE_HW_QSPI2 */
        gdi->active_buf = 0;

#if GDI_PWMLED_BACKLIGHT
        pwmled_backlight_init(GDI_PWMLED_DUTY_CYCLE_1, GDI_PWMLED_DUTY_CYCLE_2);
#endif /* GDI_PWMLED_BACKLIGHT */

        OS_BASE_TYPE res = OS_TASK_CREATE( "GDI_task",  /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                        gdi_task,                       /* The function that implements the task. */
                        NULL,                           /* The parameter passed to the task. */
                        2048,                           /* The number of bytes to allocate to the
                                                           stack of the task. */
                        OS_TASK_PRIORITY_NORMAL,        /* The priority assigned to the task. */
                        task_h );                       /* The task handle */
        OS_ASSERT(res == OS_TASK_CREATE_SUCCESS);
}

void gdi_set_callback_store_touch(store_touch_callback cb)
{
        if (cb) {
                gdi->store_touch_cb = cb;
        }
}

void gdi_touch_event(void)
{
#if GDI_TOUCH_ENABLE
        dev_notify_touch();
#endif
}

void gdi_display_power_on(void)
{
        if (!gdi->display_powered) {
                screen_power_on();
        }
        gdi->display_powered = true;
}

void gdi_display_power_off(void)
{
        if (gdi->display_powered) {
                screen_power_off();
        }
        gdi->display_powered = false;
}

bool gdi_display_is_powered(void)
{
        return gdi->display_powered;
}

void gdi_display_enable(void)
{
        if (!gdi->display_enabled) {
                screen_enable();
        }
        gdi->display_enabled = true;
#if GDI_USE_CONTINUOUS_MODE
        dev_continuous_mode_enable();
#endif
}

void gdi_display_disable(void)
{
#if GDI_USE_CONTINUOUS_MODE
        dev_continuous_mode_disable();
#endif
        if (gdi->display_enabled) {
                screen_disable();
        }
        gdi->display_enabled = false;
}

bool gdi_display_is_enabled(void)
{
        return gdi->display_enabled;
}

void gdi_display_update(void)
{
        gdi->active_layer.baseaddr = (uint32_t)gdi->buffer[gdi->active_buf];
        gdi->active_layer.dma_prefetch_lvl = HW_LCDC_FIFO_PREFETCH_LVL;
        gdi->active_layer.format = gdi_to_layer_color_format(gdi->color_format);
        gdi->active_layer.resx = gdi->width;
        gdi->active_layer.resy = gdi->height;
        gdi->active_layer.startx = 0;
        gdi->active_layer.starty = 0;
        gdi->active_layer.stride = gdi->stride;

        dev_open_display();
        dev_draw(gdi);
        dev_close_display();
}

void gdi_display_update_async(draw_callback cb, void *user_data)
{
        dev_draw_async(cb, user_data);
}

void gdi_set_partial_update_area(gdi_coord_t x0, gdi_coord_t y0, gdi_coord_t x1, gdi_coord_t y1)
{
        hw_lcdc_frame_t frame;
        frame.startx = x0;
        frame.starty = y0;
        frame.endx = x1;
        frame.endy = y1;

        screen_set_partial_update_area(&frame);
        if (!memcmp(&gdi->active_frame, &frame, sizeof(frame))) {
                return;
        }
        memcpy(&gdi->active_frame, &frame, sizeof(frame));

        dev_open_display();
        dev_set_partial_update();
        dev_close_display();
}

void gdi_display_clear(void)
{
        screen_clear();
}

uint8_t gdi_set_next_frame_buffer(void)
{
        if (++gdi->active_buf >= gdi->bufs_num) {
                gdi->active_buf = 0;
        }
        return gdi->active_buf;
}

uint8_t gdi_get_current_frame_buffer(void)
{
        return gdi->active_buf;
}

void gdi_set_frame_buffer(uint8_t frame)
{
        gdi->active_buf = frame >= gdi->bufs_num ? 0 : frame;
}

static void dma_cb(void *user_data, dma_size_t len)
{
        gdi_t *data = (gdi_t *)user_data;
        OS_EVENT_SIGNAL_FROM_ISR(data->dma_event);
}

void gdi_buffer_memcpy(uint8_t dst, uint8_t src)
{
        DMA_setup dma_setup = {
                .channel_number  = HW_DMA_CHANNEL_0,
                .dma_prio        = HW_DMA_PRIO_7,
                .burst_mode      = HW_DMA_BURST_MODE_8x,
                .irq_enable      = HW_DMA_IRQ_STATE_ENABLED,
                .dma_idle        = HW_DMA_IDLE_BLOCKING_MODE,
                .bus_width       = HW_DMA_BW_WORD,
                .circular        = HW_DMA_MODE_NORMAL,
                .callback        = NULL,
                .user_data       = NULL,
                .irq_nr_of_trans = 0,
                .dma_init        = HW_DMA_INIT_AX_BX_AY_BY,
                .a_inc           = HW_DMA_AINC_TRUE,
                .b_inc           = HW_DMA_BINC_TRUE,
                .dreq_mode       = HW_DMA_DREQ_START,
                .dma_req_mux     = HW_DMA_TRIG_NONE,
        };
        const resource_mask_t res_mask[] = {
                RES_MASK(RES_ID_DMA_CH0), RES_MASK(RES_ID_DMA_CH1),
                RES_MASK(RES_ID_DMA_CH2), RES_MASK(RES_ID_DMA_CH3),
                RES_MASK(RES_ID_DMA_CH4), RES_MASK(RES_ID_DMA_CH5),
                RES_MASK(RES_ID_DMA_CH6), RES_MASK(RES_ID_DMA_CH7)
        };

        dma_setup.src_address = (uint32_t)gdi->buffer[src];
        dma_setup.dest_address = (uint32_t)gdi->buffer[dst];
        dma_setup.bus_width = !(gdi->single_buff_sz & 3) ? HW_DMA_BW_WORD :
                              !(gdi->single_buff_sz & 1) ? HW_DMA_BW_HALFWORD :
                                                           HW_DMA_BW_BYTE;
        dma_setup.length =
                (dma_setup.bus_width != 0) ?
                                             (gdi->single_buff_sz / dma_setup.bus_width) :
                                             gdi->single_buff_sz;
        dma_setup.callback = dma_cb;
        dma_setup.user_data = gdi;

        resource_acquire(res_mask[dma_setup.channel_number], RES_WAIT_FOREVER);

        hw_dma_channel_initialization(&dma_setup);
        hw_dma_channel_enable(dma_setup.channel_number, HW_DMA_STATE_ENABLED);

        OS_EVENT_WAIT(gdi->dma_event, OS_EVENT_FOREVER);

        resource_release(res_mask[dma_setup.channel_number]);
}

void *gdi_get_frame_buffer_addr(void)
{
#if GDI_FB_USE_QSPI_RAM
        return (void *)((uint32_t)qspi_automode_addr(dg_configQSPI2_FLASH_BASE_ADDR)
                                                                   + GDI_QSPI_RAM_OFFSET);
#else /* GDI_FB_USE_QSPI_RAM */
        return (void *)(frame_buffer);
#endif /* GDI_FB_USE_QSPI_RAM */
}

void *gdi_get_gui_heap_addr(void)
{
#if GDI_HEAP_USE_QSPI_RAM
#if GDI_FB_USE_QSPI_RAM
        return (void *)(gdi_get_frame_buffer_addr()
                + GDI_DISP_RESX * GDI_DISP_RESY * GDI_COLOR_BYTES * GDI_SINGLE_FB_NUM);
#else /* GDI_FB_USE_QSPI_RAM */
        return (void *)((uint32_t)qspi_automode_addr(dg_configQSPI2_FLASH_BASE_ADDR)
                                                                   + GDI_QSPI_RAM_OFFSET);
#endif /* GDI_FB_USE_QSPI_RAM */
#else /* GDI_HEAP_USE_QSPI_RAM */
        return (void *)(gui_heap_area);
#endif /* GDI_HEAP_USE_QSPI_RAM */
}

/**
 * \}
 * \}
 */
