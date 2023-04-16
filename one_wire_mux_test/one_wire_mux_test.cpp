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

    rom_address_t address{};
    while (true) {
        one_wire.single_device_read_rom(address);
        printf("Device Address: %02x%02x%02x%02x%02x%02x%02x%02x\n", address.rom[0], address.rom[1], address.rom[2], address.rom[3], address.rom[4], address.rom[5], address.rom[6], address.rom[7]);

        for (int i=0; i < 8; i++)
            one_wire.ram[i] = i;
        one_wire.write_scratch_pad(address, 8);

        sleep_ms(10);

        one_wire.read_scratch_pad(address);
        printf("TA1: %02x TA2: %02x E/S: %02x\n", one_wire.ta1, one_wire.ta2, one_wire.e_s);
        for (int i=0; i < 10; i++)
            printf("%02x", one_wire.ram[i]);
        printf("\n");

        int ret = one_wire.copy_scratch_pad(address);
        printf("Ret: %d\n", ret);

        uint8_t data[16] = {};
        one_wire.read_memory(address, 0, data, 16);
        for (int i=0; i < 16; i++)
            printf("%02x", data[i]);
        printf("\n");

        sleep_ms(1000);
    }

    return 0;
}
