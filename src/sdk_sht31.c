#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sdk_sht31.h>
#include "hx_drv_timer.h"

#define USE_SS_IIC_X USE_SS_IIC_1 

void sht31_write_command(uint16_t cmd)
{
    static uint8_t tx_buf[2];
    tx_buf[0] = (uint8_t)(cmd>>8);  
    tx_buf[1] = (uint8_t)(cmd & 0xFF);  
    hx_drv_i2cm_write_data(USE_SS_IIC_1, SHT_ADDRESS, tx_buf, 0, tx_buf, 2); 
} 

void sht31_init(void)
{
    //set measure period 
    sht31_write_command(0x2130);
}

static float SHT3X_CalcTemperature(uint16_t rawValue)
{
    //T = -45 + 175 * rawValue / (2^16-1)
    float   temperature = 175.0f * (float)rawValue / 65535.0f - 45.0f;
    return  temperature;
}

static float SHT3X_CalcHumidity(uint16_t rawValue)
{
    //RH = rawValue / (2^16-1) * 100
    float huminity = 100.0f * (float)rawValue / 65535.0f;
    return huminity;
}


void sht31_read_data(uint16_t* humidity)
{
    static uint8_t  data[6];
    
    //0xE000,readout measurements for periodic mode
    sht31_write_command(0xE000); 
    printf("Read Succeed\n");
    board_delay_ms(200);
    hx_drv_i2cm_read_data(USE_SS_IIC_1, SHT_ADDRESS, data, 6);
    //*temperature = SHT3X_CalcTemperature((data[0] << 8) | data[1]);
    //*humidity = SHT3X_CalcHumidity((data[3] << 8) | data[4]);
    *humidity = ((data[3] << 8) | data[4]);

    //CRC check
    //error = SHT3X_CheckCrc(bytes, 2, bytes[2]);
    /*
    if(error == NO_ERROR){
        *temperature = SHT3X_CalcTemperature((bytes[0] << 8) | bytes[1]);
        *humidity = SHT3X_CalcHumidity((bytes[3] << 8) | bytes[4]);
    } 
    */
}

