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

// -----------------------------------------------------------
static rtc_rx8900_config_t s_config;

static void rx8900_start(void);
static void rx8900_stop(void);
static void rx8900_write_reg(uint8_t reg_addr, uint8_t data);
static uint8_t rx8900_read_reg(uint8_t reg_addr);
// -----------------------------------------------------------
// [Static関数]

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

    // TODO: 割り込み/時刻/アラームの設定

    // RTCスタート(カウント開始)
    rx8900_start();

    return ret;
}

void drv_rtc_rx8900_set_time(rtc_rx8900_time_t *p_time)
{
    // TODO:
}

void drv_rtc_rx8900_get_time(rtc_rx8900_time_t *p_time)
{
    // TODO:
}

void drv_rtc_rx8900_set_alarm(rtc_rx8900_time_t *p_time)
{
    // TODO:
}