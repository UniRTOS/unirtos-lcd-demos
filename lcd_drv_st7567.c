/*****************************************************************/ /**
* @file unir_lcd_drv_st7567.c
* @brief ST7567 LCD Driver Implementation for 128x64 Dot Matrix LCD (SPI Mode)
* @author NIKE.BU@quectel.com
* @date 2026-03-09
*
* @copyright Copyright (c) 2023 Quectel Wireless Solution, Co., Ltd.
* All Rights Reserved. Quectel Wireless Solution Proprietary and Confidential.
*
* @par EDIT HISTORY FOR MODULE
* <table>
* <tr><th>Date <th>Version <th>Author <th>Description
* <tr><td>2026-03-09 <td>1.0 <td>Author Name <td> Init
* </table>
**********************************************************************/
#include "qosa_def.h"
#include "qosa_sys.h"
#include "qosa_gpio.h"
#include "qosa_pwm.h"
#include "qosa_log.h"
#include "qosa_lcd.h"
#include "qosa_spi.h"
#include "lcd_drv_st7567.h"
#include <string.h>
#include <stdlib.h>
#include "qosa_pinctrl.h"
/*===========================================================================
 *  Macro Definition
 ===========================================================================*/
#define lcd_drv_log(...) QOSA_LOG_D(LOG_TAG_LCD_API, ##__VA_ARGS__)

// PWM Configuration
#define LCD_PIN_BL      100  // PWM backlight pin

#define PWM_CHANNEL     0
#define PWM_PERIOD      1000


// ST7567 uses SPI port 1
#define ST7567_USE_SPI_PORT  QOSA_LCD_SPECIAL_PORT

/*===========================================================================
 *  Static Variables
 ===========================================================================*/
// LCD display buffer
static uint8_t g_display_buffer[LCD_BUFF_SIZE];

// Current display parameters
static uint8_t  g_backlight_level   = 5;
static bool     g_lcd_is_active     = false;
// static uint8_t  g_display_direction = LCD_DIRECT_NORMAL;

// Current LCD channel
static qosa_lcd_channel_e g_lcd_channel = ST7567_USE_SPI_PORT;

/*===========================================================================
 *  Static Function Prototypes (Driver Callbacks)
 ===========================================================================*/
static void unir_lcd_drv_st7567_init(qosa_lcd_channel_e lcd_no);
static void unir_lcd_drv_st7567_deInit(qosa_lcd_channel_e lcd_no);
// static void unir_lcd_drv_st7567_enterSleep(qosa_lcd_channel_e lcd_no, qosa_bool_t is_sleep);
static void unir_lcd_drv_st7567_set_window(
                    qosa_lcd_channel_e lcd_no, 
                                uint16_t left, 
                                uint16_t  top,
                                uint16_t right, 
                                uint16_t bottom);
static uint32_t unir_lcd_drv_st7567_read_id(qosa_lcd_channel_e lcd_no);

/*===========================================================================
 *  Driver Operations Structure
 ===========================================================================*/
static qosa_lcd_operations_t g_unir_lcd_drv_st7567_operations = {
    .init               = unir_lcd_drv_st7567_init,
    .deInit             = unir_lcd_drv_st7567_deInit,
    // .enterSleep         = unir_lcd_drv_st7567_enterSleep,
    .setDisplayWindow   = unir_lcd_drv_st7567_set_window,
    .readId             = unir_lcd_drv_st7567_read_id
};

/*===========================================================================
 *  Driver Configuration Structure
 ===========================================================================*/
qosa_lcd_drv_cfg_t g_unir_lcd_drv_st7567_info = {
    .lcd_channel = ST7567_USE_SPI_PORT,
    
    .lcd_name = (qosa_uint8_t *)"ST7567",
    .lcd_device_id = 0x7567,
    
    .lcd_width = LCD_WIDTH,
    .lcd_height = LCD_HEIGHT,
    
    .bus_mode = QOSA_LCD_BUS_MODE_4_WIRE_I_SPI,
    .lcd_input_format = QOSA_LCD_INPUT_FORMAT_RGB565,
    .lcd_output_format = QOSA_LCD_OUTPUT_FORMAT_RGB565,
    
    .operation = &g_unir_lcd_drv_st7567_operations,
    .lcd_write_frequence =   QOSA_SPI_CLK_26MHZ,
    .lcd_read_frequence  =   QOSA_SPI_CLK_3_25MHZ,
    .lcd_reset_func = {QOSA_GPIO_15, 10},  

};

