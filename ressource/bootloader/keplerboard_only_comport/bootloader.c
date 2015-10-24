/******************************************************************************
* bootloader.c                                                                *
* ============                                                                *
*                                                                             *
* Version: 1.0.2                                                              *
* Date   : 23.10.15                                                           *
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

//**************************[bootloader_start]*********************************
uint8_t bootloader_start(void) {

    // check boot pin
    DDRG  = 0x00;
    PORTG = _BV(2);
    delay_ms(50);
    if (PING & _BV(2)) {
        PORTG = 0x00;
        return 0x00;
    }
    PORTG = 0x00;

    uart0_send('~');

    string_from_const(display_print, "Bootloader  V4.0");
    display_gotoxy(0,1);
    string_from_const(display_print, "P.W.  23.10.2015");

    return 0xFF;
}

//**************************[bootloader_end]***********************************
void bootloader_end(void) {

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
