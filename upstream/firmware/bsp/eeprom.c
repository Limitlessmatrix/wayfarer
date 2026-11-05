#include "eeprom.h"
#include "pico/stdlib.h"
#include <string.h>
#include <stdio.h>

static i2c_inst_t *eeprom_i2c = I2C_PORT;

void eeprom_init() {
    i2c_init(eeprom_i2c, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

int eeprom_test() {
    uint8_t dummy_byte = 0;
    int result = i2c_write_blocking(eeprom_i2c, EEPROM_I2C_ADDRESS, &dummy_byte, 1, false);
    return (result >= 0);
}

int eeprom_readbytes(uint16_t address, uint8_t *buffer, size_t length) {
    if (!eeprom_i2c || !buffer || length == 0) {
        return -1;
    }
    if (address >= EEPROM_SIZE_BYTES || (address + length) > EEPROM_SIZE_BYTES) {
        return -2;
    }
    uint8_t mem_top = address >> 8;
    uint8_t mem_addr = (uint8_t)address;
    int res = i2c_write_blocking(eeprom_i2c, EEPROM_I2C_ADDRESS + mem_top, &mem_addr, 1, true);
    if (res != 1) {
        return -3;
    }

    res = i2c_read_blocking(eeprom_i2c, EEPROM_I2C_ADDRESS, buffer, length, false);
    return (res == (int)length) ? 0 : -4;
}

int eeprom_writebytes(uint16_t address, const uint8_t *buffer, size_t length) {
    if (!eeprom_i2c || !buffer || length == 0) {
        return -1;
    }
    if (address >= EEPROM_SIZE_BYTES || (address + length) > EEPROM_SIZE_BYTES) {
        return -2;
    }

    size_t written = 0;
    while (written < length) {
        uint8_t page_offset = address % EEPROM_PAGE_SIZE;
        size_t chunk = EEPROM_PAGE_SIZE - page_offset;
        if (chunk > (length - written)) {
            chunk = length - written;
        }

        uint8_t tx_buffer[1 + EEPROM_PAGE_SIZE];
        uint8_t mem_top = address >> 8;
        tx_buffer[0] = (uint8_t)address;
        memcpy(&tx_buffer[1], buffer + written, chunk);

        int res = i2c_write_blocking(eeprom_i2c, EEPROM_I2C_ADDRESS + mem_top, tx_buffer, 1 + chunk, false);
        if (res != (int)(1 + chunk)) {
            return -3;
        }

        sleep_ms(5);

        written += chunk;
        address += chunk;
    }

    return 0;
}
