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

#include "qosa_sys.h"
#include "qosa_lcd.h"
#include "qosa_gpio.h"
#include "qosa_rtc.h"
#include "qosa_def.h"
#include "qosa_log.h"
#include "lcd_demo.h"
#include "lcd_drv_st7567.h"
#include "qosa_dev_eigen.h"
#include "qosa_pinctrl.h"
/*===========================================================================
 * Macro Definition
 ===========================================================================*/

/** @brief Log tag for LCD API module */
#define QOS_LOG_TAG LOG_TAG_LCD_API

/*===========================================================================
 * Global Variables
 ===========================================================================*/

/** @brief LCD demonstration task handle */
static qosa_task_t g_unir_lcd_st7567_demo_task = QOSA_NULL;

/** @brief Current active LCD test case */
static qosa_uint8_t g_lcd_test_case = QOSA_LCD_DEMO_PIXEL_TEST;

/** @brief Pixel color definitions for ST7567 (monochrome display) */
#define PIXEL_OFF   (0)     /**< Pixel off (black) */
#define PIXEL_ON    (1)     /**< Pixel on (white) */

/*===========================================================================
 * External API Functions / Variables
 ===========================================================================*/
static const uint8_t font8x16[96][16] = 
{
    // 0x20 - Space
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    // 0x30-0x39 - '0' to '9'
    {0x00,0x00,0x00,0x38,0x44,0x4C,0x54,0x64,0x44,0x44,0x44,0x38,0x00,0x00,0x00,0x00}, // 0
    {0x00,0x00,0x00,0x10,0x30,0x50,0x10,0x10,0x10,0x10,0x10,0x7C,0x00,0x00,0x00,0x00}, // 1
    {0x00,0x00,0x00,0x38,0x44,0x44,0x04,0x08,0x10,0x20,0x40,0x7C,0x00,0x00,0x00,0x00}, // 2
    {0x00,0x00,0x00,0x38,0x44,0x04,0x08,0x18,0x04,0x04,0x44,0x38,0x00,0x00,0x00,0x00}, // 3
    {0x00,0x00,0x00,0x08,0x18,0x28,0x48,0x48,0x7C,0x08,0x08,0x08,0x00,0x00,0x00,0x00}, // 4
    {0x00,0x00,0x00,0x7C,0x40,0x40,0x78,0x44,0x04,0x04,0x44,0x38,0x00,0x00,0x00,0x00}, // 5
    {0x00,0x00,0x00,0x18,0x20,0x40,0x78,0x44,0x44,0x44,0x44,0x38,0x00,0x00,0x00,0x00}, // 6
    {0x00,0x00,0x00,0x7C,0x04,0x08,0x10,0x10,0x20,0x20,0x40,0x40,0x00,0x00,0x00,0x00}, // 7
    {0x00,0x00,0x00,0x38,0x44,0x44,0x44,0x38,0x44,0x44,0x44,0x38,0x00,0x00,0x00,0x00}, // 8
    {0x00,0x00,0x00,0x38,0x44,0x44,0x44,0x3C,0x04,0x04,0x08,0x30,0x00,0x00,0x00,0x00}, // 9
    // 0x3A - ':' Colon
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x00}, // :
    // 0x41-0x46 - 'A' to 'F'
    {0x00,0x00,0x00,0x10,0x28,0x28,0x44,0x44,0x7C,0x44,0x44,0x44,0x00,0x00,0x00,0x00}, // A
    {0x00,0x00,0x00,0x78,0x44,0x44,0x44,0x78,0x44,0x44,0x44,0x78,0x00,0x00,0x00,0x00}, // B
    {0x00,0x00,0x00,0x38,0x44,0x40,0x40,0x40,0x40,0x40,0x44,0x38,0x00,0x00,0x00,0x00}, // C
    {0x00,0x00,0x00,0x78,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x78,0x00,0x00,0x00,0x00}, // D
    {0x00,0x00,0x00,0x7C,0x40,0x40,0x40,0x78,0x40,0x40,0x40,0x7C,0x00,0x00,0x00,0x00}, // E
    {0x00,0x00,0x00,0x7C,0x40,0x40,0x40,0x78,0x40,0x40,0x40,0x40,0x00,0x00,0x00,0x00}, // F
};

