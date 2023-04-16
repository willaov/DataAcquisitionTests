#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/cyw43_arch.h"

// MUX Defines
#define ONEWIRE_SEL_A 6
#define ONEWIRE_SEL_B 7
#define ONEWIRE_SEL_C 8
#define ONEWIRE_DAT 9


void setup_mux() {
    // MUX Pins
    // B, A and X3 are shorted, so to be safe set to low drive strength
    gpio_init(ONEWIRE_SEL_A);
    gpio_set_dir(ONEWIRE_SEL_A, GPIO_OUT);
    gpio_set_drive_strength(ONEWIRE_SEL_A, GPIO_DRIVE_STRENGTH_2MA);
    gpio_put(ONEWIRE_SEL_A, true);

    gpio_init(ONEWIRE_SEL_B);
    gpio_set_dir(ONEWIRE_SEL_B, GPIO_OUT);
    gpio_set_drive_strength(ONEWIRE_SEL_B, GPIO_DRIVE_STRENGTH_2MA);
    gpio_put(ONEWIRE_SEL_B, true);

    gpio_init(ONEWIRE_SEL_C);
    gpio_set_dir(ONEWIRE_SEL_C, GPIO_OUT);
    gpio_set_drive_strength(ONEWIRE_SEL_C, GPIO_DRIVE_STRENGTH_2MA);
    gpio_put(ONEWIRE_SEL_C, true);
}

void set_mux(uint8_t value) {
    printf("Set to %d\n", value);
    printf("Set to %d %d %d\n", value & 0b100, value & 0b010, value & 0b001);
    gpio_put(ONEWIRE_SEL_C, (value & 0b100) > 0);
    gpio_put(ONEWIRE_SEL_B, (value & 0b010) > 0);
    gpio_put(ONEWIRE_SEL_A, (value & 0b001) > 0);
}

int main() {
    stdio_init_all();
    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        return -1;
    }

    setup_mux();

    gpio_init(ONEWIRE_DAT);
    gpio_set_dir(ONEWIRE_DAT, GPIO_OUT);
    gpio_set_drive_strength(ONEWIRE_DAT, GPIO_DRIVE_STRENGTH_2MA);
    gpio_put(ONEWIRE_DAT, false);

    bool flash = false;
    uint8_t settings[2] = {4, 7};
    int setting = 0;
    while (1) {
        // Set to the next setting
        set_mux(settings[setting]);
        printf("Set to %d\n", settings[setting]);
        setting++;
        if (setting >= sizeof(settings)) setting = 0;

        for (int i=0; i < 4; i++) {
            gpio_put(ONEWIRE_DAT, flash);
            flash = !flash;
            sleep_ms(250);
        }
    }

    return 0;
}
