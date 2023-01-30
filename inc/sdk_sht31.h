#ifndef NRF_DRV_SHT__
#define NRF_DRV_SHT__

#include "hx_drv_iic_m.h"

#define SHT_TWI_BUFFER_SIZE           8       // 14 byte buffers 
#define SHT_TIMEOUT                   10000 
#define SHT_ADDRESS                   (0x44)

/**************************************************************/
void sht31_write_command(uint16_t cmd);
void sht31_read_data(uint16_t* humidity);
void sht31_init(void);
/**************************************************************/

#endif /* DRV_SHT31__ */
