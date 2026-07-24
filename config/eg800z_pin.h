/*****************************************************************/ /**
* @file eg800z_pin.h
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
#ifndef __EG800Z_PIN_H__
#define __EG800Z_PIN_H__

#include "qosa_def.h"
#include "qosa_lcd.h"

extern qosa_lcd_drv_cfg_t g_unir_lcd_drv_st7789v_info;
extern qosa_lcd_drv_cfg_t g_unir_lcd_drv_st7567_info;

#define UNIR_LCD_ST7789V_DRV_CFG_ADDR &g_unir_lcd_drv_st7789v_info
#define UNIR_LCD_ST7567_DRV_CFG_ADDR  &g_unir_lcd_drv_st7567_info

#define UNIR_LCD_PORT         QOSA_LCD_SPECIAL_PORT

// EG800Z LSPI PIN
#define UNIR_LCD_RST_PIN      49  // GPIO Control reset
#define UNIR_LCD_RST_FUNC     (0)

#define UNIR_LCD_DS_PIN       51
#define UNIR_LCD_DS_FUNC      (2)

#define UNIR_LCD_CS_PIN       52
#define UNIR_LCD_CS_FUNC      (1)

#define UNIR_LCD_DOUT_PIN     50
#define UNIR_LCD_DOUT_FUNC    (1)

#define UNIR_LCD_DIN_PIN      78
#define UNIR_LCD_DIN_FUNC     (1)

#define UNIR_LCD_CLK_PIN      53
#define UNIR_LCD_CLK_FUNC     (1)

#endif /* __EC800Z_PIN_H__ */