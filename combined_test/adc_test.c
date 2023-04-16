#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/cyw43_arch.h"

// ADC Defines
#define SPI1_RX 12
#define SPI1_CS   13
#define SPI1_SCK  14
#define SPI_ADC spi1
#define ADC_BUSY 10
#define ADC_FRST 11
#define CONVST 22
#define ADC_RST 31


void setup_adc() {
    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_ADC, 16*1000*1000);
    gpio_set_function(SPI1_RX, GPIO_FUNC_SPI);
    gpio_set_function(SPI1_CS,   GPIO_FUNC_SPI);
    gpio_set_function(SPI1_SCK,  GPIO_FUNC_SPI);
    spi_set_format(SPI_ADC, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(SPI1_CS, GPIO_OUT);
    gpio_put(SPI1_CS, 1);
    // Other ADC pins
    gpio_init(ADC_BUSY);
    gpio_set_dir(ADC_BUSY, GPIO_IN);
    gpio_init(ADC_FRST);
    gpio_set_dir(ADC_FRST, GPIO_IN);
    gpio_init(CONVST);
    gpio_set_dir(CONVST, GPIO_OUT);
    gpio_put(CONVST, 0);
    gpio_init(ADC_RST);
    gpio_set_dir(ADC_RST, GPIO_OUT);
    gpio_put(ADC_RST, 0);
}

void reset_adc() {
    gpio_put(ADC_RST, 1);
    sleep_us(1);
    gpio_put(ADC_RST, 0);
    sleep_us(0);
}

void conv_adc() {
    gpio_put(CONVST, 1);
    // Sleep for 8*4 = 32ns
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
    gpio_put(CONVST, 0);
    // Sleep for 8*6 - 48ns
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
    __asm volatile ("nop\n");
}

void read_adc(uint16_t values[8]) {
    conv_adc();
    while (gpio_get(ADC_BUSY));
    spi_read16_blocking(SPI_ADC, 0, values, 8);
}
