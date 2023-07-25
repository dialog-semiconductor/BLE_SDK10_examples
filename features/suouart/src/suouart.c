/**
 ****************************************************************************************
 *
 * @file suouart.c
 *
 * @brief Software Update over UART (SUOUART) application implementation
 *
 * Copyright (C) 2016-2022 Renesas Electronics Corporation and/or its affiliates
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
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdarg.h>
#include "sys_power_mgr.h"
#include "sys_watchdog.h"
#include "osal.h"
#include "suouart.h"

#define DEBUG_APP
#include "suota.h"
#include "ad_nvms.h"
#include "ad_gpadc.h"
#include "sys_watchdog.h"
#include "flash_partitions.h"

#include "ad_nvparam.h"
#include "platform_nvparam.h"
#include "platform_devices.h"

#include "dlg_suouart.h"


/* Calculate CLI overhead:
"SUOUART_PATCH_DATA 0 %d %s\n", suotabuffsz, hexstringbuff
 xxxxxxxxxxxxxxxxxxx  x       20 chars for text
                     yy        4 chars for length
                     zz        2 chars for \n and CR
                               6 chars to make a round 32 */
#define CLI_PATCH_DATA_CMD_SZ   32
/* This is the size of the binary data we can shift in one go */
#define SUOUART_CHUNK_SIZE        2048
/* The host app will probe for this figure (CMD:"getsuouartbuffsz") to decide what size chunk of code to transmit i.e. half that*/
#define SUOUART_BUFFER_SIZE       (SUOUART_CHUNK_SIZE * 2)
/* This is the size of the CLI command buffer we must have for SUOSB
it can be less for other commands, but we expect to have to receive and work on whole
RX buffer's worth when pulling in data for SUOUART */
#define CLI_BUFF_SIZE           (SUOUART_BUFFER_SIZE + CLI_PATCH_DATA_CMD_SZ)

/*********************************************************************
 *
 *       Defines, configurable
 *
 **********************************************************************
 */

#define suouart_TASK_PRIORITY              ( OS_TASK_PRIORITY_NORMAL )
__RETAINED_RW volatile static uint8 run_task = 0;
ad_uart_handle_t uart_handle;
static uint8_t cli_buffer[CLI_BUFF_SIZE];

static uint8_t asciibyte2nibble(uint8_t *src);
static uint32_t suouart_alloc_execution(char *argv, uint8_t **buf);
static void suouart_callback(const char *status);
static char *suouart_err_str(suouart_error_t err);
static void suouart_fwupdate_execution(int32_t pkt_length, char *argv[10], uint8_t *buf, uint32_t buf_size);
static void suouart_task(void *params);
__USED static void uart_printfln(const char *fmt, ...);
static inline int32_t uart_readline(uint8_t *buf, size_t size, bool echo);

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

static uint32_t suouart_alloc_execution(char *argv, uint8_t **buf)
{
        uint32_t size = atoi(argv);

        if (size) {
                if (*buf != NULL) {
                        OS_FREE(buf);
                        buf = 0;
                }
                *buf = OS_MALLOC(size);
                if (*buf != NULL) {
                        uart_printfln("OK");
                        return size;
                } else {
                        uart_printfln("ERROR fail to allocate [%s]", argv[1]);
                }
        } else {
                uart_printfln("ERROR [%s]=INVALID", argv[1]);
        }

        return 0;
}

static void suouart_callback(const char *status)
{
        uart_printfln("INFO %s", status);
}

static char *suouart_err_str(suouart_error_t err)
{
        switch (err)
        {
        case SUOUART_ERROR_OK:
                return "OK";
        case SUOUART_ERROR_READ_NOT_PERMITTED:
                return "READ_NOT_PERMITTED";
        case SUOUART_ERROR_REQUEST_NOT_SUPPORTED:
                return "REQUEST_NOT_SUPPORTED";
         case SUOUART_ERROR_ATTRIBUTE_NOT_FOUND:
                return "ATTRIBUTE_NOT_FOUND";
        case SUOUART_ERROR_ATTRIBUTE_NOT_LONG:
                return "ATTRIBUTE_NOT_LONG";
        case SUOUART_ERROR_APPLICATION_ERROR:
                return "APPLICATION_ERROR";
        default:
                return "UNKNOWN";
        }
}

