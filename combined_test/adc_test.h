#ifndef ADC_TEST_H
#define ADC_TEST_H

#include "pico/stdlib.h"

void setup_adc();
void reset_adc();
void read_adc(uint16_t values[8]);

#endif