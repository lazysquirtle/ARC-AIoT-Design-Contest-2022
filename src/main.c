#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include "embARC.h"
#include "embARC_debug.h"
#include "board_config.h"
#include "arc_timer.h"
#include "hx_drv_spi_s.h"
#include "spi_slave_protocol.h"
#include "hardware_config.h"

#include "hx_drv_iic_m.h"
#include "hx_drv_uart.h"
#include "hx_drv_iomux.h"

#include "SC16IS750_Bluepacket.h"
#include "synopsys_i2c_oled1306.h"

#include "sdk_sht31.h"
#include "sdk_mpu9250.h"
#include "sdk_bm1383aglv.h"

#include "tflitemicro_algo.h"

#define FLOAT_MARKER "%c%d.%02d"
#define FLOAT(val)  (uint8_t)(((val) < 0 && (val) > -1.0) ? '-' : ' '),                     \
                    (int32_t)(val),                                                         \
                    (int32_t)((((val) > 0) ? (val) - (int32_t)(val) : (int32_t)(val) - (val))*100)

#define uart_buf_size 100
#define char_buf_size 100

DEV_IIC  *iic1_ptr;
DEV_UART *uart0_ptr;
char uart_buf[uart_buf_size] = {0};
char char_buf[char_buf_size] = {0};

uint8_t     gpio_value_0;
uint8_t     gpio_value_1;
uint16_t    temperature;
uint32_t    pressure;
uint16_t    humidity;

int16_t x_val;
int16_t y_val;
int16_t z_val;

float first_pressure_float = 0.0;
float first_temperature_float = 0.0;
float pressure_float = 0.0;
float temperature_float = 0.0;
float x_value_float = 0.0;
float y_value_float = 0.0;
float z_value_float = 0.0;

uint16_t    loop_count = 0;
float       input_data[5] = {0.0};

void standarization(float *pres,float *temp);

void standarization(float *pres,float *temp){
    if (*pres > (first_pressure_float+2.0))
        *pres = 1.0;
    else if (*pres < first_pressure_float) 
        *pres = 0.0;
    else
        *pres = (*pres - first_pressure_float)/2.0;

    if (*temp > (first_temperature_float+3.0))
        *temp = 1.0;
    else if (*temp < first_temperature_float)
        *temp = 0.0;
    else 
        *temp = (*temp - first_temperature_float)/3.0;

}

