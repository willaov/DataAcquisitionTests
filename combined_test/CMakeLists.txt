add_executable(combined_test
        combined_test.c
        w5x00_gpio_irq.c
        w5x00_spi.c
        adc_test.c
        )

target_link_libraries(combined_test
        pico_stdlib
        pico_cyw43_arch_none
        hardware_spi
        hardware_dma
        ETHERNET_FILES
        HTTPSERVER_FILES
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(combined_test)

# enable usb output, disable uart output
pico_enable_stdio_usb(combined_test 1)
pico_enable_stdio_uart(combined_test 0)
