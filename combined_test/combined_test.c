/**
 * Copyright (c) 2021 WIZnet Co.,Ltd
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * ----------------------------------------------------------------------------------------------------
 * Includes
 * ----------------------------------------------------------------------------------------------------
 */
#include <stdio.h>

#include "port_common.h"

#include "wizchip_conf.h"
#include "w5x00_spi.h"

#include "httpServer.h"
#include "web_page.h"

#include "adc_test.h"
#include "pico/cyw43_arch.h"

/**
 * ----------------------------------------------------------------------------------------------------
 * Macros
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
#define PLL_SYS_KHZ (133 * 1000)

/* Buffer */
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)

/* Socket */
#define HTTP_SOCKET_MAX_NUM 4

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
/* Network */
static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip = {192, 168, 1, 15},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 1, 1},                     // Gateway
        .dns = {192, 168, 1, 1},                         // DNS server
        .dhcp = NETINFO_STATIC                       // DHCP enable/disable
};

/* HTTP */
static uint8_t g_http_send_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};
static uint8_t g_http_recv_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};
static uint8_t g_http_socket_num_list[HTTP_SOCKET_MAX_NUM] = {0, 1, 2, 3};

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void);

/**
 * ----------------------------------------------------------------------------------------------------
 * Main
 * ----------------------------------------------------------------------------------------------------
 */
int main()
{
    /* Initialize */
    uint8_t i = 0;

    set_clock_khz();

    stdio_init_all();

    if (cyw43_arch_init()) {
        printf("WiFi init failed");
        return -1;
    }

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);

    // ADC
    setup_adc();
    reset_adc();

    // Wiznet
    wizchip_spi_initialize();
    wizchip_cris_initialize();

    wizchip_reset();
    wizchip_initialize();
    wizchip_check();

    network_initialize(g_net_info);

    httpServer_init(g_http_send_buf, g_http_recv_buf, HTTP_SOCKET_MAX_NUM, g_http_socket_num_list);

    /* Get network information */
    print_network_information(g_net_info);

    /* Web page buffers */
    #define INDEX_PAGE_SIZE (sizeof(index_page) + 100)
    char index_page_filled[INDEX_PAGE_SIZE];

    #define DATA_PAGE_SIZE (sizeof(data_page) + 100)
    char data_page_filled[DATA_PAGE_SIZE];

    /* Register web page */
    snprintf(index_page_filled, INDEX_PAGE_SIZE, index_page, "Unfilled");
    reg_httpServer_webContent((uint8_t*)"index.html", (uint8_t*)index_page_filled);

    /* Variables for loop */
    double voltages[8] = {0.0};
    /* Infinite loop */
    while (1)
    {
        /* Run HTTP server */
        for (i = 0; i < HTTP_SOCKET_MAX_NUM; i++)
        {
            httpServer_run(i);
        }
        
        int16_t data[8] = {0};

        read_adc((uint16_t*)data);

        for (uint8_t j = 0; j < 8; j++) {
            voltages[j] = data[j] * 305e-6;
        }

        snprintf(data_page_filled, DATA_PAGE_SIZE, data_page, voltages[0], voltages[1], voltages[2], voltages[3], voltages[4], voltages[5], voltages[6], voltages[7]);
        reg_httpServer_webContent((uint8_t*)"data.html", (uint8_t*)data_page_filled);
        printf("Running\n");
        for (uint8_t j = 0; j < 8; j++) {
            printf("%f, ", voltages[j]);
        }
        printf("\n");
    }
}

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void)
{
    // set a system clock frequency in khz
    set_sys_clock_khz(PLL_SYS_KHZ, true);

    // configure the specified clock
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
}
