/*****************************************************************/ /**
* @file unir_lcd_drv_st7789v.c
* @brief
* @author bronson.zhan@quectel.com
* @date 2025-11-03
*
* @copyright Copyright (c) 2023 Quectel Wireless Solution, Co., Ltd.
* All Rights Reserved. Quectel Wireless Solution Proprietary and Confidential.
*
* @par EDIT HISTORY FOR MODULE
* <table>
* <tr><th>Date <th>Version <th>Author <th>Description
* <tr><td>2025-11-03 <td>1.0 <td>Bronson.Zhan <td> Init
* </table>
**********************************************************************/
#include "qosa_def.h"
#include "qosa_sys.h"
#include "qosa_log.h"

#include "qosa_lcd.h"
#include "qosa_spi.h"

/*===========================================================================
 *  Macro Definition
 ===========================================================================*/
#define lcd_drv_log(...) QOSA_LOG_D(LOG_TAG_LCD_API, ##__VA_ARGS__)

/*===========================================================================
 *  extern API Functions / Variate
 ===========================================================================*/
static void unir_lcd_drv_st7789v_init(qosa_lcd_channel_e lcd_no);
static void unir_lcd_drv_st7789v_deInit(qosa_lcd_channel_e lcd_no);
static void unir_lcd_drv_st7789v_enterSleep(qosa_lcd_channel_e lcd_no, qosa_bool_t is_sleep);
static void unir_lcd_drv_st7789v_set_window(
    qosa_lcd_channel_e lcd_no,
    qosa_uint16_t      left,   // start Horizon address
    qosa_uint16_t      top,    // start Vertical address
    qosa_uint16_t      right,  // end Horizon address
    qosa_uint16_t      bottom  // end Vertical address
);
static qosa_uint32_t unir_lcd_drv_st7789v_Read_Id(qosa_lcd_channel_e lcd_no);

/*===========================================================================
 *  Variate
 ===========================================================================*/

static qosa_lcd_operations_t g_unir_lcd_drv_st7789v_operations
    = {.init = unir_lcd_drv_st7789v_init,
       .deInit = unir_lcd_drv_st7789v_deInit,
       .enterSleep = unir_lcd_drv_st7789v_enterSleep,
       .setDisplayWindow = unir_lcd_drv_st7789v_set_window,
       .readId = unir_lcd_drv_st7789v_Read_Id};

qosa_lcd_drv_cfg_t g_unir_lcd_drv_st7789v_info
    = {QOSA_LCD_SPECIAL_PORT,

       (qosa_uint8_t *)"ST7789V", /* Camera model name */
       0x858552,

       240,
       320,

       QOSA_LCD_BUS_MODE_4_WIRE_I_SPI,
       QOSA_LCD_INPUT_FORMAT_RGB565,
       QOSA_LCD_OUTPUT_FORMAT_RGB565,

       &g_unir_lcd_drv_st7789v_operations,
       52 * 1024 * 1024,
       4 * 1024 * 1024,
       {QOSA_GPIO_15, 10}};

qosa_lcd_drv_cfg_t g_unir_lcd_spi_drv_st7789v_info
    = {QOSA_LCD_SPI_PORT_1,

       (qosa_uint8_t *)"ST7789V", /* Camera model name */
       0x858552,

       240,
       320,

       QOSA_LCD_BUS_MODE_4_WIRE_I_SPI,
       QOSA_LCD_INPUT_FORMAT_RGB565,
       QOSA_LCD_OUTPUT_FORMAT_RGB565,

       &g_unir_lcd_drv_st7789v_operations,
       QOSA_SPI_CLK_52MHZ,
       QOSA_SPI_CLK_3_25MHZ,
       {QOSA_GPIO_8, 10},
       {QOSA_SPI_CLK_CPOL0_CPHA0, QOSA_SPI_LSB_FIRST, QOSA_SPI_TRANSMIT_DMA, QOSA_SPI_NSS_MASTER_SOFTWARE, QOSA_GPIO_12, QOSA_GPIO_14}};

/*===========================================================================
 *  Static API Functions
 ===========================================================================*/

/**
 * @brief Function will running after LCD init
 *
 * @param[in] qosa_lcd_channel_e lcd_no
 *          - LCD channel number selection
 */
