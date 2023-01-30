#ifndef _BM1383_AGLV_H
#define _BM1383_AGLV_H

#include "hx_drv_iic_m.h"

#define BM1383AGLV_ADDRESS                  (0x5D)    // 7bit Addrss
#define BM1383AGLV_ID_VAL                   (0x32)
#define BM1383AGLV_ID                       (0x10)
#define BM1383AGLV_POWER_DOWN               (0x12)
#define BM1383AGLV_RESET                    (0x13)
#define BM1383AGLV_MODE_CONTROL             (0x14)
#define BM1383AGLV_PRESSURE_MSB             (0x1A)

#define BM1383AGLV_POWER_DOWN_PWR_DOWN          (1 << 0)
#define BM1383AGLV_RESET_RSTB                   (1 << 0)
#define BM1383AGLV_MODE_CONTROL_AVE_NUM16       (4 << 5)
#define BM1383AGLV_MODE_CONTROL_AVE_NUM64       (6 << 5)
#define BM1383AGLV_MODE_CONTROL_RESERVED_3BIT   (1 << 3)
#define BM1383AGLV_MODE_CONTROL_MODE_CONTINUOUS (4 << 0)

#define BM1383AGLV_POWER_DOWN_VAL      (BM1383AGLV_POWER_DOWN_PWR_DOWN)
#define BM1383AGLV_RESET_VAL           (BM1383AGLV_RESET_RSTB)
#define BM1383AGLV_MODE_CONTROL_VAL    (BM1383AGLV_MODE_CONTROL_AVE_NUM16 | BM1383AGLV_MODE_CONTROL_RESERVED_3BIT | BM1383AGLV_MODE_CONTROL_MODE_CONTINUOUS)

#define HPA_PER_COUNT                           (2048)
#define DEGREES_CELSIUS_PER_COUNT               (32)
#define GET_BYTE_PRESS_TEMP                     (5)
#define GET_BYTE_PRESS                          (3)
#define WAIT_TMT_MAX                            (240)
#define WAIT_BETWEEN_POWER_DOWN_AND_RESET       (2)



uint8_t bm1383aglv_init(void);
void bm1383_get_val(uint32_t *press , uint16_t *temp);
void I2C_bm1383_WRITE_API(uint8_t reg_addr, uint8_t reg_data); 
void I2C_bm1383_READ_API(uint8_t reg_addr, uint8_t * read_buf, uint8_t read_len); 
void bm1383_get_raw_val(uint8_t *data);


#endif