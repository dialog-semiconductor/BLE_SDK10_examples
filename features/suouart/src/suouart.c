/**
 ****************************************************************************************
 *
 * @file usb_cdc_suousb.c
 *
 * @brief USB CDC SUoUSB application implementation
 *
 * Copyright (C) 2016-2017 Dialog Semiconductor.
 * This computer program includes Confidential, Proprietary Information
 * of Dialog Semiconductor. All Rights Reserved.
 *
 ****************************************************************************************
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdarg.h>
#include "sys_charger.h"
#include "sys_power_mgr.h"
#include "sys_usb.h"
#include "hw_usb.h"
#include "sys_watchdog.h"
#include "USB_CDC.h"
#include "osal.h"

#define DEBUG_APP
#include "suota.h"
#include "ad_nvms.h"
#include "ad_gpadc.h"
#include "sys_watchdog.h"
#include "flash_partitions.h"

#include "ad_nvparam.h"
#include "platform_nvparam.h"
//#include "platform_devices.h"

#include "dlg_suouart.h"

//#include "commons.h"

#include "hw_rtc.h"

/* Calculate CLI overhead:
"SUOUSB_PATCH_DATA 0 %d %s\n", suotabuffsz, hexstringbuff
 xxxxxxxxxxxxxxxxxxx  x       20 chars for text
                     yy        4 chars for length
                     zz        2 chars for \n and CR
                               6 chars to make a round 32 */
#define CLI_PATCH_DATA_CMD_SZ   32
/* This is the size of the binary data we can shift in one go */
#define SUOUSB_CHUNK_SIZE        2048
/* The host app will probe for this figure (CMD:"getsuousbbuffsz") to decide what size chunks to transmit i.e. half that
it knows it has to do 2 chunks per buffer - less failed */
#define SUOUSB_BUFFER_SIZE       (SUOUSB_CHUNK_SIZE * 2)
/* Should be big enough for any response we have */
#define USB_CDC_TX_BUFF_SIZE    256
#define USB_CDC_RX_BUFF_SIZE    (SUOUSB_BUFFER_SIZE + CLI_PATCH_DATA_CMD_SZ)
/* This is the size of the CLI command buffer we must have for SUOSB
it can be less for other commands, but we expect to have to receive and work on whole
RX buffer's worth when pulling in data for SUOUSB */
#define CLI_BUFF_SIZE           USB_CDC_RX_BUFF_SIZE

/*********************************************************************
 *
 *       Defines, configurable
 *
 **********************************************************************
 */

#define usb_main_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )
__RETAINED static OS_TASK usb_cdc_task_handle;
__RETAINED_RW volatile static uint8 run_usb_task = 0;
//static USB_HOOK UsbpHook;
static USB_CDC_HANDLE usb_cdc_hInst; //no need to be __RETAINED. No Sleep when USB is plugged.
#if (CONFIG_RETARGET_USB==0)
static uint8_t cli_buffer[CLI_BUFF_SIZE];
#endif

extern void app_hibernate(void);

/* USB Enumeration Information */
static const USB_DEVICE_INFO _DeviceInfo = {
        0x2DCF,                 // VendorId
        0x6002,                 // ProductId
        "Dialog Semiconductor", // VendorName
        "DA1469x CDC",          // ProductName
        "12345678"              // SerialNumber
};

__USED void dialog_printfln(const char *fmt, ...)
{
        char buf[127 + 2];
        va_list ap;
        int len;

        /* Format line of text into buffer and save space for newline. vsnprintf() reserves 1 byte
        for \0, but we'll overwrite it with newline anyway - see below. */
        va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf) - 2 + 1, fmt, ap);
        va_end(ap);

        /* Append newline characters at the end of string. It's ok to overwrite \0 since string does
        not need to be null-terminated for sending over UART (it's handled as binary buffer). */
        len = strlen(buf);

        memcpy(&buf[len], "\r\n", 2);
        len += 2;

        buf[len]=0;
        printf(buf);
}

