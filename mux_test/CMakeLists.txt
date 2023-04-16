add_executable(mux_test
        mux_test.c
        )

target_link_libraries(mux_test
        pico_stdlib
        pico_cyw43_arch_none
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(mux_test)

# enable usb output, disable uart output
pico_enable_stdio_usb(mux_test 1)
pico_enable_stdio_uart(mux_test 0)
