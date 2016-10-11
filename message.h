/** @file   message.h
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
*/

#ifndef MESSAGE_H
#define MESSAGE_H

//~ #include "system.h"
//~ #include "ir_uart.h"

#include "system.h"
#include "button.h"
#include "task.h"
#include "navswitch.h"
#include "pio.h"
#include "pacer.h"
#include "led.h"
#include "ir_uart.h"
#include "message.h"
#include "var.h"


/** Construct a message then send a message with ball information. */
void send_ball_msg(void);

/** Send signal to opponent. */
void send_signal(void);

#endif
