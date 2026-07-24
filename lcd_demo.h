/*****************************************************************/ /**
* @file lcd_demo.h
* @brief
* @author bronson.zhan@quectel.com
* @date 2025-11-18
*
* @copyright Copyright (c) 2023 Quectel Wireless Solution, Co., Ltd.
* All Rights Reserved. Quectel Wireless Solution Proprietary and Confidential.
*
* @par EDIT HISTORY FOR MODULE
* <table>
* <tr><th>Date <th>Version <th>Author <th>Description"
* <tr><td>2025-11-18 <td>1.0 <td>Bronson.Zhan <td> Init
* </table>
**********************************************************************/
#ifndef __LCD_DEMO_H__
#define __LCD_DEMO_H__

#include "qosa_def.h"
#include "qosa_sys.h"

/*===========================================================================
 * Macro Definition
 ===========================================================================*/

/** @brief Demo task stack size configuration */
#define CONFIG_UNIRTOS_LCD_DEMO_TASK_STACK_SIZE 4096

/** @brief Demo task priority configuration */
#define UNIR_LCD_DEMO_TASK_PRIO                 QOSA_PRIORITY_NORMAL

/** @brief LCD channel configuration */
#define UNIR_LCD_DEMO_CHANNEL                   0

/** @brief Color definitions in RGB565 format */
#define RED                                     (0xF800) /**< Pure red color */
#define GREEN                                   (0x07E0) /**< Pure green color */
#define BLUE                                    (0x001F) /**< Pure blue color */
#define WHITE                                   (0xFFFF) /**< Pure white color */
#define BLACK                                   (0x0000) /**< Pure black color */
#define YELLOW                                  (0xFFE0) /**< Yellow color */
#define CYAN                                    (0x07FF) /**< Cyan color */
#define MAGENTA                                 (0xF81F) /**< Magenta color */

/** @brief LCD module selection - choose the module that you are using */
#define LCD_EC800Z_CNLD_PIN                     0 /**< EC800Z CNLD pin configuration */
#define LCD_EG800Z_PIN                          1 /**< EG800Z pin configuration */

/** @brief Include appropriate pin configuration based on selected module */
#if (LCD_EC800Z_CNLD_PIN == 1)
#include "ec800z_cnld_pin.h"
#elif (LCD_EG800Z_PIN == 1)
#include "eg800z_pin.h"
#endif

/*===========================================================================
 * Enum Definition
 ===========================================================================*/

/**
 * @enum qosa_lcd_demo_case_e
 * @brief LCD demonstration test cases enumeration
 */
typedef enum
{
    QOSA_LCD_DEMO_API_TEST = 0, /**< Comprehensive API functionality test */
    QOSA_LCD_DEMO_READ_ID,      /**< Read LCD controller ID test */
    QOSA_LCD_DEMO_SHOW_PICTURE, /**< Display colors and UniRTOS logo test */
    QOSA_LCD_DEMO_FLESH_RATE,   /**< Screen refresh rate measurement test */

    // st7567
    QOSA_LCD_DEMO_PIXEL_TEST ,      /**< Pixel drawing test */
    QOSA_LCD_DEMO_SEND_BUFFER_TEST, /**< Send Buffer drawing test */
    QOSA_LCD_DEMO_CLEAR_TEST,       /**< Clear screen test */
    
    QOSA_LCD_DEMO_MAX,              /**< Maximum test case identifier */
} qosa_lcd_demo_case_e;

/*===========================================================================
 * Function Declaration
 ===========================================================================*/

/**
 * @brief Initialize LCD demonstration application
 *
 * This function creates and starts the LCD demo task which handles
 * all LCD demonstration functionalities including display tests,
 * color rendering, and performance measurements.
 */
void unir_lcd_demo_init(void);


//LCD_ST7567
void unir_lcd_st7567_demo_init(void);

#endif /* __LCD_DEMO_H__ */