__USED void dialog_cdc_printfln(const char *fmt, ...)
{
        char buf[127 + 2];
        va_list ap;
        int len;

        /* Format line of text into buffer and save space for newline. vsnprintf() reserves 1 byte
        for \0, but we'll overwrite it with newline anyway - see below. */
        va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf) - 2 + 1, fmt, ap);
        va_end(ap);

        /* Append newline characters at the end of string. It's ok to overwrite \0 since string does
        not need to be null-terminated for sending over UART (it's handled as binary buffer). */
        len = strlen(buf);

        memcpy(&buf[len], "\n\r", 2);
        len += 2;
        USBD_CDC_Write(usb_cdc_hInst, (uint8_t*)buf, len, 0);
}

/*********************************************************************
 *
 *       _AddCDC
 *
 *  Function description
 *    Add communication device class to USB stack
 */
static USB_CDC_HANDLE _AddCDC(void)
{
        static U8 _abOutBuffer[USB_MAX_PACKET_SIZE];
        USB_CDC_INIT_DATA InitData;
        USB_CDC_HANDLE hInst;

        InitData.EPIn = USBD_AddEP(USB_DIR_IN, USB_TRANSFER_TYPE_BULK, 0, NULL, 0);
        InitData.EPOut = USBD_AddEP(USB_DIR_OUT, USB_TRANSFER_TYPE_BULK, 0, _abOutBuffer,
                USB_MAX_PACKET_SIZE);
        InitData.EPInt = USBD_AddEP(USB_DIR_IN, USB_TRANSFER_TYPE_INT, 8, NULL, 0);
        hInst = USBD_CDC_Add(&InitData);

        return hInst;
}

/*********************************************************************
 *
 *       usb_is_suspended
 *
 *  Function description
 *    Callback to indicate that the USB Node is going to be suspended
 */
void usb_is_suspended(void)
{
        printf("App: USB Suspend!\r\n");
}

/*********************************************************************
 *
 *       usb_is_resumed
 *
 *  Function description
 *    Callback to indicate that the USB Node is going to be resumed
 */
void usb_is_resumed(void)
{
        printf("App: USB Resume!\r\b");
}

/**********************************************************************************************************************/
#if (CONFIG_RETARGET_USB==0)
static char *suousb_err_str(suousb_error_t err)
{
        switch (err)
        {
        case SUOUSB_ERROR_OK:
                return "OK";
        case SUOUSB_ERROR_READ_NOT_PERMITTED:
                return "READ_NOT_PERMITTED";
        case SUOUSB_ERROR_REQUEST_NOT_SUPPORTED:
                return "REQUEST_NOT_SUPPORTED";
         case SUOUSB_ERROR_ATTRIBUTE_NOT_FOUND:
                return "ATTRIBUTE_NOT_FOUND";
        case SUOUSB_ERROR_ATTRIBUTE_NOT_LONG:
                return "ATTRIBUTE_NOT_LONG";
        case SUOUSB_ERROR_APPLICATION_ERROR:
                return "APPLICATION_ERROR";
        default:
                return "UNKNOWN";
        }
}

/**
 * Brief:   Readline for CDC device
 *          Will cancel and flush reception if buffer size exceeded
 *
 * Param 1: buffer pointer
 * Param 2: buffer size
 * return:  confirm if line is still active
 */
static inline int32_t cdc_readline(uint8_t *buf, size_t size, bool echo)
{
        uint8_t c;
        uint8_t *start = buf;
        int32_t len;
        int ret = -1;

        *start = 0; /* ensure kill old buffer */

        do {
                ret = USBD_CDC_Receive(usb_cdc_hInst, (uint8_t*)&c, 1, 0);
                if (run_usb_task == 0) {
                        return 0;
                }

                if (ret != 1) {
                        continue;
                }


                /* echo all but CR and LF, or combination thereof */
                /* Terminals doing different things, so will recreate 'whatever' as CRLF */
                if (echo) {
                        if ((c != '\n') && (c != '\r')) {
                                ret = USBD_CDC_Write(usb_cdc_hInst, (uint8_t*)&c, 1, 0);
                                if (ret < 0) {
                                        printf("cdc_readline: usb_cdc_tx_data1 returned 0 [%d]\r\n", ret);
                                        continue;
                                }
                        }
                }

                if (c == 8) { /* backspace */
                        buf--;
                        size++;
                        /* overwrite old character */
                        if (echo) {
                                ret = USBD_CDC_Write(usb_cdc_hInst, (uint8_t*)" ", 1, 0);
                                if (ret < 0) {
                                        printf(("cdc_readline: usb_cdc_tx_data2 returned 0\r\n"));
                                        return 0;
                                }
                                ret = USBD_CDC_Write(usb_cdc_hInst, (uint8_t*)&c, 1, 0);
                                if (ret < 0) {
                                        printf(("cdc_readline: usb_cdc_tx_data3 returned 0\r\n"));
                                        return 0;
                                }
                        }
                }
                else {
                        *(buf++) = c;
                        size--;
                }
        } while ((c != '\n') && (c != '\r') && (size > 1)); // wait for CR or reserve 1 char for \0

        /* reject overly long lines, flush remainder, return NULL string */
        if (size == 1) {
                *start = 0; /* empty string */
                do {
                        ret = USBD_CDC_Receive(usb_cdc_hInst, (uint8_t*)&c, 1, 0);
                        if (ret != 1)
                                return 0;
                } while ((c != '\n') && (c != '\r')); /* wait for CR/LF */
        }

        /* make sure it's null-terminated */
        *buf = '\0';
        len = strlen((char*)start);

        if (!len) {
                printf("cdc_readline: strlen returned 0\r\n");
        }
        return len;
}

