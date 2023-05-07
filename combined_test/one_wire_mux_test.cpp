#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#include "one_wire.h"
extern "C" {
    #include "mux_test.h"
}
#ifdef __cplusplus
extern "C"
{
#endif

void setup_onewire();
uint check_for_devices(rom_address_t addresses[8]);
void strfmtrom(char* buffer, rom_address_t address);

#ifdef __cplusplus
}
#endif

#define ONE_WIRE_DAT 9

One_wire one_wire(ONE_WIRE_DAT);

void setup_onewire() {
    setup_mux();
    one_wire.init();
}

void strfmtrom(char* buffer, rom_address_t address) {
    sprintf(buffer, "%02x%02x%02x%02x%02x%02x%02x%02x", address.rom[0], address.rom[1], address.rom[2], address.rom[3], address.rom[4], address.rom[5], address.rom[6], address.rom[7]);
}

uint check_for_devices(rom_address_t addresses[8]) {
    uint devices = 0;
    for (int i=0; i < 8; i++) {
        set_mux(i);
        sleep_ms(1);
        int found = one_wire.find_and_count_devices_on_bus();
        if (found) {
            addresses[i] = one_wire.get_address(0);
            devices += 1;
        }
    }
    return devices;
}
