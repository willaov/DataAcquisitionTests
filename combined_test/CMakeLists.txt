add_executable(combined_test
        combined_test.c
        w5x00_gpio_irq.c
        w5x00_spi.c
        adc_test.c
        mux_test.c
        one_wire_mux_test.cpp
        )

target_link_libraries(combined_test
        pico_stdlib
        pico_cyw43_arch_none
        pico_one_wire
        hardware_spi
        hardware_dma
        ETHERNET_FILES
        HTTPSERVER_FILES
        DHCP_FILES
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(combined_test)

# enable usb output, disable uart output
pico_enable_stdio_usb(combined_test 1)
pico_enable_stdio_uart(combined_test 0)
