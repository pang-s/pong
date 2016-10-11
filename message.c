/** @file   message.c
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
    @brief This implements functions to do with sending and receiving
    messages.
*/

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

/** Ready to receive a signal from opponent. */
void receive_opp_signal(void)
{
	uint8_t rec_char = ir_uart_getc();
	// if receive 'S' from opp, add ball to bat
	if (rec_char == 'S') 
	{
		communicated = GOT_SIGNAL;	// made contact with opp
		bitmap[3] = 0b0001000;		// give ball
		ir_uart_putc('B');		// tell opp you got ball
	}

	// if receive 'B' from opp, opponent has ball
	if (rec_char == 'B') 
	{
		// opponent will start with ball
		opp_start = true;
		communicated = GOT_SIGNAL;
	}
}

/** Ready to receive a game message from opponent. */
void receive_game_msg(void)
{
	// should receive a message
	uint8_t rec_msg = ir_uart_getc();
	// so decrypt ball, get last three bits
	rec_ball = rec_msg & 0b111;

	// get direction of ball
	uint8_t rec_direct = rec_msg & 0x18;
	if (rec_direct == FLY_RIGHT) 
	{
		// receive ball flying left
		received = true;
		receiving_left = true;

	} 
	else if (rec_direct == FLY_LEFT) 
	{
		// receive ball flying right
		received = true;
		receiving_right = true;
	}

	// check that the receieved ball shows one dot
	if (rec_ball >= 0 && rec_ball <= 6) 
	{
		// show received ball on screen
		bitmap[0] = reverse(decrypt_ball(rec_ball));
		received = true;
	}
}

