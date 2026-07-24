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
#ifndef __LCD_DRV_ST7567_H__
#define __LCD_DRV_ST7567_H__

#include "qosa_def.h"
#include "qosa_lcd.h"
#include <stdbool.h>

/*===========================================================================
 *  Macro Definition
 ===========================================================================*/
#define LCD_WIDTH       128
#define LCD_HEIGHT      64
#define LCD_PAGE        8
#define LCD_COLUMN      128
#define LCD_BUFF_SIZE   (LCD_WIDTH * LCD_PAGE)  // 1024 bytes

// Backlight brightness level
#define BL_LEVEL_MIN    0
#define BL_LEVEL_MAX    10

// Display direction definition
#define LCD_DIRECT_NORMAL   0   // SEG0->SEG131, COM0->COM63
#define LCD_DIRECT_MY       1   // COM63->COM0
#define LCD_DIRECT_MX       2   // SEG131->SEG0
#define LCD_DIRECT_MY_MX    3   // SEG131->SEG0, COM63->COM0

// ST7567 command definition
#define CMD_DISPLAY_ON          0xAF    // Display on
#define CMD_DISPLAY_OFF         0xAE    // Display off    
#define CMD_DISPLAY_NORMAL      0xA4    // Normal display mode
#define CMD_DISPLAY_ALL_ON      0xA5    // Display all points on
#define CMD_DISPLAY_NORMAL_MODE 0xA6    // Normal display mode
#define CMD_DISPLAY_REVERSE     0xA7    // Reverse display mode



#define CMD_ADC_NORMAL          0xA0    // Normal scan direction
#define CMD_ADC_REVERSE         0xA1    // Reverse scan direction
#define CMD_COM_NORMAL          0xC0    // COM output normal direction
#define CMD_COM_REVERSE         0xC8    // COM output reverse direction

#define CMD_BIAS_1_9            0xA2    // 1/9 bias (1/64 Duty)
#define CMD_BIAS_1_7            0xA3    // 1/7 bias (1/64 Duty)

#define CMD_RESET               0xE2    // Software reset
#define CMD_POWER_CTRL          0x2F    
#define CMD_BOOSTER_RATIO       0xF8    // Booster ratio set (2-byte command)
#define CMD_BOOSTER_RATIO_SET   0x00    // 
#define CMD_REGULATOR_RATIO     0x25
#define CMD_EV_MODE             0x81

#define CMD_SET_PAGE_BASE       0xB0
#define CMD_SET_COLUMN_HIGH     0x10
#define CMD_SET_COLUMN_LOW      0x00

// LCD channel selection
#define ST7567_LCD_CHANNEL      QOSA_LCD_SPECIAL_PORT


/*===========================================================================
 *  Driver Init API Functions
 ===========================================================================*/

/**
 * @brief Initialize backlight PWM
 */
void unir_lcd_drv_st7567_backlight_init(void);

/**
 * @brief Set backlight brightness
 * @param level Brightness level 0-10
 */
void unir_lcd_drv_st7567_set_backlight(uint8_t level);

/**
 * @brief Get current backlight brightness
 * @return Brightness level 0-10
 */
uint8_t unir_lcd_drv_st7567_get_backlight(void);

/**
 * @brief Clear screen
 * @param lcd_no LCD channel number
 */
void unir_lcd_drv_st7567_clear(qosa_lcd_channel_e lcd_no);

/**
 * @brief Set pixel
 * @param lcd_no LCD channel number
 * @param x X coordinate (0-127)
 * @param y Y coordinate (0-63)
 * @param val 0=off, 1=on
 * @return 0=success, -1=failure
 */
int32_t unir_lcd_drv_st7567_set_pixel(qosa_lcd_channel_e lcd_no, 
                                      uint16_t x, uint16_t y, uint8_t val);


/**
 * @brief Send data to LCD
 * @param lcd_no LCD channel number
 * @param column Column number (0-127)
 * @param page Page number (0-7)
 * @param buff Data buffer
 * @param size Data length
 */
void unir_lcd_drv_st7567_send_buffer(qosa_lcd_channel_e lcd_no, uint8_t column, 
                                 uint8_t page, void *buff, uint32_t size);

/**
 * @brief Close LCD
 * @param lcd_no LCD channel number
 */
void unir_lcd_drv_st7567_close(qosa_lcd_channel_e lcd_no);

#endif /* __LCD_DRV_ST7567_H__ */