static void unir_lcd_drv_st7789v_init(qosa_lcd_channel_e lcd_no)
{
    lcd_drv_log("lcd:    _st7789vInit ");

    qosa_lcd_write_cmd(lcd_no, 0x11);
    qosa_lcd_write_cmd_data(lcd_no, 0x00);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_task_sleep_ms(120);

    qosa_lcd_write_cmd(lcd_no, 0x36);
    qosa_lcd_write_cmd_data(lcd_no, 0x08);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0x20);
    qosa_lcd_write_cmd_data(lcd_no, 0x00);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0x3A);
    qosa_lcd_write_cmd_data(lcd_no, 0x05);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0x35);
    qosa_lcd_write_cmd_data(lcd_no, 0x00);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0xB2);
    qosa_lcd_write_cmd_data(lcd_no, 0x0C);
    qosa_lcd_write_cmd_data(lcd_no, 0x0C);
    qosa_lcd_write_cmd_data(lcd_no, 0x00);
    qosa_lcd_write_cmd_data(lcd_no, 0x33);
    qosa_lcd_write_cmd_data(lcd_no, 0x33);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0xB7);
    qosa_lcd_write_cmd_data(lcd_no, 0x35);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0xBB);
    qosa_lcd_write_cmd_data(lcd_no, 0x20);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0xC0);
    qosa_lcd_write_cmd_data(lcd_no, 0x2C);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0xC2);
    qosa_lcd_write_cmd_data(lcd_no, 0x01);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0xC3);
    qosa_lcd_write_cmd_data(lcd_no, 0x0B);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0xC4);
    qosa_lcd_write_cmd_data(lcd_no, 0x20);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0xC6);
    qosa_lcd_write_cmd_data(lcd_no, 0x0F);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0xD0);
    qosa_lcd_write_cmd_data(lcd_no, 0xA4);
    qosa_lcd_write_cmd_data(lcd_no, 0xA1);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0xE0);
    qosa_lcd_write_cmd_data(lcd_no, 0xD0);
    qosa_lcd_write_cmd_data(lcd_no, 0x03);
    qosa_lcd_write_cmd_data(lcd_no, 0x09);
    qosa_lcd_write_cmd_data(lcd_no, 0x0E);
    qosa_lcd_write_cmd_data(lcd_no, 0x11);
    qosa_lcd_write_cmd_data(lcd_no, 0x3D);
    qosa_lcd_write_cmd_data(lcd_no, 0x47);
    qosa_lcd_write_cmd_data(lcd_no, 0x55);
    qosa_lcd_write_cmd_data(lcd_no, 0x53);
    qosa_lcd_write_cmd_data(lcd_no, 0x1A);
    qosa_lcd_write_cmd_data(lcd_no, 0x16);
    qosa_lcd_write_cmd_data(lcd_no, 0x14);
    qosa_lcd_write_cmd_data(lcd_no, 0x1F);
    qosa_lcd_write_cmd_data(lcd_no, 0x22);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0xE1);
    qosa_lcd_write_cmd_data(lcd_no, 0xD0);
    qosa_lcd_write_cmd_data(lcd_no, 0x02);
    qosa_lcd_write_cmd_data(lcd_no, 0x08);
    qosa_lcd_write_cmd_data(lcd_no, 0x0D);
    qosa_lcd_write_cmd_data(lcd_no, 0x12);
    qosa_lcd_write_cmd_data(lcd_no, 0x2C);
    qosa_lcd_write_cmd_data(lcd_no, 0x43);
    qosa_lcd_write_cmd_data(lcd_no, 0x55);
    qosa_lcd_write_cmd_data(lcd_no, 0x53);
    qosa_lcd_write_cmd_data(lcd_no, 0x1E);
    qosa_lcd_write_cmd_data(lcd_no, 0x1B);
    qosa_lcd_write_cmd_data(lcd_no, 0x19);
    qosa_lcd_write_cmd_data(lcd_no, 0x20);
    qosa_lcd_write_cmd_data(lcd_no, 0x22);
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0x29);
    qosa_lcd_write_cmd_data(lcd_no, 0x00);
    qosa_lcd_flush_cmd(lcd_no);
}

/**
 * @brief Function will running before LCD deinit
 *
 * @param[in] qosa_lcd_channel_e lcd_no
 *          - LCD channel number selection
 */
static void unir_lcd_drv_st7789v_deInit(qosa_lcd_channel_e lcd_no)
{
    lcd_drv_log("lcd:   _st7789vdeInit");
}

