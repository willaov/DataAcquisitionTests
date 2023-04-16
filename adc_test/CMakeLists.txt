add_executable(adc_test
        adc_test.cpp
        )

target_link_libraries(adc_test
        pico_stdlib
        pico_cyw43_arch_none
        hardware_spi
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(adc_test)

# enable usb output, disable uart output
pico_enable_stdio_usb(adc_test 1)
pico_enable_stdio_uart(adc_test 0)
