#include "sdk_bm1383aglv.h"
#include "hx_drv_timer.h"

#define USE_SS_IIC_X USE_SS_IIC_1

uint8_t bm1383aglv_init(void)
{
    uint8_t read_data[1];
    I2C_bm1383_READ_API(BM1383AGLV_ID, &read_data[0], 1);

    I2C_bm1383_WRITE_API(BM1383AGLV_POWER_DOWN, BM1383AGLV_POWER_DOWN_VAL);
    board_delay_ms(WAIT_BETWEEN_POWER_DOWN_AND_RESET);
    I2C_bm1383_WRITE_API(BM1383AGLV_RESET, BM1383AGLV_RESET_VAL);

    I2C_bm1383_WRITE_API(BM1383AGLV_MODE_CONTROL, BM1383AGLV_MODE_CONTROL_VAL);
    board_delay_ms(WAIT_TMT_MAX);

    return read_data[0];
}

void I2C_bm1383_WRITE_API(uint8_t reg_addr, uint8_t reg_data)
{
    uint8_t write_data[2];

    write_data[0] = reg_addr;
    write_data[1] = reg_data;   
    hx_drv_i2cm_write_data(USE_SS_IIC_X, BM1383AGLV_ADDRESS, write_data, 0, write_data, 2); 
}


void I2C_bm1383_READ_API(uint8_t reg_addr, uint8_t * read_buf, uint8_t read_len)
{
    uint8_t write_data[1];

    write_data[0] = reg_addr;
    hx_drv_i2cm_write_data(USE_SS_IIC_X, BM1383AGLV_ADDRESS, write_data, 0, write_data, 1); 
    hx_drv_i2cm_read_data(USE_SS_IIC_X, BM1383AGLV_ADDRESS, read_buf, read_len);
}

void bm1383_get_raw_val(uint8_t *data)
{
    I2C_bm1383_READ_API(BM1383AGLV_PRESSURE_MSB, data, GET_BYTE_PRESS_TEMP);
}

void bm1383_get_val(uint32_t *press , uint16_t *temp)
{
    uint8_t     val[GET_BYTE_PRESS_TEMP];
    uint32_t    raw_press;
    uint16_t    raw_temp;

    bm1383_get_raw_val(val);

    raw_press = (((uint32_t)val[0] << 16) | ((uint32_t) val[1] << 8) | val[2] & 0xFC) >> 2;
    *press = raw_press;
    raw_temp  = ((uint16_t)val[3]<<8) | val[4];
    *temp  = raw_temp;
}