/*===========================================================================
 * Static Function Implementations
 ===========================================================================*/

 /**
 * @brief Display a single character on LCD
 *
 * @param x Start X coordinate
 * @param y Start Y coordinate
 * @param ch Character to display ('0'-'9', 'A'-'F')
 */
static void unir_lcd_draw_char(uint16_t x, uint16_t y, char ch)
{
    uint8_t char_index;
    uint8_t i, j;
    uint8_t pixel_val;
    
    // Calculate character index
    if (ch >= '0' && ch <= '9') 
    {
        char_index = ch - '0' + 1;
    } 
    else if (ch == ':') 
    {
        char_index = 11;  // Colon at index 11
    } 
    else if (ch >= 'A' && ch <= 'F') 
    {
        char_index = ch - 'A' + 12;  // 'A' should be at index 12 (colon occupies index 11)
    } 
    else if (ch == ' ') 
    {
        char_index = 0;
    } 
    else 
    {
        return;
    }

    
    // Draw 8x16 pixel character
    for (i = 0; i < 8; i++) {       // 8 columns
        for (j = 0; j < 16; j++) {   // 16 rows
            // Read font data, 1 bit per row
            pixel_val = (font8x16[char_index][j] >> (7 - i)) & 0x01;
            unir_lcd_drv_st7567_set_pixel(UNIR_LCD_PORT, x + i, y + j, pixel_val);
        }
    }
}


/**
 * @brief Perform pixel drawing test
 *
 * This function tests the pixel drawing functionality by:
 * - Drawing various patterns on the screen
 * - Testing individual pixel control
 * - Demonstrating coordinate system
 */
static void unir_pixel_test(void)
{
    uint8_t level;
    char level_char;
    
    QLOGI("Starting backlight PWM test...");
    
    // Display brightness level 1-5
    for (level = 1; level <= 5; level++) {
        unir_lcd_drv_st7567_clear(UNIR_LCD_PORT);
        
        // Display number in screen center
        level_char = '0' + level;
        unir_lcd_draw_char(60, 24, level_char);
        
        // Set corresponding backlight brightness (level * 2 maps to 0-10)
        unir_lcd_drv_st7567_set_backlight(level * 2);
        
        QLOGI("Displaying level: %d, backlight: %d", level, level * 2);
        qosa_task_sleep_ms(500);
    }
    
    // Clear screen
    unir_lcd_drv_st7567_clear(UNIR_LCD_PORT);
    
    // Restore to default brightness
    unir_lcd_drv_st7567_set_backlight(5);
    QLOGI("Backlight test completed.");
}

/**
 * @brief Display single character using batch transfer
 * @param x Start X coordinate
 * @param y Start Y coordinate
 * @param ch Character to display
 */
static void unir_lcd_draw_char_buffer(uint16_t x, uint16_t y, char ch)
{
    uint8_t char_index;
    uint8_t page, bit;
    uint8_t i, j;
    uint8_t pixel_val;
    uint8_t char_buffer_top[8] = {0};    // Top half page buffer
    uint8_t char_buffer_bottom[8] = {0}; // Bottom half page buffer
    
    // Calculate character index
    if (ch >= '0' && ch <= '9') 
    {
        char_index = ch - '0' + 1;
    } 
    else if (ch == ':') 
    {
        char_index = 11;  // Colon at index 11
    } 
    else if (ch >= 'A' && ch <= 'F') 
    {
        char_index = ch - 'A' + 12;  // 'A' should be at index 12 (colon occupies index 11)
    } 
    else if (ch == ' ') 
    {
        char_index = 0;
    } 
    else 
    {
        return;
    }
    
    // Calculate character start page and offset within page
    page = y / 8;
    bit = y % 8;
    
    // Build character data (8 columns x 16 rows)
    for (i = 0; i < 8; i++) {           // 8 columns
        for (j = 0; j < 16; j++) {       // 16 rows font height
            pixel_val = (font8x16[char_index][j] >> (7 - i)) & 0x01;
            if (!pixel_val) continue;
            
            if (j < (8 - bit)) {
                // Upper part in current page
                char_buffer_top[i] |= (pixel_val << (bit + j));
            } else {
                // Lower part in next page
                char_buffer_bottom[i] |= (pixel_val << (bit + j - 8));
            }
        }
    }
    
    // Send top half page data
    unir_lcd_drv_st7567_send_buffer(UNIR_LCD_PORT, x, page, char_buffer_top, 8);
    
    // Send bottom half page data (if character spans two pages)
    if ((page + 1) < LCD_PAGE) {
        unir_lcd_drv_st7567_send_buffer(UNIR_LCD_PORT, x, page + 1, char_buffer_bottom, 8);
    }
}


