add_executable(control_test
        control_test.cpp
        )

target_link_libraries(control_test
        pico_stdlib
        hardware_pwm
        hardware_i2c
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(control_test)

# disable usb output, disable uart output
pico_enable_stdio_usb(control_test 0)
pico_enable_stdio_uart(control_test 0)
