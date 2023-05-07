#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "one_wire.h"
extern "C" {
    #include "mux_test.h"
}

#define ONE_WIRE_DAT 9


int main() {
    stdio_init_all();

    setup_mux();

    One_wire one_wire(ONE_WIRE_DAT);
    one_wire.init();

    while (true) {
        for (int i=0; i < 8; i++) {
            set_mux(i);
            sleep_ms(100);
            int found = one_wire.find_and_count_devices_on_bus();
            if (found) {
                printf("Found %d on %d\n", found, i);
                rom_address_t address = one_wire.get_address(0);
                printf("Device Address: %02x%02x%02x%02x%02x%02x%02x%02x\n", address.rom[0], address.rom[1], address.rom[2], address.rom[3], address.rom[4], address.rom[5], address.rom[6], address.rom[7]);
            } else {
                printf("Not Found on %d\n", i);
            }
            sleep_ms(100);
        }
    }

    return 0;
}