/**
 * Brief:  Given an ASCII character, returns hex nibble value
 * Param:  pointer to ASCII value
 * Return: 0x00 to 0x0F, or 0xFF if invalid hex
 *         (i.e. not '0'-'9', 'a'-'f' or 'A'-'F')
 */
static uint8_t asciibyte2nibble(uint8_t *src)
{
        return ((src[0] >= '0') ?
                ((src[0] > '9') ?
                        ((src[0] >= 'A') ?
                                ((src[0] > 'F') ?
                                        ((src[0] >= 'a') ?
                                                (src[0] > 'f') ? 0xFF : (src[0] - 'a' + 10)
                                                : 0xFF)
                                        :
                                        (src[0] - 'A' + 10))
                                :
                                0xFF)
                        :
                        (src[0] - '0'))
                :
                0xFF);
}

static void usb_cdc_suota_callback(const char *status)
{
        dialog_cdc_printfln("INFO %s", status);
}

static uint32_t usb_cdc_suousb_alloc_execution(char *argv, uint8_t **buf)
{
        uint32_t size = atoi(argv);

        if (size) {
                if (*buf != NULL) {
                        OS_FREE(buf);
                        buf = 0;
                }
                *buf = OS_MALLOC(size);
                if (*buf != NULL) {
                        dialog_cdc_printfln("OK");
                        return size;
                } else {
                        dialog_cdc_printfln("ERROR fail to allocate [%s]", argv[1]);
                }
        } else {
                dialog_cdc_printfln("ERROR [%s]=INVALID", argv[1]);
        }

        return 0;
}

