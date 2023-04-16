add_executable(sd_test
        sd_test.cpp
        hw_config.c
        )

target_link_libraries(sd_test
        pico_stdlib
        FatFs_SPI
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(sd_test)

# enable usb output, disable uart output
pico_enable_stdio_usb(sd_test 1)
pico_enable_stdio_uart(sd_test 0)
