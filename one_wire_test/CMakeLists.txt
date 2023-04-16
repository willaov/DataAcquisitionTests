add_executable(one_wire_test
        one_wire_test.cpp
        )

target_link_libraries(one_wire_test
        pico_stdlib
        hardware_spi
        pico_one_wire
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(one_wire_test)

# enable usb output, disable uart output
pico_enable_stdio_usb(one_wire_test 1)
pico_enable_stdio_uart(one_wire_test 0)
