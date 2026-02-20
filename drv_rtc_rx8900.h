/**
 * @file drv_rtc_rx8900.h
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief RTCドライバ (RX8900)
 * @version 0.1
 * @date 2026-02-20
 * 
 * @copyright Copyright (c) 2025 Chimipupu All Rights Reserved.
 * 
 */

#ifndef DRV_RTC_RX8900_H
#define DRV_RTC_RX8900_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define RTC_RX8900_I2C_ADDR             0x32 // RX8900のI2Cアドレス

#define RTC_RX8900_REG_SEC              0x00 // 秒
#define RTC_RX8900_REG_MIN              0x01 // 分
#define RTC_RX8900_REG_HOUR             0x02 // 時
#define RTC_RX8900_REG_WEEK             0x03 // 曜日
#define RTC_RX8900_REG_DAY              0x04 // 日
#define RTC_RX8900_REG_MONTH            0x05 // 月
#define RTC_RX8900_REG_YEAR             0x06 // 年
#define RTC_RX8900_REG_RAM              0x07
#define RTC_RX8900_REG_MIN_ALM          0x08 // アラーム分
#define RTC_RX8900_REG_HOUR_ALM         0x09 // アラーム時
#define RTC_RX8900_REG_WEEK_DAY_ALM     0x0A // アラーム曜日
#define RTC_RX8900_REG_TIMER_CNT_0      0x0B // タイマーカウンタ0
#define RTC_RX8900_REG_TIMER_CNT_1      0x0C // タイマーカウンタ1
#define RTC_RX8900_REG_EXTENSION        0x0D // 拡張レジスタ
#define RTC_RX8900_REG_FLAG             0x0E // フラグレジスタ
#define RTC_RX8900_REG_CTRL             0x0F // コントロールレジスタ

typedef void (*p_drc_i2c_init)(uint8_t slave_addr);
typedef void(*p_func_drv_i2c_write)(uint8_t data);
typedef uint8_t(*p_func_drv_i2c_read)(void);

typedef struct {
    bool is_interrupt;                     // 割り込み有効フラグ
    p_drc_i2c_init p_func_i2c_init;        // 呼び元のI2C初期化関数ポインタ
    p_func_drv_i2c_write p_func_i2c_write; // 呼び元のI2C書き込み関数ポインタ
    p_func_drv_i2c_read p_func_i2c_read;   // 呼び元のI2C読み取り関数ポインタ
} rtc_rx8900_config_t;

typedef struct {
    uint8_t sec;      // 秒 (0-59)
    uint8_t min;      // 分 (0-59)
    uint8_t hour;     // 時 (0-23)
    uint8_t day;      // 日 (1-31)
    uint8_t weekday;  // 曜日 (0-6, 0=日曜)
    uint8_t month;    // 月 (1-12)
    uint16_t year;    // 年 (2000-2099)
} rtc_rx8900_time_t;

bool drv_rtc_rx8900_init(rtc_rx8900_config_t *p_config);
void drv_rtc_rx8900_set_time(rtc_rx8900_time_t *p_time);
void drv_rtc_rx8900_get_time(rtc_rx8900_time_t *p_time);
void drv_rtc_rx8900_set_alarm(rtc_rx8900_time_t *p_time);

#endif // DRV_RTC_RX8900_H