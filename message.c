/** @file   message.c
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
*/


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
void send_ball_msg(void)
{
	uint8_t ball_msg = encrypt_ball(bitmap[0]);
	uint8_t message = direction + ball_msg;
	ir_uart_putc(message);  // send message
	delete_col_0 = true;    // because ball has left screen
	ball_shot = false;
	bounce = false;
}

/** Send signal to opponent. */
void send_signal(void)
{
	ir_uart_putc('S'); // send signal
	communicated = SENT_SIGNAL;       // sent signal
}
