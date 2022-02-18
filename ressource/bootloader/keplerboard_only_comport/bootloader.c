/******************************************************************************
* bootloader.c                                                                *
* ============                                                                *
*                                                                             *
* Version: 1.1.0                                                              *
* Date   : 25.10.15                                                           *
* Author : Peter Weissig                                                      *
*                                                                             *
* For help or bug report please visit:                                        *
*   https://github.com/peterweissig/robolib                                   *
******************************************************************************/

// include from gcc
#include <inttypes.h>
#include <avr/io.h>

// include all necessary headers from system
#include "bootloader_header.h"

#define GET_BOOT_PIN()     (PING & _BV(2))
#define ENABLE_BOOT_PIN()  do {DDRG  = 0x00; PORTG = _BV(2); } while (0)
#define DISABLE_BOOT_PIN() do {DDRG  = 0x00; PORTG =   0x00; } while (0)

//**************************[bootloader_start]*********************************
uint8_t bootloader_start(void) {

    // check boot pin
    ENABLE_BOOT_PIN();

    delay_ms(50);
    if (GET_BOOT_PIN()) {
        return 0x00;
    }

    // print information
    bootloader_data_out('~');

    string_from_const(display_print, "Bootloader  V4.0");
    display_gotoxy(0,1);
    string_from_const(display_print, "RS232   25.10.15");

    return 0xFF;
}

//**************************[bootloader_end]***********************************
void bootloader_end(void) {

    DISABLE_BOOT_PIN();

    display_clear();

    uart0_flush();
    delay_ms(10);
    uart0_disable();
}

//**************************[bootloader_data_in]*******************************
uint8_t bootloader_data_in(void) {

    return uart0_get();
}

//**************************[bootloader_data_stat]*****************************
uint8_t bootloader_data_stat(void) {

    return uart0_rxcount_get();
}

//**************************[bootloader_data_out]******************************
void bootloader_data_out(uint8_t data) {

    uart0_send(data);
}

//**************************[bootloader_idle]**********************************
uint8_t bootloader_idle(void) {

    return 0xFF;
}

//**************************[bootloader_command]*******************************
void bootloader_command(void) {
}

//**************************[bootloader_help]**********************************
void bootloader_help(void) {
}

//**************************[bootloader_error]*********************************
uint8_t bootloader_error(void) {

    return 0xFF;
}
