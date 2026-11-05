#ifndef __BSP_EEPROM_H__
#define __BSP_EEPROM_H__

#include <stdint.h>
#include <stddef.h>
#include "hardware/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define I2C_PORT i2c0
#define I2C_SDA 4
#define I2C_SCL 5

#define EEPROM_I2C_ADDRESS 0x50
#define EEPROM_PAGE_SIZE   16
#define EEPROM_SIZE_BYTES  1024
#define EEPROM_SLOTS       15

void eeprom_init();
int eeprom_test();
int eeprom_readbytes(uint16_t address, uint8_t *buffer, size_t length);
int eeprom_writebytes(uint16_t address, const uint8_t *buffer, size_t length);

#ifdef __cplusplus
}
#endif

#endif // __BSP_EEPROM_H__
