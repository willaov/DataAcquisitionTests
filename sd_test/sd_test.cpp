#include <stdio.h>
#include "pico/stdlib.h"
#include "sd_card.h"
#include "ff.h"

#define SD_CD 15
#define SPI0_RX 16
#define SPI0_SCK 18
#define SPI0_TX 19

int main() {

    FRESULT fr;
    FATFS fs;
    FIL fil;
    int ret;
    char buf[100];
    char filename[] = "test02.txt";

    // Initialize chosen serial port
    stdio_init_all();
    sleep_ms(1000);
    printf("Here\n");
    sleep_ms(1000);
    printf("Here2\n");
    sleep_ms(1000);
    printf("Here3\n");

    // Wait for user to press 'enter' to continue
    printf("\nSD card test. Press 'enter' to start.\n");
    // while (true) {
    //     buf[0] = getchar();
    //     if (buf[0] == '\n') {
    //         break;
    //     }
    // }

    // Initialize SD card
    if (!sd_init_driver()) {
        printf("ERROR: Could not initialize SD card\n");
        while (true);
    }

    // Mount drive
    fr = f_mount(&fs, "0:", 1);
    if (fr != FR_OK) {
        printf("ERROR: Could not mount filesystem (%d)\n", fr);
        while (true);
    }

    // Open file for writing ()
    fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\n", fr);
        while (true);
    }

    // Write something to file
    ret = f_printf(&fil, "This is another test\n");
    if (ret < 0) {
        printf("ERROR: Could not write to file (%d)\n", ret);
        f_close(&fil);
        while (true);
    }
    ret = f_printf(&fil, "of writing to an SD card.\n");
    if (ret < 0) {
        printf("ERROR: Could not write to file (%d)\n", ret);
        f_close(&fil);
        while (true);
    }

    // Close file
    fr = f_close(&fil);
    if (fr != FR_OK) {
        printf("ERROR: Could not close file (%d)\n", fr);
        while (true);
    }

    // Open file for reading
    fr = f_open(&fil, filename, FA_READ);
    if (fr != FR_OK) {
        printf("ERROR: Could not open file (%d)\n", fr);
        while (true);
    }

    // Print every line in file over serial
    printf("Reading from file '%s':\n", filename);
    printf("---\n");
    while (f_gets(buf, sizeof(buf), &fil)) {
        printf(buf);
    }
    printf("\n---\n");

    // Close file
    fr = f_close(&fil);
    if (fr != FR_OK) {
        printf("ERROR: Could not close file (%d)\n", fr);
        while (true);
    }

    // Unmount drive
    f_unmount("0:");

    // Loop forever doing nothing
    while (true) {
        sleep_ms(1000);
    }
}