static void usb_cdc_suousb_fwupdate_execution(int32_t pkt_length, char *argv[10], uint8_t *buf, uint32_t buf_size)
{
        uint16_t offs;
        uint16_t size;
        uint32_t slen;
        uint8_t *src;
        uint8_t *dst;
        suousb_error_t err;
        uint32_t value;
        bool read;

        /* keep going until get an empty line */
        while (pkt_length > 0)
        {
                pkt_length = cdc_readline(cli_buffer, sizeof(cli_buffer),
                        false) ? true : false;

                if (pkt_length > 0) {
                        uint32_t len = strlen((char*)cli_buffer);
                        /* strip CRs and LFs from end */
                        while (len && ((cli_buffer[len - 1] == '\r') || (cli_buffer[len - 1] == '\n')))
                                len--;
                        cli_buffer[len] = 0;
                }

                /* work as a CLI */
                if ((pkt_length > 0) && (cli_buffer[0] != 0)) {
                        uint32_t len = strlen((char*)cli_buffer);
                        char *argv[10];
                        uint32_t argc = 0;
                        uint32_t n = 0;
                        uint8_t *p = cli_buffer;

                        /* pseudo-strtok */
                        while ((n < len) && (argc < 10)) {
                                argv[argc++] = (char*)&p[n];
                                while ((n < len) && (p[n] != ' '))
                                        n++;
                                p[n] = 0;
                                n++;
                                while ((n < len) && (p[n] == ' '))
                                        n++;
                        }

                        /* process valid requests */
                        if (argc != 4) {
                                dialog_cdc_printfln("ERROR wrong number of parameters!");
                                return;
                        }


                        offs = atoi(argv[1]);
                        size = atoi(argv[2]);
                        slen = strlen(argv[3]);

                        /* ensure we have enough working buffer allocated and message is consistent */
                        if (buf == NULL) {
                                dialog_cdc_printfln("ERROR no buffer!");
                                return;
                        }

                        if (size > buf_size) {
                                dialog_cdc_printfln("ERROR out of bounds! (%d > %d buffer)", size, buf_size);
                                return;
                        }

                        if ((size * 2) != slen) {
                                dialog_cdc_printfln("ERROR size != string given");
                                return;
                        }


                        src = (uint8_t*)argv[3];
                        dst = buf;
                        err = SUOUSB_ERROR_REQUEST_NOT_SUPPORTED;
                        read = false;

                        /* convert hex string back to data */
                        for (n = 0; n < size; n++) {
                                uint8_t hi, lo, hex;
                                hi = asciibyte2nibble(src++);
                                lo = asciibyte2nibble(src++);
                                hex = ((hi << 4) | lo);
                                dst[n] = hex;
                        }
                        if (0 == strcmp(argv[0], "SUOUSB_WRITE_STATUS")) {
                                printf("fwupdate: SUOUSB_WRITE_STATUS [%s]\r\n", argv[3]);
                                err = suousb_write_req(SUOUSB_WRITE_STATUS, offs, size, buf);
                        } else if (0 == strcmp(argv[0], "SUOUSB_MEM_DEV")) {
                                printf("fwupdate: SUOUSB_MEM_DEV [%s]\n", argv[3]);
                                err = suousb_write_req(SUOUSB_WRITE_MEMDEV, offs, size, buf);
                        } else if (0 == strcmp(argv[0], "SUOUSB_GPIO_MAP")) {
                                printf("fwupdate: SUOUSB_GPIO_MAP [%s]\r\n", argv[3]);
                                err = suousb_write_req(SUOUSB_WRITE_GPIO_MAP, offs, size, buf);
                        } else if (0 == strcmp(argv[0], "SUOUSB_PATCH_LEN")) {
                                printf("fwupdate: SUOUSB_PATCH_LEN [%s]\r\n", argv[3]);
                                err = suousb_write_req(SUOUSB_WRITE_PATCH_LEN, offs, size, buf);
                        } else if (0 == strcmp(argv[0], "SUOUSB_PATCH_DATA")) {
                                err = suousb_write_req(SUOUSB_WRITE_PATCH_DATA, offs, size, buf);
                        } else if (0 == strcmp(argv[0], "SUOUSB_READ_STATUS")) {
                                read = true;
                                err = suousb_read_req(SUOUSB_READ_STATUS, &value);
                                printf("fwupdate: SUOUSB_READ_STATUS [%04lx]\r\n", value);
                        } else if (0 == strcmp(argv[0], "SUOUSB_READ_MEMINFO")) {
                                read = true;
                                err = suousb_read_req(SUOUSB_READ_MEMINFO, &value);
                                printf("fwupdate: SUOUSB_READ_MEMINFO [%04lx]\r\n", value);
                        } else {
                                err = SUOUSB_ERROR_REQUEST_NOT_SUPPORTED;
                                printf("fwupdate: what? [%s]\r\n", argv[0]);
                        }
                        if (err == SUOUSB_ERROR_OK) {
                                if (read) {
                                        dialog_cdc_printfln("OK %d", value);
                                } else {

                                        /* but will need to deal with sequence
                                        disorder with SUOTA_CMP_OK notification
                                        on host side
                                        But should wait on host of OK because
                                        flash erase may take time and we need
                                        next TX deferred so USB timing isn't
                                        messed up
                                        - only, I don't get the notification
                                        and writing next block causes error!
                                        Because chunk_len is a uint8_t and the
                                        note about using len=n*20 where
                                        64<len<buffer in receiver is incomplete,
                                        so 240 is maximum chunk size. */
                                        if (0 != strcmp(argv[0], "SUOUSB_PATCH_DATA"))
                                                dialog_cdc_printfln("OK");
                                }
                        } else {
                                dialog_cdc_printfln("ERROR %s", suousb_err_str(err));
                        }
                }
        }
        printf(("fwupdate: done"));
}
#endif

