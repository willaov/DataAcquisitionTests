/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Sweep through all 7-bit I2C addresses, to see if any slaves are present on
// the I2C bus. Print out a table that looks like this:
//
// I2C Bus Scan
//   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
// 0
// 1       @
// 2
// 3             @
// 4
// 5
// 6
// 7
//
// E.g. if slave addresses 0x12 and 0x34 were acknowledged.

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "pico/cyw43_arch.h"

#define I2CA_SDA 0
#define I2CA_SCL 1
#define I2CA_INT 5
#define I2CA i2c0

#define I2CB_SDA 2
#define I2CB_SCL 3
#define I2CB_INT 4
#define I2CB i2c1

#define I2C_MUX_ADDR 0x70

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

void bus_scan() {
    printf("\nI2CA Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(I2CA, addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");

    printf("\nI2CB Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(I2CB, addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
}

int main() {
    // Enable UART so we can print status output
    stdio_init_all();
    sleep_ms(5000);
    printf("Starting Now\n");
    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        return -1;
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);

    uint baudA = i2c_init(I2CA, 90 * 1000);
    gpio_set_function(I2CA_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2CA_SCL, GPIO_FUNC_I2C);
    printf("%x, %x, functions, %d, baud\n", gpio_get_function(I2CA_SDA), gpio_get_function(I2CA_SCL), baudA);
    gpio_pull_up(I2CA_SDA);
    gpio_pull_up(I2CA_SCL);

    uint baudB = i2c_init(I2CB, 90 * 1000);
    gpio_set_function(I2CB_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2CB_SCL, GPIO_FUNC_I2C);
    printf("%x, %x, functions, %d, baud\n", gpio_get_function(I2CB_SDA), gpio_get_function(I2CB_SCL), baudB);
    gpio_pull_up(I2CB_SDA);
    gpio_pull_up(I2CB_SCL);

    bus_scan();

    int ret;
    uint8_t rxdata = 0;
    uint8_t txdata = 0;

    ret = i2c_read_blocking(I2CB, 0x54, &rxdata, 1, false);
    printf("%d %x from control pico\n", ret, rxdata);
    rxdata = 0;

    ret = i2c_read_blocking(I2CA, I2C_MUX_ADDR, &rxdata, 1, false);
    printf("%d %x from multiplexer A\n", ret, rxdata);
    rxdata = 0;
    txdata = 0b101; // Select channel 1
    // sleep_ms(500);
    ret = i2c_write_blocking(I2CA, I2C_MUX_ADDR, &txdata, 1, false);
    printf("%d %x to multiplexer A\n", ret, txdata);
    // sleep_ms(500);
    ret = i2c_read_blocking(I2CA, I2C_MUX_ADDR, &rxdata, 1, false);
    printf("%d %x from multiplexer A\n", ret, rxdata);
    rxdata = 0;

    // sleep_ms(500);

    ret = i2c_read_blocking(I2CB, I2C_MUX_ADDR, &rxdata, 1, false);
    printf("%d %x from multiplexer B\n", ret, rxdata);
    rxdata = 0;
    txdata = 0b101; // Select channel 1
    // sleep_ms(500);
    ret = i2c_write_blocking(I2CB, I2C_MUX_ADDR, &txdata, 1, false);
    printf("%d %x to multiplexer B\n", ret, txdata);
    // sleep_ms(500);
    ret = i2c_read_blocking(I2CB, I2C_MUX_ADDR, &rxdata, 1, false);
    printf("%d %x from multiplexer B\n", ret, rxdata);
    rxdata = 0;

    bus_scan();

    printf("All done");

    return 0;
}