int main(void)
{    

    //UART 0 is already initialized with 115200bps
    uart0_ptr = hx_drv_uart_get_dev(USE_SS_UART_0);
    iic1_ptr = hx_drv_i2cm_get_dev(USE_SS_IIC_1);
    iic1_ptr->iic_open(DEV_MASTER_MODE, IIC_SPEED_STANDARD); 

    sprintf(uart_buf, "Mask Project Start\r\n");    
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
    board_delay_ms(100);

    /*
    HX_GPIOSetup();
	IRQSetup();
    UartInit(SC16IS750_PROTOCOL_SPI);
    GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1, INPUT);
    GPIOSetPinMode(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7, INPUT);
    */

    tflitemicro_algo_init();
    
    
    sprintf(uart_buf, "OLED Init\r\n");    
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
    board_delay_ms(100);
    OLED_Init();
	OLED_Clear(); 
    board_delay_ms(100);
    

    sprintf(uart_buf, "bm1383aglv Init\r\n");    
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
    board_delay_ms(100);

    uint8_t chip_id1 = bm1383aglv_init();
    if(chip_id1 == BM1383AGLV_ID_VAL)
        sprintf(uart_buf, "Chip ID: 0x%2X | OK\r\n", chip_id1);    
    else 
        sprintf(uart_buf, "Chip ID: 0x%2X | Error\r\n", chip_id1);    
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
    board_delay_ms(100);
    
    
    bm1383_get_val(&pressure, &temperature);
    
    sprintf(char_buf, "%3d.%2d\t",(pressure >> 11), abs(pressure & 0x3FF));
    //uart0_ptr->uart_write(char_buf, strlen(char_buf));
    first_pressure_float = (float)atof(char_buf);
    memset(char_buf ,0 ,strlen(char_buf));

    sprintf(char_buf, "%2d.%2d\t",(temperature >> 5), abs(temperature & 0xF));
    //uart0_ptr->uart_write(char_buf, strlen(uart_buf));
    first_temperature_float = (float)atof(char_buf);
    memset(char_buf ,0 ,strlen(char_buf));
    
    board_delay_ms(100);

    uint8_t chip_id2 = mpu9250_init();
    if(chip_id2 == 0x71 || chip_id2 == 0x73)
        sprintf(uart_buf, "Chip ID: 0x%2X | OK\r\n", chip_id2);    
    else 
        sprintf(uart_buf, "Chip ID: 0x%2X | Error\r\n", chip_id2);    
    uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
    board_delay_ms(100);
        

    sht31_init();
    sprintf(uart_buf, "Chip ID 0x44 OK\r\n");
    board_delay_ms(100);

    
    while(1)
    {
        
        bm1383_get_val(&pressure, &temperature);
        /*
        sprintf(uart_buf, "%3d.%-2d\t%2d.%-2d\t",              
        (pressure >> 11), abs(pressure & 0x3FF),                //加快 pressure/2048  pressure%2048
        (temperature >> 5), abs(temperature &0xF));             //加快 temperature/32 temperature%32   
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(20);
        */
        
        sprintf(char_buf, "%3d.%3d\t",(pressure >> 11), abs(pressure & 0x3FF));
        pressure_float = (float)atof(char_buf);
        memset(char_buf ,0 ,strlen(char_buf));
        sprintf(char_buf, "%2d.%2d\t",(temperature >> 5), abs(temperature & 0xF));
        temperature_float = (float)atof(char_buf);
        memset(char_buf ,0 ,strlen(char_buf));
        
        standarization(&pressure_float,&temperature_float);
        board_delay_ms(100);
        /*
        sprintf(uart_buf, FLOAT_MARKER"\t"FLOAT_MARKER"\t\r\n",
        FLOAT(pressure_float),FLOAT(temperature_float));    
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(100);
        */
        
        mpu9250_Get_Data(&x_val, &y_val, &z_val);

        x_value_float = ((float)x_val/16384);
        y_value_float = ((float)y_val/16384);
        z_value_float = ((float)z_val/16384);

        /*
        sprintf(uart_buf, FLOAT_MARKER"\t"FLOAT_MARKER"\t"FLOAT_MARKER"\t",
        FLOAT(x_value_float),FLOAT(y_value_float),FLOAT(z_value_float));    
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        board_delay_ms(100);
        */
        
        input_data[0] = pressure_float;
        input_data[1] = temperature_float;
        input_data[2] = x_value_float;
        input_data[3] = y_value_float;
        input_data[4] = z_value_float;
        

        int index = tflitemicro_algo_run(input_data);
        
        if(index == 0){
            sprintf(uart_buf, "Nothing\r\n");    
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            //printf("Nothing\r\n");
            OLED_Clear(); 
            OLED_SetCursor(4, 5);
            OLED_DisplayString("NOTHING.");           
        }
        else{
            sprintf(uart_buf, "Cough\r\n");    
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            //printf("Cougn\r\n");
            OLED_Clear(); 
            OLED_SetCursor(4, 5);
            OLED_DisplayString("COUGH.");  
        }
        

        /*
        gpio_value_0 = GPIOGetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO7);
		gpio_value_1 = GPIOGetPinState(SC16IS750_PROTOCOL_SPI, CH_A, GPIO1);
        
        if (gpio_value_0 == 1 && gpio_value_1 == 1){
            sprintf(uart_buf, "0\n", 2); 
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        }     
        if (gpio_value_0 == 0){
            sprintf(uart_buf, "1\n", 2);   
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        }     
        if (gpio_value_1 == 0){
            sprintf(uart_buf, "2\n", 2); 
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
        }  
        printf("\r");
        */

        
        if (loop_count > 10000)
        {
            loop_count = 0;
            sht31_read_data(&humidity);
            sprintf(uart_buf, "H: %4d.%04d \r\n",(humidity*100/65536), abs((humidity*100)%(65536)));    
            uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(2000);
        }
        loop_count++;
        

        board_delay_ms(100);
    }

   /*
   while (1)
   {
        sht31_read_data(&temperature,&humidity);
        sprintf(uart_buf, "H: %4d.%04d \r\n",(humidity*100/65536), abs((humidity*100)%(65536)));    
        uart0_ptr->uart_write(uart_buf, strlen(uart_buf));
            board_delay_ms(2000);
   }
    */

    return 0;
}