void usb_cdc_task(void *params)
{
#if (CONFIG_RETARGET_USB==0)
        int32_t length;
        uint8_t *qspibuf = NULL;
        uint32_t qspibufsz = 0;

        /* register call back for status notifications from software update process
        also specify buffer size - but add 2 because do_patch_data_write() won't accept
        having written a full buffer */
        suousb_init(usb_cdc_suota_callback);
#endif

        USBD_Init();
        USBD_CDC_Init();
        usb_cdc_hInst = _AddCDC();
        USBD_SetDeviceInfo(&_DeviceInfo);
        USBD_Start();

        printf("usb_flash_task: enter (%s %s)\r\n", __DATE__, __TIME__);

        while(run_usb_task == 1) {
                while ((USBD_GetState() & (USB_STAT_CONFIGURED | USB_STAT_SUSPENDED))
                        != USB_STAT_CONFIGURED) {
                        USB_OS_Delay(50);
                }
#if (CONFIG_RETARGET_USB==0)
                printf(("usb_flash_task: prompt\r\n"));
                USBD_CDC_Write(usb_cdc_hInst, (uint8_t*)">", 1, 0);
                length = cdc_readline(cli_buffer, sizeof(cli_buffer), true);

                if (length > 0) {
                        uint32_t len = strlen((char*)cli_buffer);
                        /* strip CRs and LFs from end */
                        while (len && ((cli_buffer[len - 1] == '\r') || (cli_buffer[len - 1] == '\n')))
                                len--;
                        cli_buffer[len] = 0;
                        /* proper CRLF before new prompt */
                        length = USBD_CDC_Write(usb_cdc_hInst, (uint8_t*)"\r\n", 2, 0);
                }

                /* work as a CLI */
                if ((length > 0) && (cli_buffer[0] != 0)) {
                        uint32_t len = strlen((char*)cli_buffer);
                        char *argv[10];
                        uint32_t argc = 0;
                        uint32_t n = 0;
                        uint8_t *p = cli_buffer;

                        printf("usb_flash_task: action [%s]\r\n", cli_buffer);

                        //pseudo-strtok
                        while ((n < len) && (argc < 10)) {
                                argv[argc++] = (char*)&p[n];
                                while ((n < len) && (p[n] != ' '))
                                        n++;
                                p[n] = 0;
                                n++;
                                while ((n < len) && (p[n] == ' '))
                                        n++;
                        }

                        //interpret
                        if ((0 == strcmp(argv[0], "alloc")) && (argc == 2)) {
                                qspibufsz = usb_cdc_suousb_alloc_execution(argv[1], &qspibuf);
                        }
                        else if ((0 == strcmp(argv[0], "getsuousbbuffsz")) && (argc == 1)) {
                                dialog_cdc_printfln("OK %d", SUOUSB_BUFFER_SIZE);
                        }
                        else if ((0 == strcmp(argv[0], "fwupdate")) && (argc == 1)) {

                                if ((!qspibuf) || (qspibufsz > (CLI_BUFF_SIZE / 2))) {
                                        dialog_cdc_printfln(
                                                "ERROR use 'alloc' to define buffer with size <= %d",
                                                (CLI_BUFF_SIZE / 2));
                                }
                                dialog_cdc_printfln("OK");
                                usb_cdc_suousb_fwupdate_execution(length, argv, qspibuf, qspibufsz);
                        }
                        else if ((0 == strcmp(argv[0], "readsdtparam")) && (argc == 1)) {

                                nvms_t *nvms_h;
                                nvms_h = ad_nvms_open(2);
                                uint8_t param_data[84];
                                ad_nvms_read(nvms_h, 0, param_data, 84);
                                for(uint8_t i = 0; i<84; i++)
                                {
                                        dialog_cdc_printfln("%02x",param_data[i]);
                                }
                        }
                        else if ((0 == strcmp(argv[0], "readbatt")) && (argc == 1)) {

                                uint16_t batt_mv = 0;
//                                uint16_t value;
//                                ad_gpadc_handle_t handle;
//                                handle = ad_gpadc_open(&BATTERY_LEVEL);
//                                ad_gpadc_read(handle, &value);
//                                batt_mv = ad_gpadc_conv_to_batt_mvolt(BATTERY_LEVEL.drv, value);
//                                ad_gpadc_close(handle, false);
                                dialog_cdc_printfln("Battery voltage is %d mv",batt_mv);
                        }
                        else if ((0 == strcmp(argv[0], "setsleep")) && (argc == 1)) {

//                                app_hibernate();
                        }
                        else if ((0 == strcmp(argv[0], "readtimeonbump")) && (argc == 1)) {

                                //uint32_t timeofbump = 0;
                                hw_rtc_time_t rtc_time_bump;
                                hw_rtc_calendar_t rtc_cal;

                                /*Use RTC API to unpack BCD date/time into convenient formats.*/
                                hw_rtc_get_time_clndr(&rtc_time_bump,  &rtc_cal);
/*
                                rtc_time rtc_time_bump= {
                                        .hour   = RTC_TIME_HOUR,
                                        .minute = RTC_TIME_MINUTE,
                                        .sec    = RTC_TIME_SECOND,
                                        .hsec   = 0,
                                        .hour_mode = RTC_24H_CLK
                                };
                                rtc_calendar rtc_cal= {
                                        .year   = RTC_CALENDAR_YEAR,
                                        .month  = RTC_CALENDAR_MONTH,
                                        .mday   = RTC_CALENDAR_MONTH_DAY,
                                        .wday   = RTC_CALENDAR_WEEK_DAY
                                };
*/
//                                timeofbump = hw_rtc_get_time_bcd();
                                hw_rtc_get_time_clndr(&rtc_time_bump, &rtc_cal);

                                dialog_cdc_printfln("Current Time is Time/Date = %u:%u:%u.%u / %u-%u-%u \n ",
                                                                rtc_time_bump.hour, rtc_time_bump.minute,
                                                                rtc_time_bump.sec, rtc_time_bump.hsec,
                                                                rtc_cal.year, rtc_cal.month, rtc_cal.mday
                                                        );

                        }
                        else {
                                dialog_cdc_printfln("ERROR unrecognised command [%s]", argv[0]);
                                for (n = 1; n < argc; n++)
                                        dialog_cdc_printfln("ERROR argument %d = [%s]", n, argv[n]);
                        }
                }

                if (length <= 0) {
                        printf(("disconnected"));
                }
#else
                break;
#endif
        }

        run_usb_task = 0;

        OS_TASK_DELETE(NULL);
}