static void suouart_fwupdate_execution(int32_t pkt_length, char *argv[10], uint8_t *buf, uint32_t buf_size)
{
        uint16_t offs;
        uint16_t size;
        uint32_t slen;
        uint8_t *src;
        uint8_t *dst;
        suouart_error_t err;
        uint32_t value;
        bool read;

        /* keep going until get an empty line */
        while (pkt_length > 0)
        {
                pkt_length = uart_readline(cli_buffer, sizeof(cli_buffer), false) ? true : false;

                if (pkt_length > 0) {
                        uint32_t len = strlen((char*)cli_buffer);
                        /* strip CRs and LFs from end */
                        while (len && ((cli_buffer[len - 1] == '\r') || (cli_buffer[len - 1] == '\n')))
                                len--;
                        cli_buffer[len] = 0;
                }

                uint32_t my_len = 0;
                /* work as a CLI */
                if ((pkt_length > 0) && (cli_buffer[0] != 0)) {
                        uint32_t len = strlen((char*)cli_buffer);
                        char *argv[10];
                        uint32_t argc = 0;
                        uint32_t n = 0;
                        uint8_t *p = cli_buffer;

                        my_len = len;
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
                                uart_printfln("ERROR wrong number of parameters! argc=%d. len=%d", argc, my_len);
                                return;
                        }


                        offs = atoi(argv[1]);
                        size = atoi(argv[2]);
                        slen = strlen(argv[3]);

                        /* ensure we have enough working buffer allocated and message is consistent */
                        if (buf == NULL) {
                                uart_printfln("ERROR no buffer!");
                                return;
                        }

                        if (size > buf_size) {
                                uart_printfln("ERROR out of bounds! (%d > %d buffer)", size, buf_size);
                                return;
                        }

                        if ((size * 2) != slen) {
                                uart_printfln("ERROR size[%d] != string given[slen=%d]", size, slen);
                                return;
                        }


                        src = (uint8_t*)argv[3];
                        dst = buf;
                        err = SUOUART_ERROR_REQUEST_NOT_SUPPORTED;
                        read = false;

                        /* convert hex string back to data */
                        for (n = 0; n < size; n++) {
                                uint8_t hi, lo, hex;
                                hi = asciibyte2nibble(src++);
                                lo = asciibyte2nibble(src++);
                                hex = ((hi << 4) | lo);
                                dst[n] = hex;
                        }
                        if (0 == strcmp(argv[0], "SUOUART_WRITE_STATUS")) {
                                printf("fwupdate: SUOUART_WRITE_STATUS [%s]\r\n", argv[3]);
                                err = suouart_write_req(SUOUART_WRITE_STATUS, offs, size, buf);
                        } else if (0 == strcmp(argv[0], "SUOUART_MEM_DEV")) {
                                printf("fwupdate: SUOUART_MEM_DEV [%s]\n", argv[3]);
                                err = suouart_write_req(SUOUART_WRITE_MEMDEV, offs, size, buf);
                        } else if (0 == strcmp(argv[0], "SUOUART_GPIO_MAP")) {
                                printf("fwupdate: SUOUART_GPIO_MAP [%s]\r\n", argv[3]);
                                err = suouart_write_req(SUOUART_WRITE_GPIO_MAP, offs, size, buf);
                        } else if (0 == strcmp(argv[0], "SUOUART_PATCH_LEN")) {
                                printf("fwupdate: SUOUART_PATCH_LEN [%s]\r\n", argv[3]);
                                err = suouart_write_req(SUOUART_WRITE_PATCH_LEN, offs, size, buf);
                        } else if (0 == strcmp(argv[0], "SUOUART_PATCH_DATA")) {
                                err = suouart_write_req(SUOUART_WRITE_PATCH_DATA, offs, size, buf);
                        } else if (0 == strcmp(argv[0], "SUOUART_READ_STATUS")) {
                                read = true;
                                err = suouart_read_req(SUOUART_READ_STATUS, &value);
                                printf("fwupdate: SUOUART_READ_STATUS [%04lx]\r\n", value);
                        } else if (0 == strcmp(argv[0], "SUOUART_READ_MEMINFO")) {
                                read = true;
                                err = suouart_read_req(SUOUART_READ_MEMINFO, &value);
                                printf("fwupdate: SUOUART_READ_MEMINFO [%04lx]\r\n", value);
                        } else {
                                err = SUOUART_ERROR_REQUEST_NOT_SUPPORTED;
                                printf("fwupdate: what? [%s]\r\n", argv[0]);
                        }
                        if (err == SUOUART_ERROR_OK) {
                                if (read) {
                                        uart_printfln("OK %d", value);
                                } else {
                                        if (0 != strcmp(argv[0], "SUOUART_PATCH_DATA"))
                                                uart_printfln("OK");
                                }
                        } else {
                                uart_printfln("ERROR %s", suouart_err_str(err));
                        }
                }
        }
        printf(("fwupdate: done"));
}