/**
 * @brief Display multi-digit number (right-aligned)
 * @param x Start X coordinate
 * @param y Start Y coordinate
 * @param num Number to display (0-999)
 * @param digits Number of digits to display (1-3)
 */
static void unir_lcd_draw_number(uint16_t x, uint16_t y, uint16_t num, uint8_t digits)
{
    char buf[4];
    uint8_t i;
    
    // Limit number range
    if (num > 999) num = 999;
    if (digits > 3) digits = 3;
    
    // Format number as string
    if (digits == 3) {
        buf[0] = '0' + (num / 100) % 10;
        buf[1] = '0' + (num / 10) % 10;
        buf[2] = '0' + num % 10;
        buf[3] = '\0';
    } else if (digits == 2) {
        buf[0] = '0' + (num / 10) % 10;
        buf[1] = '0' + num % 10;
        buf[2] = '\0';
    } else {
        buf[0] = '0' + num % 10;
        buf[1] = '\0';
    }
    
    // Display each character sequentially (each character 8 pixels wide)
    for (i = 0; i < digits; i++) {
        unir_lcd_draw_char_buffer(x + i * 8, y, buf[i]);
    }
}

/**
 * @brief Batch transfer test example - Counter display
 */
static void unir_lcd_send_buffer_test(void)
{
    uint16_t count = 0;
    
    QLOGI("Starting counter test...");
    
    while (1) {
        // Clear screen
        unir_lcd_drv_st7567_clear(UNIR_LCD_PORT);
        
        // Display "LCD:"
        unir_lcd_draw_char_buffer(20, 24, 'A');
        unir_lcd_draw_char_buffer(28, 24, 'B');  // Need to add letter support
        unir_lcd_draw_char_buffer(36, 24, 'C');
        unir_lcd_draw_char_buffer(44, 24, ':');
        
        // Display 3-digit counter
        unir_lcd_draw_number(52, 24, count, 3);
        
        QLOGI("Count: %03d", count);
        
        // Increment counter
        count++;
        if (count >= 100) {
            count = 0;  // Reset and recount
        }
        
        qosa_task_sleep_ms(500);  // Update every 500ms
    }
}


/**
 * @brief Perform clear screen test
 *
 * This function tests the clear screen functionality
 */
static void unir_clear_test(void)
{
    QLOGI("Starting clear screen test...");
    
    // Fill screen with pixels
    uint16_t x, y;
    for (x = 0; x < LCD_WIDTH; x++) {
        for (y = 0; y < LCD_HEIGHT; y++) {
            unir_lcd_drv_st7567_set_pixel(UNIR_LCD_PORT, x, y, PIXEL_ON);
        }
    }
    QLOGI("Screen filled with pixels");
    qosa_task_sleep_ms(2000);
    
    // Clear screen
    QLOGI("Clearing screen...");
    unir_lcd_drv_st7567_clear(UNIR_LCD_PORT);
    qosa_task_sleep_ms(1000);
    
    QLOGI("Clear screen test completed.");
}

/**
 * @brief Main LCD demonstration process
 *
 * This function implements the main demonstration logic including:
 * - Hardware initialization
 * - LCD controller configuration
 * - Test case execution based on current selection
 *
 * @param ctx Context pointer (unused in this implementation)
 */