/*===========================================================================
 *  PWM Backlight Control
 ===========================================================================*/
/**
 * @brief Initialize PWM backlight
 */
 void unir_lcd_drv_st7567_backlight_init(void)
{
    qosa_pwm_info_t pwm_info;
    
    lcd_drv_log("Initializing PWM backlight on pin %d", LCD_PIN_BL);
    
    qosa_pin_set_func(LCD_PIN_BL, 5);  // PWM0 function
    
    pwm_info.clk_src = QOSA_FCLK_SEL_26M;
    pwm_info.high_one_cycle_duration = 500;
    pwm_info.total_one_cycle_duration = PWM_PERIOD;
    pwm_info.pwm_psc = 1;
    
    qosa_pwm_config(PWM_CHANNEL, &pwm_info);
    qosa_pwm_enable(PWM_CHANNEL, 500);  // 50% duty
}

/**
 * @brief Set backlight level
 *
 * @param[in] uint8_t level
 *          - Backlight level

 */
void unir_lcd_drv_st7567_set_backlight(uint8_t level)
{
    uint16_t duty;
    
    if (level > BL_LEVEL_MAX) level = BL_LEVEL_MAX;
    g_backlight_level = level;
    
    duty = (level * PWM_PERIOD) / BL_LEVEL_MAX;
    qosa_pwm_enable(PWM_CHANNEL, duty);
}

/**
 * @brief Get current backlight level
 *
 * @return uint8_t
 */
uint8_t unir_lcd_drv_st7567_get_backlight(void)
{
    return g_backlight_level;
}


/*===========================================================================
 *  Static API Functions
 ===========================================================================*/

/**
 * @brief Function will running after LCD init
 *
 * @param[in] qosa_lcd_channel_e lcd_no
 *          - LCD channel number selection
 */

static void unir_lcd_drv_st7567_init(qosa_lcd_channel_e lcd_no)
{
    lcd_drv_log("lcd:    _st7567Init ");

    g_lcd_channel = lcd_no;

    qosa_lcd_write_cmd(lcd_no, CMD_RESET);  // Display off
    qosa_lcd_flush_cmd(lcd_no);
    qosa_task_sleep_ms(500);

    /*Display OFF*/  
    qosa_lcd_write_cmd(lcd_no, CMD_DISPLAY_OFF);
    qosa_lcd_flush_cmd(lcd_no);

    /*Set The DDRAM Address SEG output Correspondence (ADC=0)SEG0--SEG131*/
    qosa_lcd_write_cmd(lcd_no, CMD_ADC_NORMAL);
    qosa_lcd_flush_cmd(lcd_no);

    /*Set COM Output scan direction (SHL=1)COM63-COM0*/ 
    qosa_lcd_write_cmd(lcd_no, CMD_COM_REVERSE);
    qosa_lcd_flush_cmd(lcd_no);

    /*The DDRAM Display Start Line Address (32)*/
    qosa_lcd_write_cmd(lcd_no, 0x40);
    qosa_lcd_flush_cmd(lcd_no);

    /*Set The LCD Display(Normal)*/ 
    qosa_lcd_write_cmd(lcd_no, CMD_DISPLAY_NORMAL_MODE);
    qosa_lcd_flush_cmd(lcd_no);

    /*Sets Display all points 0: normal display*/
    qosa_lcd_write_cmd(lcd_no, CMD_DISPLAY_NORMAL);
    qosa_lcd_flush_cmd(lcd_no);

    /*Set The LCD Display Driver Voltage Bias Ratio (1/9)*/
    qosa_lcd_write_cmd(lcd_no, CMD_BIAS_1_9);
    qosa_lcd_flush_cmd(lcd_no);

    /*Booster Ratio Register Set*/ 
    qosa_lcd_write_cmd(lcd_no, CMD_BOOSTER_RATIO);
    qosa_lcd_flush_cmd(lcd_no);
    qosa_lcd_write_cmd(lcd_no, CMD_BOOSTER_RATIO_SET);
    qosa_lcd_flush_cmd(lcd_no);

    /*Booster Circuit ON,Voltage Regulator Circuit ON,Voltage Follower Circuit ON*/ 
    qosa_lcd_write_cmd(lcd_no, CMD_POWER_CTRL);
    qosa_lcd_flush_cmd(lcd_no);

    /*Select Internal Resistor Rate (Rb/Ra)*/ 
    qosa_lcd_write_cmd(lcd_no, CMD_REGULATOR_RATIO);
    qosa_lcd_flush_cmd(lcd_no);

    /*Set Electronic volume Register (32)*/
    qosa_lcd_write_cmd(lcd_no, CMD_EV_MODE);
    qosa_lcd_flush_cmd(lcd_no);
    qosa_lcd_write_cmd(lcd_no, 0x16);
    qosa_lcd_flush_cmd(lcd_no);

    /*Display ON*/  
    qosa_lcd_write_cmd(lcd_no, CMD_DISPLAY_ON);
    qosa_lcd_flush_cmd(lcd_no);

    g_lcd_is_active = true;

    // Initialize backlight
    unir_lcd_drv_st7567_backlight_init();
    unir_lcd_drv_st7567_set_backlight(5);  // Default brightness 50%
    
    // Clear screen
    unir_lcd_drv_st7567_clear(lcd_no);


}

