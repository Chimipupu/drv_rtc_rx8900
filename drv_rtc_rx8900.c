/**
 * @file drv_rtc_rx8900.c
 * @author Chimipupu(https://github.com/Chimipupu)
 * @brief RTCドライバ (RX8900)
 * @version 0.1
 * @date 2026-02-20
 * 
 * @copyright Copyright (c) 2025 Chimipupu All Rights Reserved.
 * 
 */

#include "drv_rtc_rx8900.h"

// 0〜99の10進数に対応するBCDの変換テーブル
const uint8_t g_dec_to_bcd_tbl[100] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99
};
// -----------------------------------------------------------
static rtc_rx8900_config_t s_config;

static void time_dec_to_bcd(rtc_rx8900_time_t *p_time);
static void time_bcd_to_dec(rtc_rx8900_time_t *p_time);
static void rx8900_start(void);
static void rx8900_stop(void);
static void rx8900_write_reg(uint8_t reg_addr, uint8_t data);
static uint8_t rx8900_read_reg(uint8_t reg_addr);
// -----------------------------------------------------------
// [Static関数]

static void time_dec_to_bcd(rtc_rx8900_time_t *p_time)
{
    // 10進数をBCDにテーブルで変換
    p_time->sec   = g_dec_to_bcd_tbl[p_time->sec];
    p_time->min   = g_dec_to_bcd_tbl[p_time->min];
    p_time->hour  = g_dec_to_bcd_tbl[p_time->hour];
    p_time->day   = g_dec_to_bcd_tbl[p_time->day];
    p_time->month = g_dec_to_bcd_tbl[p_time->month];
    p_time->year  = g_dec_to_bcd_tbl[p_time->year];
}

static void time_bcd_to_dec(rtc_rx8900_time_t *p_time)
{
    uint8_t i, j;
    uint8_t *p_ptr_u8;

    // BCDテーブルから10進数に変換
    p_ptr_u8 = (uint8_t *)p_time;
    for(j = 0; j < sizeof(rtc_rx8900_time_t); j++) {
        for(i = 0; i < 100; i++) {
            if(*p_ptr_u8 == g_dec_to_bcd_tbl[i]) {
                *p_ptr_u8 = i;
                break;
            }
        }
        p_ptr_u8++;
    }
}

static void rx8900_start(void)
{
    // 拡張レジスタ @Addr=0x0DのBit4のTEをセット
    s_config.p_func_i2c_write(RTC_RX8900_REG_EXTENSION);
    uint8_t reg = s_config.p_func_i2c_read();
    reg |= 0x10; // Bit4をセット
    s_config.p_func_i2c_write(RTC_RX8900_REG_EXTENSION);
    s_config.p_func_i2c_write(reg);
}

static void rx8900_stop(void)
{
    // 拡張レジスタ @Addr=0x0DのBit4のTEをクリア
    s_config.p_func_i2c_write(RTC_RX8900_REG_EXTENSION);
    uint8_t reg = s_config.p_func_i2c_read();
    reg &= ~0x10; // Bit4をクリア
    s_config.p_func_i2c_write(RTC_RX8900_REG_EXTENSION);
    s_config.p_func_i2c_write(reg);
}

static void rx8900_write_reg(uint8_t reg_addr, uint8_t data)
{
    // アドレス: 0x00~0x0F
    if(((reg_addr >= 0x00) && (reg_addr <= 0x0F))) {
        if(reg_addr == RTC_RX8900_REG_CTRL) {
            // Bit7のTESTビットは常に0
            s_config.p_func_i2c_write(data & 0x7F);
        } else {
            s_config.p_func_i2c_write(data);
        }
    }
}

static  uint8_t rx8900_read_reg(uint8_t reg_addr)
{
    uint8_t ret = 0x00;

    // アドレス: 0x00~0x0F
    if(((reg_addr >= 0x00) && (reg_addr <= 0x0F))) {
        s_config.p_func_i2c_write(reg_addr);
        ret = s_config.p_func_i2c_read();
    }

    return ret;
}

// -----------------------------------------------------------
// [API]

/**
 * @brief RTC RX8900初期化関数
 * @param p_config RX8900初期化構造体ポインタ
 * @return true 初期化成功
 * @return false 初期化失敗
 */
bool drv_rtc_rx8900_init(rtc_rx8900_config_t *p_config)
{
    bool ret = true;

    // 引数のチェック
    if (p_config == NULL || p_config->p_func_i2c_init == NULL || p_config->p_func_i2c_read == NULL || p_config->p_func_i2c_write == NULL) {
        ret = false;
        return ret;
    }

    memcpy(&s_config, p_config, sizeof(rtc_rx8900_config_t));
    s_config.p_func_i2c_init(RTC_RX8900_I2C_ADDR);

    // 拡張レジスタ @Addr=0x0D
    rx8900_write_reg(RTC_RX8900_REG_EXTENSION, 0x00);

    // フラグレジスタ @Addr=0x0E
    rx8900_write_reg(RTC_RX8900_REG_FLAG, 0x00);

    // コントロールレジスタ @Addr=0x0F
    // NOTE: Bit0のRESETを1にしてカウントをクリア
    rx8900_write_reg(RTC_RX8900_REG_CTRL, 0x01);

    // 時刻設定
    drv_rtc_rx8900_set_time(&s_config.init_time);

    // TODO: 割り込み/アラームの設定

    // RTCスタート(カウント開始)
    rx8900_start();

    return ret;
}

void drv_rtc_rx8900_set_time(rtc_rx8900_time_t *p_time)
{
    rtc_rx8900_time_t bcd_time;

    bcd_time = *p_time;
    time_dec_to_bcd(&bcd_time);

    rx8900_write_reg(RTC_RX8900_REG_SEC, bcd_time.sec);
    rx8900_write_reg(RTC_RX8900_REG_MIN, bcd_time.min);
    rx8900_write_reg(RTC_RX8900_REG_HOUR, bcd_time.hour);
    rx8900_write_reg(RTC_RX8900_REG_DAY, bcd_time.day);
    rx8900_write_reg(RTC_RX8900_REG_MONTH, bcd_time.month);
    rx8900_write_reg(RTC_RX8900_REG_YEAR, bcd_time.year);
}

void drv_rtc_rx8900_get_time(rtc_rx8900_time_t *p_time)
{
    rtc_rx8900_time_t bcd_time;

    bcd_time.sec = rx8900_read_reg(RTC_RX8900_REG_SEC);
    bcd_time.min = rx8900_read_reg(RTC_RX8900_REG_MIN);
    bcd_time.hour = rx8900_read_reg(RTC_RX8900_REG_HOUR);
    bcd_time.day = rx8900_read_reg(RTC_RX8900_REG_DAY);
    bcd_time.month = rx8900_read_reg(RTC_RX8900_REG_MONTH);
    bcd_time.year = rx8900_read_reg(RTC_RX8900_REG_YEAR);

    time_bcd_to_dec(&bcd_time);
    *p_time = bcd_time;
}

void drv_rtc_rx8900_set_alarm(rtc_rx8900_time_t *p_time)
{
    // TODO:
}