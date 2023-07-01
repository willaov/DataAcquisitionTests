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
#include <stdlib.h>

#include "port_common.h"

#include "wizchip_conf.h"
#include "w5x00_spi.h"

#include "httpServer.h"
#include "dhcp.h"
#include "web_page.h"

#include "adc_test.h"
#include "one_wire_mux_test.h"
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
#define SOCKET_DHCP 0

/* Retry count */
#define DHCP_RETRY_COUNT 5

// #define DO_DHCP

/**
 * ----------------------------------------------------------------------------------------------------
 * Variables
 * ----------------------------------------------------------------------------------------------------
 */
/* Network */
static wiz_NetInfo g_net_info =
    {
        .mac = {0x00, 0x08, 0xDC, 0x12, 0x34, 0x56}, // MAC address
        .ip = {192, 168, 2, 2},                     // IP address
        .sn = {255, 255, 255, 0},                    // Subnet Mask
        .gw = {192, 168, 2, 1},                     // Gateway
        .dns = {192, 168, 2, 1},                         // DNS server
        #ifdef DO_DHCP
        .dhcp = NETINFO_DHCP                       // DHCP enable/disable
        #else
        .dhcp = NETINFO_STATIC
        #endif
};
static uint8_t g_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
}; // common buffer

/* DHCP */
static uint8_t g_dhcp_get_ip_flag = 0;

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

/* DHCP */
static void wizchip_dhcp_init(void);
static void wizchip_dhcp_assign(void);
static void wizchip_dhcp_conflict(void);

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

    // ADC
    setup_adc();
    reset_adc();

    // Onewire
    setup_onewire();

    // Wiznet
    wizchip_spi_initialize();
    wizchip_cris_initialize();

    wizchip_reset();
    wizchip_initialize();
    wizchip_check();

    #ifndef DO_DHCP
    network_initialize(g_net_info);
    #else
    wizchip_dhcp_init();

    uint8_t retval = 0;
    uint8_t dhcp_retry = 0;
    while (1)
    {
        /* Assigned IP through DHCP */
        retval = DHCP_run();

        if (retval == DHCP_IP_LEASED)
        {
            if (g_dhcp_get_ip_flag == 0)
            {
                printf(" DHCP success\n");

                g_dhcp_get_ip_flag = 1;
                break;
            }
        }
        else if (retval == DHCP_FAILED)
        {
            g_dhcp_get_ip_flag = 0;
            dhcp_retry++;

            if (dhcp_retry <= DHCP_RETRY_COUNT)
            {
                printf(" DHCP timeout occurred and retry %d\n", dhcp_retry);
            }
        }

        if (dhcp_retry > DHCP_RETRY_COUNT)
        {
            printf(" DHCP failed\n");

            DHCP_stop();

            while (1)
            {
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
                sleep_ms(100);
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
                sleep_ms(100);
            }
        }
    }
    #endif

    httpServer_init(g_http_send_buf, g_http_recv_buf, HTTP_SOCKET_MAX_NUM, g_http_socket_num_list);

    /* Get network information */
    print_network_information(g_net_info);

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);

    /* Pick Web Pages */
    // #define DATA_PAGE
    // #define ONEWIRE_PAGE
    #define DEMO_PAGE

    /* Web page buffers */
    #define INDEX_PAGE_SIZE (sizeof(index_page) + 100)
    char index_page_filled[INDEX_PAGE_SIZE];

    #ifdef DATA_PAGE
    #define DATA_PAGE_SIZE (sizeof(data_page) + 100)
    char data_page_filled[DATA_PAGE_SIZE];
    #endif

    #ifdef ONEWIRE_PAGE
    #define ONEWIRE_PAGE_SIZE (sizeof(onewire_page) + 150)
    char onewire_page_filled[ONEWIRE_PAGE_SIZE];
    #endif

    #ifdef DEMO_PAGE
    #define DEMO_PAGE_SIZE (sizeof(demo_page) + 200)
    char demo_page_filled[DEMO_PAGE_SIZE];
    #endif

    /* Register web page */
    snprintf(index_page_filled, INDEX_PAGE_SIZE, index_page, "Unfilled");
    reg_httpServer_webContent((uint8_t*)"index.html", (uint8_t*)index_page_filled);

    /* Variables for loop */
    double voltages[8] = {0.0};
    char* str_addresses[8];
    for (int i=0; i < 8; i++) {
        str_addresses[i] = (char*)malloc(sizeof("0000000000000000"));
    }
    const int x_to_w[8] = {4, 3, 2, 5, 6, 0, 7, 1};
    /* Infinite loop */
    while (1)
    {
        /* Run HTTP server */
        for (i = 0; i < HTTP_SOCKET_MAX_NUM; i++)
        {
            httpServer_run(i);
        }

        /* ADC Data Page */
        int16_t data[8] = {0};
        read_adc((uint16_t*)data);
        for (uint8_t j = 0; j < 8; j++) {
            voltages[j] = data[7-j] * 305e-6;
        }
        #ifdef DATA_PAGE
        snprintf(data_page_filled, DATA_PAGE_SIZE, data_page, voltages[0], voltages[1], voltages[2], voltages[3], voltages[4], voltages[5], voltages[6], voltages[7]);
        reg_httpServer_webContent((uint8_t*)"data.html", (uint8_t*)data_page_filled);
        #endif

        /* Onewire Page */
        rom_address_t addresses[8] = {};
        uint devices = check_for_devices(addresses);

        for (int i=0; i < 8; i++) {
            strfmtrom(str_addresses[x_to_w[i]], addresses[i]);
        }
        #ifdef ONEWIRE_PAGE
        snprintf(onewire_page_filled, ONEWIRE_PAGE_SIZE, onewire_page, devices, str_addresses[0], str_addresses[1], str_addresses[2], str_addresses[3], str_addresses[4], str_addresses[5], str_addresses[6], str_addresses[7]);
        reg_httpServer_webContent((uint8_t*)"onewire.html", (uint8_t*)onewire_page_filled);
        #endif

        #ifdef DEMO_PAGE
        snprintf(demo_page_filled, DEMO_PAGE_SIZE, demo_page, devices, voltages[0], str_addresses[0], voltages[1], str_addresses[1], voltages[2], str_addresses[2], voltages[3], str_addresses[3], str_addresses[4], str_addresses[5], str_addresses[6], str_addresses[7]);
        reg_httpServer_webContent((uint8_t*)"demo.html", (uint8_t*)demo_page_filled);
        #endif
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

/* DHCP */
static void wizchip_dhcp_init(void)
{
    printf(" DHCP client running\n");

    DHCP_init(SOCKET_DHCP, g_ethernet_buf);

    reg_dhcp_cbfunc(wizchip_dhcp_assign, wizchip_dhcp_assign, wizchip_dhcp_conflict);
}

static void wizchip_dhcp_assign(void)
{
    getIPfromDHCP(g_net_info.ip);
    getGWfromDHCP(g_net_info.gw);
    getSNfromDHCP(g_net_info.sn);
    getDNSfromDHCP(g_net_info.dns);

    g_net_info.dhcp = NETINFO_DHCP;

    /* Network initialize */
    network_initialize(g_net_info); // apply from DHCP

    print_network_information(g_net_info);
    printf(" DHCP leased time : %ld seconds\n", getDHCPLeasetime());
}

static void wizchip_dhcp_conflict(void)
{
    printf(" Conflict IP from DHCP\n");

    // halt or reset or any...
    while (1)
        ; // this example is halt.
}