static void unir_lcd_7567_demo_process(void *ctx)
{
    qosa_uint32_t       ret = 0;
    qosa_uint32_t       lcd_id = 0;
    qosa_lcd_drv_cfg_t *lcd_cfg = QOSA_NULL;

    // Delay 3 seconds to allow other system components to initialize
    qosa_task_sleep_ms(3000);
    qosa_gpio_set_voltage(VOL_3_30V);

    /* Configure GPIO pins for 4-Wire SPI mode */
    qosa_pin_set_func(UNIR_LCD_RST_PIN, UNIR_LCD_RST_FUNC);    // LCD Reset pin
    qosa_pin_set_func(UNIR_LCD_CS_PIN, UNIR_LCD_CS_FUNC);      // LCD Chip Select pin
    qosa_pin_set_func(UNIR_LCD_DS_PIN, UNIR_LCD_DS_FUNC);      // LCD Data/Command Select pin
    qosa_pin_set_func(UNIR_LCD_DOUT_PIN, UNIR_LCD_DOUT_FUNC);  // LCD Data Output pin
    qosa_pin_set_func(UNIR_LCD_CLK_PIN, UNIR_LCD_CLK_FUNC);    // LCD Clock pin

    /* Configure LCD driver chip settings */
    qosa_lcd_ioctl(UNIR_LCD_PORT, QOSA_LCD_IOCTL_SET_DRV_CFG, (void *)UNIR_LCD_ST7567_DRV_CFG_ADDR);

    QLOGI("Initializing ST7567 LCD controller...");
    ret = qosa_lcd_init(UNIR_LCD_PORT);
    if (ret != QOSA_LCD_SUCCESS)
    {
        QLOGE("LCD initialization failed with error code: %d", ret);
        return;
    }

    QLOGV("LCD demonstration test sequence starting...");

    // Main demonstration loop
    while (1)
    {
        qosa_task_sleep_ms(1000);

        switch (g_lcd_test_case)
        {
            case QOSA_LCD_DEMO_PIXEL_TEST: {
                // Pixel drawing test
                QLOGI("=== Running Pixel Test ===");
                unir_pixel_test();
            }
            break;

            case QOSA_LCD_DEMO_SEND_BUFFER_TEST: {
                 // Pixel buffer send test
                QLOGI("=== Running Backlight Test ===");
                unir_lcd_send_buffer_test();
            }
            break;

            case QOSA_LCD_DEMO_CLEAR_TEST: {
                // Clear screen test
                QLOGI("=== Running Clear Screen Test ===");
                unir_clear_test();
            }
            break;

            case QOSA_LCD_DEMO_READ_ID: {
                // Read LCD controller ID test
                lcd_cfg = UNIR_LCD_ST7567_DRV_CFG_ADDR;
                lcd_id = lcd_cfg->operation->readId(UNIR_LCD_PORT);
                QLOGI("LCD Controller ID: 0x%04x", lcd_id);
            }
            break;

            default:
                QLOGW("Unknown test case: %d", g_lcd_test_case);
                break;
        }
        
        // Delay between test cycles
        qosa_task_sleep_ms(3000);
    }
}

/*===========================================================================
 * Public API Functions
 ===========================================================================*/

/**
 * @brief Switch between different LCD test cases
 *
 * This function allows dynamic switching between various LCD demonstration
 * test cases during runtime.
 *
 * @param caseNo The test case number to activate
 *
 * @return None
 */
// static void unir_demo_spi_case_switch(qosa_lcd_demo_case_e caseNo)
// {
//     // Update current active test case
//     g_lcd_test_case = caseNo;
//     QLOGI("LCD test case switched to: %d", caseNo);
// }

/**
 * @brief Initialize and start LCD demonstration application
 *
 * This function creates the LCD demonstration task which will handle
 * all LCD operations and test sequences. The task runs with normal
 * priority and has its own stack space.
 *
 * @return None
 */
void unir_lcd_st7567_demo_init(void)
{
    QLOGI("Initializing ST7567 LCD Demonstration Application");

    if (g_unir_lcd_st7567_demo_task == QOSA_NULL)
    {
        /* Create LCD demonstration task */
        qosa_task_create(
            &g_unir_lcd_st7567_demo_task,
            CONFIG_UNIRTOS_LCD_DEMO_TASK_STACK_SIZE,
            UNIR_LCD_DEMO_TASK_PRIO,
            "lcd_st7567_demo",
            unir_lcd_7567_demo_process,
            QOSA_NULL,
            1
        );

        QLOGI("LCD demonstration task created successfully");
    }
    else
    {
        QLOGW("LCD demonstration task already exists");
    }
}
