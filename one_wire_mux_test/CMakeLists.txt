add_executable(one_wire_mux_test
        one_wire_mux_test.cpp
        mux_test.c
        )

target_link_libraries(one_wire_mux_test
        pico_stdlib
        pico_cyw43_arch_none
        pico_one_wire
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(one_wire_mux_test)

# enable usb output, disable uart output
pico_enable_stdio_usb(one_wire_mux_test 1)
pico_enable_stdio_uart(one_wire_mux_test 0)
