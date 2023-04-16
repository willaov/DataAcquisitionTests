add_executable(ethernet_test
        http_server.c
        w5x00_gpio_irq.c
        w5x00_spi.c
        )

target_link_libraries(ethernet_test
        pico_stdlib
        pico_cyw43_arch_none
        hardware_spi
        hardware_dma
        ETHERNET_FILES
        HTTPSERVER_FILES
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(ethernet_test)

# enable usb output, disable uart output
pico_enable_stdio_usb(ethernet_test 1)
pico_enable_stdio_uart(ethernet_test 0)