/*********************************************************************
 *
 *       usb_start_enumeration_cb
 *
 *  Function description
 *    Event callback called from the usbcharger task to notify
 *    the application about to allow enumeration.
 *    Note: The USB charger task is started before the application task. Thus, these
 *          call-backs may be called before the application task is started.
 *          The application code should handle this case, if need be.
 */
void sys_usb_ext_hook_begin_enumeration(void)
{
        if (run_usb_task == 0) {
                run_usb_task = 1;
                OS_BASE_TYPE status;

                /* Start the USB CDC application task. */
                status = OS_TASK_CREATE("UsbCdcTask",   /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                                usb_cdc_task,           /* The function that implements the task. */
                                NULL,                   /* The parameter passed to the task. */
                                8192,                    /* The number of bytes to allocate to the
                                                           stack of the task. */
                                usb_main_TASK_PRIORITY, /* The priority assigned to the task. */
                                usb_cdc_task_handle);   /* The task handle. */

                OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);
        }

}

//#include "commons.h"

#if (ENABLE_VISUAL_PROMPTS==1)
extern OS_TASK  led_task_h;
#endif

/*********************************************************************
 *
 *       usb_detach_cb
 *
 *  Function description
 *    Event callback called from the USB charger task to notify
 *    the application that a detach of the USB cable was detected.
 *
 *    Note: The USB charger task is started before the application task. Thus, these
 *          call-backs may be called before the application task is started.
 *          The application code should handle this case, if need be.
 */
void  sys_usb_ext_hook_detach(void)
{
        run_usb_task = 0;
        USBD_Stop();
        USBD_DeInit();


#if (ENABLE_VISUAL_PROMPTS==1)
        if (led_task_h != NULL && OS_GET_TASK_STATE(led_task_h) != eDeleted) OS_TASK_NOTIFY(led_task_h, LED_IDLE_NOTIFY, OS_NOTIFY_SET_BITS);
#endif

        printf("USB un-plugged");

}
