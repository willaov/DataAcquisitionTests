#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/clocks.h"
#include "hardware/pll.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"


#define rosc_div_addr ((io_rw_32 *const) 0x40060010u)
#define rosc_ctl_addr ((io_rw_32 *const) 0x40060000u)
#define rosc_freqa_addr ((io_rw_32 *const) 0x40060004u)
#define rosc_freqb_addr ((io_rw_32 *const) 0x40060008u)

// I2C Control Defines
#define I2CB_SDA 12
#define I2CB_SCL 13
#define I2CB i2c0

// Control Defines
#define GPIO_0_5 21
#define GPIO_1_5 20
#define GPIO_2_5 17
#define GPIO_3_5 16
#define PWM_0_5 23
#define PWM_0_5_CH PWM_CHAN_B
#define PWM_1_5 22
#define PWM_1_5_CH PWM_CHAN_A

#define GPIO_0_6 7
#define GPIO_1_6 6
#define GPIO_2_6 10
#define GPIO_3_6 11
#define PWM_0_6 9
#define PWM_0_6_CH PWM_CHAN_B
#define PWM_1_6 8
#define PWM_1_6_CH PWM_CHAN_A


void clk_init() {
    // Disable resus that may be enabled from previous software
    clocks_hw->resus.ctrl = 0;

    // Configure clocks
    // CLK_REF = XOSC (12MHz) / 1 = 12MHz
    // ROSC is 6MHz
    // clock_configure(clk_ref,
    //                 CLOCKS_CLK_REF_CTRL_SRC_VALUE_ROSC_CLKSRC_PH,
    //                 0, // No aux mux
    //                 6 * MHZ,
    //                 6 * MHZ);

    /// \tag::configure_clk_sys[]
    // CLK SYS = PLL SYS (125MHz) / 1 = 125MHz
    clock_configure(clk_sys,
                    CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLK_REF,
                    CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_ROSC_CLKSRC,
                    6 * MHZ,
                    6 * MHZ);
    /// \end::configure_clk_sys[]

    // CLK RTC = PLL USB (48MHz) / 1024 = 46875Hz
    clock_configure(clk_rtc,
                    0, // No GLMUX
                    CLOCKS_CLK_RTC_CTRL_AUXSRC_VALUE_ROSC_CLKSRC_PH,
                    6 * MHZ,
                    46875);

    // CLK PERI = clk_sys. Used as reference clock for Peripherals. No dividers so just select and enable
    // Normally choose clk_sys or clk_usb
    clock_configure(clk_peri,
                    0,
                    CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                    6 * MHZ,
                    6 * MHZ);
}


void I2CIRQHandler() {
    I2CB->hw->data_cmd = 0x55;
    I2CB->hw->clr_tx_abrt;
    I2CB->hw->clr_rd_req;
}


int main() {
    clk_init();
    uint32_t volatile sys = clock_get_hz(clk_sys);
    uint32_t volatile ref = clock_get_hz(clk_ref);
    uint32_t volatile rtc = clock_get_hz(clk_rtc);
    uint32_t volatile peri = clock_get_hz(clk_peri);


    gpio_init(GPIO_0_5);
    gpio_set_dir(GPIO_0_5, GPIO_OUT);
    gpio_put(GPIO_0_5, true);
    gpio_init(GPIO_1_5);
    gpio_set_dir(GPIO_1_5, GPIO_IN);
    gpio_set_pulls(GPIO_1_5, true, false);

    gpio_init(GPIO_2_5);
    gpio_set_dir(GPIO_2_5, GPIO_OUT);
    gpio_put(GPIO_2_5, true);
    gpio_init(GPIO_3_5);
    gpio_set_dir(GPIO_3_5, GPIO_OUT);
    gpio_put(GPIO_3_5, true);

    gpio_set_function(PWM_0_5, GPIO_FUNC_PWM);
    uint slice_num_5 = pwm_gpio_to_slice_num(PWM_0_5);
    pwm_set_clkdiv(slice_num_5, 10.0);
    pwm_set_wrap(slice_num_5, 16);
    pwm_set_chan_level(slice_num_5, PWM_0_5_CH, 8);      // 50% duty cycle
    pwm_set_enabled(slice_num_5, true);

    gpio_set_function(PWM_1_5, GPIO_FUNC_PWM);
    uint slice_num_6 = pwm_gpio_to_slice_num(PWM_1_5);
    pwm_set_clkdiv(slice_num_6, 10.0);
    pwm_set_wrap(slice_num_6, 16);
    pwm_set_chan_level(slice_num_6, PWM_1_5_CH, 8);      // 50%?
    pwm_set_enabled(slice_num_6, true);

    uint8_t pwm_switch = 8;

    // I2C
    i2c_init(I2CB, 100 * 1000);
    gpio_set_function(I2CB_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2CB_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2CB_SDA);
    gpio_pull_up(I2CB_SCL);

    i2c_set_slave_mode(I2CB, true, 0x54);

    // Enable the I2C interrupts we want to process
    I2CB->hw->intr_mask = (I2C_IC_INTR_MASK_M_RD_REQ_BITS | I2C_IC_INTR_MASK_M_RX_FULL_BITS);

    // Set up the interrupt handler to service I2C interrupts - needs to be done on this core
    irq_set_exclusive_handler(I2C0_IRQ, &I2CIRQHandler);

    // Enable I2C interrupt
    irq_set_enabled(I2C0_IRQ, true);

    while (true) {
        gpio_put(GPIO_3_5, gpio_get(GPIO_1_5));

        gpio_put(GPIO_2_5, true);
        gpio_put(GPIO_0_5, true);
        sleep_ms(1);
        gpio_put(GPIO_2_5, false);
        gpio_put(GPIO_0_5, false);
        sleep_ms(1);
        pwm_switch += 1;
        if (pwm_switch > 16) pwm_switch = 0;
        pwm_set_chan_level(slice_num_5, PWM_0_5_CH, pwm_switch);
    }

    return 0;
}
