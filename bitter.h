/** @file   message.h
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
*/

#ifndef BITTER_H
#define BITTER_H



#include <avr/io.h>
#include "system.h"
#include "button.h"
#include "task.h"
#include "navswitch.h"
#include "pio.h"
#include "pacer.h"
#include "led.h"
#include "ir_uart.h"
#include "var.h"
#include "bitter.h"

/** Given bits n, return the reversed version of the bits. */
uint8_t reverse(uint8_t n);

/** Encrypt ball bitmap into condensed form 
 * eg. takes 1000000 and turns into 110 */
uint8_t encrypt_ball(uint8_t bits);

/** Decrypt ball bitmap into true form
 * eg takes 110 turns into 1000000 */
uint8_t decrypt_ball(uint8_t bits);


#endif
