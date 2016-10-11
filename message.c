/** @file   message.c
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
    @brief This implements functions to do with sending and receiving
    messages.
*/

#include "ir_uart.h"
#include "message.h"
#include "var.h"
#include "tinygl.h"

/** Construct a message then send a message with ball information. */
void send_ball_msg(void)
{
	// construct message
	uint8_t ball_msg = encrypt_ball(bitmap[0]);
	uint8_t message = direction + ball_msg;
	ir_uart_putc(message);		// send message
	delete_col_0 = true;		// because ball has left screen
	bounce = false;
}

/** Send initial signal to opponent. */
void send_signal(void)
{
	ir_uart_putc(SIGNAL1);
	communicated = SENT_SIGNAL;		
}

/** Tell opponent they won */
void give_medal(void)
{
	ir_uart_putc(SIGNAL_WIN);
}

/** Ready to receive a signal from opponent. */
void receive_opp_signal(void)
{
	uint8_t rec_char = ir_uart_getc();
	// if receive initial signal from opp, add ball to bat
	if (rec_char == SIGNAL1) 
	{
		communicated = GOT_SIGNAL;
		bitmap[3] = 0b0001000;		// give ball
		ir_uart_putc(SIGNAL_BALL);			// tell opp you got ball
	}

	// if receive a ball signal from opp means opponent has ball
	if (rec_char == SIGNAL_BALL) 
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
	
	// check if winning message is received first
	if(rec_msg == SIGNAL_WIN && game_on)
	{
		win = true;
		game_on = false;
	}
	else
	{
		rec_ball = rec_msg & 0b111;	// decrypt ball, get last three bits
		uint8_t rec_direct = rec_msg & 0x18;	// get direction of ball
		
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

}

/** Ready to send messages to opponent. */
void ready_to_send(void)
{
	// check if ball is at the edge of the screen
	if ((start_shot && bitmap[0] != 0)) 
	{
		send_ball_msg();
		start_shot = false; // because ball shoots once in a game
	}
	else if((bounce && bitmap[0] != 0))
	{
		send_ball_msg();
	}
	
}

/** Ready to receive messages from opponent. */
void ready_to_receive(void)
{
	// check if there is something to receive
	if (ir_uart_read_ready_p()) 
	{
		// if game restarted, do not send opponent a signal
		if (communicated == SENT_SIGNAL && restarted == false) 
		{
			receive_opp_signal();
		} 
		else 
		{
			receive_game_msg();
		}
	}
}


