#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "sdk_mpu9250.h"

#define BUFFER_SIZE                 14      //read acceleromter, gyroscope and temperature data in one transmission.
#define MPU_TWI_TIMEOUT 		    10000 
#define MPU9250_ADDRESS     		0x68 
#define MPU_AK89XX_MAGN_ADDRESS     0x0C

#define USE_SS_IIC_X USE_SS_IIC_1

static uint8_t twi_tx_buffer[BUFFER_SIZE];

uint8_t mpu9250_init(void)
{
    uint8_t read_data;
    I2C_mpu9250_READ_API(MPU_9250_REG_WHO_AM_I, &read_data, 1);
    return read_data;
}

void mpu9250_Get_Data(int16_t* x, int16_t* y, int16_t* z)
{
    uint8_t read_data[6];
    I2C_mpu9250_READ_API(MPU_9250_REG_ACCEL_XOUT_H, read_data, 6);   

    *x = ((uint16_t) read_data[0] << 8) | read_data[1];
    *y = ((uint16_t) read_data[2] << 8) | read_data[3];
    *z = ((uint16_t) read_data[4] << 8) | read_data[5];
}

void I2C_mpu9250_WRITE_API(uint8_t reg_addr, uint8_t reg_data)
{
    uint8_t write_data[2];
    write_data[0] = reg_addr;
    write_data[1] = reg_data;   
    hx_drv_i2cm_write_data(USE_SS_IIC_X, MPU9250_ADDRESS, write_data, 0, write_data, 2); 
}


void I2C_mpu9250_READ_API(uint8_t reg_addr, uint8_t* read_buf, uint8_t read_len)
{
    uint8_t write_data[1];
    write_data[0] = reg_addr;
    hx_drv_i2cm_write_data(USE_SS_IIC_X, MPU9250_ADDRESS, write_data, 0, write_data, 1); 
    hx_drv_i2cm_read_data(USE_SS_IIC_X, MPU9250_ADDRESS, read_buf, read_len);
}