/**
 * @brief Function will running before try display LCD on/off
 *
 * @param[in] qosa_lcd_channel_e lcd_no
 *          - LCD channel number selection
 *
 * @param[in] qosa_bool_t is_sleep
 *          - QOSA_TRUE:  display on
 *          - QOSA_FALSE: display off
 */
static void unir_lcd_drv_st7789v_enterSleep(qosa_lcd_channel_e lcd_no, qosa_bool_t is_sleep)
{
    lcd_drv_log("lcd:   _st7789vSleepIn, is_sleep = %d", is_sleep);

    if (is_sleep)
    {
        qosa_lcd_write_cmd(lcd_no, 0x28);  // display off
        qosa_lcd_flush_cmd(lcd_no);
        qosa_task_sleep_ms(120);
        qosa_lcd_write_cmd(lcd_no, 0x10);  // enter sleep mode
        qosa_lcd_flush_cmd(lcd_no);
    }
    else
    {
        qosa_lcd_write_cmd(lcd_no, 0x11);  // sleep out mode
        qosa_lcd_flush_cmd(lcd_no);
        qosa_task_sleep_ms(120);
        qosa_lcd_write_cmd(lcd_no, 0x29);  // display on
        qosa_lcd_flush_cmd(lcd_no);
    }
}

/**
 * @brief Function will running before try write package to LCD
 *
 * @param[in] qosa_lcd_channel_e lcd_no
 *          - LCD channel number selection
 *
 * @param[in] qosa_uint16_t left
 *          - start Horizon address
 *
 * @param[in] qosa_uint16_t top
 *          - start Vertical address
 *
 * @param[in] qosa_uint16_t right
 *          - end Horizon address
 *
 * @param[in] qosa_uint16_t bottom
 *          - end Vertical address
 */
static void unir_lcd_drv_st7789v_set_window(qosa_lcd_channel_e lcd_no, qosa_uint16_t left, qosa_uint16_t top, qosa_uint16_t right, qosa_uint16_t bottom)
{
    qosa_uint16_t newleft = left;
    qosa_uint16_t newtop = top;
    qosa_uint16_t newright = right;
    qosa_uint16_t newbottom = bottom;

    lcd_drv_log("lcd:st7789vSetDisplayWindow L = %d, top = %d, R = %d, bot = %d", left, top, right, bottom);

    qosa_lcd_write_cmd(lcd_no, 0x2a);                         // set hori start , end (left, right)
    qosa_lcd_write_cmd_data(lcd_no, (newleft >> 8) & 0xff);   // left high 8 b
    qosa_lcd_write_cmd_data(lcd_no, newleft & 0xff);          // left low 8 b
    qosa_lcd_write_cmd_data(lcd_no, (newright >> 8) & 0xff);  // right high 8 b
    qosa_lcd_write_cmd_data(lcd_no, newright & 0xff);         // right low 8 b
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0x2b);                          // set vert start , end (top, bot)
    qosa_lcd_write_cmd_data(lcd_no, (newtop >> 8) & 0xff);     // top high 8 b
    qosa_lcd_write_cmd_data(lcd_no, newtop & 0xff);            // top low 8 b
    qosa_lcd_write_cmd_data(lcd_no, (newbottom >> 8) & 0xff);  // bot high 8 b
    qosa_lcd_write_cmd_data(lcd_no, newbottom & 0xff);         // bot low 8 b
    qosa_lcd_flush_cmd(lcd_no);

    qosa_lcd_write_cmd(lcd_no, 0x2c);  // recover memory write mode
}

/**
 * @brief Function will running when wants to read LCD ID
 *
 * @param[in] qosa_lcd_channel_e lcd_no
 *          - LCD channel number selection
 */
static qosa_uint32_t unir_lcd_drv_st7789v_Read_Id(qosa_lcd_channel_e lcd_no)
{
    qosa_uint32_t ret_id = 0;
    qosa_uint8_t  id[3] = {0};
    lcd_drv_log("lcd:st7789vReadId");

    qosa_lcd_read_cmd_data(lcd_no, 0x04, id, 3, 1);

    lcd_drv_log("lcd:st7789vReadId ID: 0x%0x, 0x%x, 0x%0x", id[0], id[1], id[2]);
    ret_id = ((id[2] << 16) | (id[1] << 8) | id[0]);

    if (g_unir_lcd_drv_st7789v_info.lcd_device_id == ret_id)
    {
        lcd_drv_log("lcd:   is using !");
    }

    return ret_id;
}