/**
 * @brief Function will running before LCD deinit
 *
 * @param[in] qosa_lcd_channel_e lcd_no
 *          - LCD channel number selection
 */
static void unir_lcd_drv_st7567_deInit(qosa_lcd_channel_e lcd_no)
{
    lcd_drv_log("lcd:   _st7567_deInit");
    
    // Turn off display
    qosa_lcd_write_cmd(lcd_no, CMD_DISPLAY_OFF);
    qosa_lcd_flush_cmd(lcd_no);
    
    // Turn off backlight
    unir_lcd_drv_st7567_set_backlight(0);
    
    g_lcd_is_active = false;
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
// static void unir_lcd_drv_st7567_enterSleep(qosa_lcd_channel_e lcd_no, qosa_bool_t is_sleep)
// {
//     lcd_drv_log("lcd:   _st7567SleepIn, is_sleep = %d", is_sleep);

//     if(is_sleep)
//     {
//         qosa_lcd_write_cmd(lcd_no, CMD_DISPLAY_OFF);
//         qosa_lcd_flush_cmd(lcd_no);
//         g_lcd_is_active = false;
//     }
//     else
//     {
//         qosa_lcd_write_cmd(lcd_no, CMD_RESET);
//         qosa_lcd_flush_cmd(lcd_no);
//         qosa_task_sleep_ms(10);
//         qosa_lcd_write_cmd(lcd_no, CMD_DISPLAY_OFF);
//         qosa_lcd_flush_cmd(lcd_no);
//         qosa_lcd_write_cmd(lcd_no, CMD_DISPLAY_NORMAL);
//         qosa_lcd_flush_cmd(lcd_no);
//         qosa_lcd_write_cmd(lcd_no, CMD_POWER_CTRL);
//         qosa_lcd_flush_cmd(lcd_no);
//         qosa_task_sleep_ms(10);
//         qosa_lcd_write_cmd(lcd_no, CMD_DISPLAY_ON);
//         qosa_lcd_flush_cmd(lcd_no);
//         // drv_lcd_st7567_update();
//         g_lcd_is_active = true;
//     }
// }

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
static void unir_lcd_drv_st7567_set_window(
    qosa_lcd_channel_e lcd_no, 
    qosa_uint16_t  left, qosa_uint16_t    top,                           
    qosa_uint16_t right, qosa_uint16_t bottom)
{
    lcd_drv_log("lcd:st7567_SetDisplayWindow \
                L = %d, top = %d, \
                R = %d, bot = %d", left, top, right, bottom);
    
                
}


/**
 * @brief Function will running when wants to read LCD ID
 *
 * @param[in] qosa_lcd_channel_e lcd_no
 *          - LCD channel number selection
 */
static qosa_uint32_t unir_lcd_drv_st7567_read_id(qosa_lcd_channel_e lcd_no)
{
    qosa_uint32_t ret_id = 0;
    
    lcd_drv_log("lcd:st7567ReadId");

    // The ST7567 does not support standard ID reading and returns the configured device ID.
    ret_id = g_unir_lcd_drv_st7567_info.lcd_device_id;
    
    lcd_drv_log("lcd:st7567ReadId ID: 0x%04x", ret_id);

    return ret_id;

}

/**
 * @brief Function to close and deinitialize the ST7567 LCD
 *
 * @param[in] qosa_lcd_channel_e lcd_no
 *          - LCD channel number selection
 */

// void unir_lcd_drv_st7567_close(qosa_lcd_channel_e lcd_no)
// {
//     lcd_drv_log("lcd:     ST7567LCD close");
//     g_unir_lcd_drv_st7567_operations.enterSleep(true);
//     unir_lcd_drv_st7567_set_backlight(0);
// }


/*===========================================================================
 *  Display Functions
 ===========================================================================*/
/**
 * @brief Set page address and column address
 * @param lcd_no LCD channel number
 * @param page Page address
 * @param col Column address
 */
static void lcd_set_page_column(qosa_lcd_channel_e lcd_no, uint8_t page, uint8_t col)
{
    qosa_lcd_write_cmd(lcd_no, CMD_SET_PAGE_BASE | (page & 0x0F));
    qosa_lcd_flush_cmd(lcd_no);
    qosa_lcd_write_cmd(lcd_no, CMD_SET_COLUMN_HIGH | ((col >> 4) & 0x0F));
    qosa_lcd_flush_cmd(lcd_no);
    qosa_lcd_write_cmd(lcd_no, CMD_SET_COLUMN_LOW | (col & 0x0F));
    qosa_lcd_flush_cmd(lcd_no);
}

/**
 * @brief Write display data
 * @param lcd_no LCD channel number
 * @param data Data buffer (buffer accepts max 64 bytes)
 * @param length Data length
 */
static void lcd_write_display_data(qosa_lcd_channel_e lcd_no, uint8_t *data, uint16_t length)
{ 
    uint16_t i;
    
    for (i = 0; i < length; i++) 
    {
        qosa_lcd_write_cmd_data(lcd_no, data[i]);
    }
    qosa_lcd_flush_cmd(lcd_no);
}


/**
 * @brief Clear screen
 * @param lcd_no LCD channel number
 */
void unir_lcd_drv_st7567_clear(qosa_lcd_channel_e lcd_no)
{ 
    uint8_t page;
    uint8_t zero_data[LCD_COLUMN] = {0};
    
    for (page = 0; page < LCD_PAGE; page++) {
        lcd_set_page_column(lcd_no, page, 0);
        lcd_write_display_data(lcd_no, zero_data, LCD_COLUMN);
    }
    
    memset(g_display_buffer, 0, LCD_BUFF_SIZE);
} 


/**
 * @brief Set pixel
 * @param lcd_no LCD channel number
 * @param x Column number (0-127)
 * @param y Page number (0-7)
 * @param val Pixel value (0-1)
 */
int32_t unir_lcd_drv_st7567_set_pixel(qosa_lcd_channel_e lcd_no, 
                                  uint16_t x, uint16_t y, uint8_t val)
{
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT) 
    {
        lcd_drv_log("set_pixel failed: coordinate out of range!");
        return -1;
    }
    
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    uint16_t buf_index = page * LCD_COLUMN + x;
    
    if (val) 
    {
        g_display_buffer[buf_index] |= (1 << bit);
    } 
    else 
    {
        g_display_buffer[buf_index] &= ~(1 << bit);
    }
    
    // Update immediately
    lcd_set_page_column(lcd_no, page, x);
    lcd_write_display_data(lcd_no, &g_display_buffer[buf_index], 1);
    
    return 0;
}

/**
 * @brief Send data to LCD
 * @param lcd_no LCD channel number
 * @param column Column number (0-127)
 * @param page Page number (0-7)
 * @param buff Data buffer
 * @param size Data length
 */
void unir_lcd_drv_st7567_send_buffer(qosa_lcd_channel_e lcd_no, uint8_t column, uint8_t page, void *buff, uint32_t size)
{
    if (page >= LCD_PAGE) 
    {
        lcd_drv_log("send_buffer: page out of range");
        return;
    }
    
    if ((column + size) > LCD_COLUMN) 
    {
        lcd_drv_log("send_buffer: column+size out of range");
        return;
    }
    
    lcd_set_page_column(lcd_no, page, column);
    lcd_write_display_data(lcd_no ,(uint8_t *)buff, size);
    
    memcpy(&g_display_buffer[page * LCD_COLUMN + column], buff, size);
}





















































