static void suouart_task(void *params)
{
        int32_t length;
        uint8_t *qspibuf = NULL;
        uint32_t qspibufsz = 0;

        /* register call back for status notifications from software update process
        also get active fw image and product header */
        suouart_init(suouart_callback);

       run_task = 1;

       uart_handle = ad_uart_open(&uart_conf);  /* Open the UART with the desired configuration    */
       ASSERT_ERROR(uart_handle != NULL);  /* Check if the UART1 opened OK */

        while(run_task == 1) {

                // indicate cli prompt
                ad_uart_write(uart_handle, (char*)">", 1);

                length = uart_readline(cli_buffer, sizeof(cli_buffer), true);

                if (length > 0) {
                        uint32_t len = strlen((char*)cli_buffer);
                        /* strip CRs and LFs from end */
                        while (len && ((cli_buffer[len - 1] == '\r') || (cli_buffer[len - 1] == '\n')))
                                len--;
                        cli_buffer[len] = 0;
                        /* proper CRLF before new prompt */
                        ad_uart_write(uart_handle, (char*)"\r\n", 2);
                }

                /* work as a CLI */
                if ((length > 0) && (cli_buffer[0] != 0)) {

                        uint32_t len = strlen((char*)cli_buffer);
                        char *argv[10];
                        uint32_t argc = 0;
                        uint32_t n = 0;
                        uint8_t *p = cli_buffer;

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
                                qspibufsz = suouart_alloc_execution(argv[1], &qspibuf);
                        }
                        else if ((0 == strcmp(argv[0], "getsuouartbuffsz")) && (argc == 1)) {
                                uart_printfln("OK %d", SUOUART_BUFFER_SIZE);
                        }
                        else if ((0 == strcmp(argv[0], "fwupdate")) && (argc == 1)) {

                                if ((!qspibuf) || (qspibufsz > (CLI_BUFF_SIZE / 2))) {
                                        uart_printfln(
                                                "ERROR use 'alloc' to define buffer with size <= %d",
                                                (CLI_BUFF_SIZE / 2));
                                }
                                uart_printfln("OK");
                                suouart_fwupdate_execution(length, argv, qspibuf, qspibufsz);
                        }
                        else if ((0 == strcmp(argv[0], "readsdtparam")) && (argc == 1)) {

                                nvms_t *nvms_h;
                                nvms_h = ad_nvms_open(2);
                                uint8_t param_data[84];
                                ad_nvms_read(nvms_h, 0, param_data, 84);
                                for(uint8_t i = 0; i<84; i++)
                                {
                                        uart_printfln("%02x",param_data[i]);
                                }
                        }
                        else {
                                uart_printfln("ERROR unrecognised command [%s]", argv[0]);
                                for (n = 1; n < argc; n++)
                                        uart_printfln("ERROR argument %d = [%s]", n, argv[n]);
                        }
                }
        }

        run_task = 0;

        OS_TASK_DELETE(NULL);
}

void suouart_start_task(void)
{
        if (run_task == 0) {
                run_task = 1;
                OS_BASE_TYPE status;

                OS_TASK task_handle; // Not used

                /* Start the SUOUART application task. */
                status = OS_TASK_CREATE("SuoUartTask",  /* The text name assigned to the task, for
                                                           debug only; not used by the kernel. */
                                suouart_task,           /* The function that implements the task. */
                                NULL,                   /* The parameter passed to the task. */
                                16000,                  /* The number of bytes to allocate to the
                                                           stack of the task. */
                                suouart_TASK_PRIORITY, /* The priority assigned to the task. */
                                task_handle);                  /* The task handle. */

                OS_ASSERT(status == OS_TASK_CREATE_SUCCESS);
        }
}
__USED static void uart_printfln(const char *fmt, ...)
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
        ad_uart_write(uart_handle,buf, len);

}

/**
 * Brief:   Readline for uart device
 *          Will cancel and flush reception if buffer size exceeded
 *
 * Param 1: buffer pointer
 * Param 2: buffer size
 * return:  confirm if line is still active
 */
static inline int32_t uart_readline(uint8_t *buf, size_t size, bool echo)
{
        char c = 0;
        uint8_t *start = buf;
        int32_t len;
        int ret = -1;

        *start = 0; /* ensure kill old buffer */

        do {
                ret = ad_uart_read(uart_handle, &c, 1, 0);
                if (run_task == 0) {
                        return 0;
                }

                if (ret != 1) {
                        continue;
                }


                /* echo all but CR and LF, or combination thereof */
                /* Terminals doing different things, so will recreate 'whatever' as CRLF */
                if (echo) {
                        if ((c != '\n') && (c != '\r')) {
                                ret = ad_uart_write(uart_handle, &c, 1);
                                if (ret < 0) {
                                        continue;
                                }
                        }
                }

                if (c == 8) { /* backspace */
                        buf--;
                        size++;
                        /* overwrite old character */
                        if (echo) {
                                ret = ad_uart_write(uart_handle,(char*)" ", 1);
                                if (ret < 0) {
                                        return 0;
                                }
                                ret = ad_uart_write(uart_handle,&c, 1);
                                if (ret < 0) {
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
                        ret = ad_uart_read(uart_handle, &c, 1, 0);
                        if (ret != 1)
                                return 0;
                } while ((c != '\n') && (c != '\r')); /* wait for CR/LF */
        }

        /* make sure it's null-terminated */
        *buf = '\0';
        len = strlen((char*)start);

        if (!len) {
                ad_uart_write(uart_handle, &c, 1);
        }
        return len;
}

