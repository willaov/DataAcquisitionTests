#ifndef ONEWIRE_MUX_TEST_H
#define ONEWIRE_MUX_TEST_H

#include "pico/stdlib.h"

typedef struct {
	uint8_t rom[8];
} rom_address_t;

void setup_onewire();
void strfmtrom(char* buffer, rom_address_t address);
uint check_for_devices(rom_address_t addresses[8]);

#endif