/** @file   getball.c
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
    @brief Function to receive the ball to fly out of screen or not.
*/


#include "pio.h"
#include "var.h"
#include "getball.h"
#include "tinygl.h"
#include "message.h"

/** Check if ball needs to be shown flying out of screen. */
void check_flyout(void)
{
	if (flyout) // if activated, show ball flying out of screen
	{
		// make ball fly out of screen
		bitmap[4] = flybit;
		flyout = false;
		lose = true;
		tinygl_text (LOSER_TEXT);
		give_medal();
	}
}

/** Bounce ball to the left as it hits right of bat. */
void ball_bounce_left(void)
{
	bounce_left_from = 3;
	bounce_left_to = 2;
	bounce_left = true;
	bounce = true;
	reflect_right = false;
	direction = FLY_RIGHT;       // fly to the right
}

/** Bounce ball to the middle. */
void ball_bounce_middle(void)
{                               
	bounce_straight_from = 3;
	bounce_straight_to = 2;
	bounce_straight = true;
	bounce = true;
	direction = 0;
}

/** Bounce ball to the right as it hits left of bat. */
void ball_bounce_right(void)
{
	bounce_right_from = 3;
	bounce_right_to = 2;
	bounce_right = true;
	bounce = true;
	reflect_left = false;
	direction = FLY_LEFT;        // fly to the left
}

/** Check actions after receiving ball. */
void receive_ball(void)
{
	// check for clashing/hitting bat
	if ((bitmap[4] | bitmap[3]) != bitmap[4]) 
	{
		// not on bat, make ball show next to bat
		flybit = bitmap[4];
		bitmap[4] = bitmap[4] | bitmap[3];
		bitmap[3] = 0x00;
		flyout = true;
	} 
	else 
	{
		// get ball ready to bounce back
		// check where ball hits bat
		uint8_t bat = bitmap[4];
		uint8_t ball = bitmap[3];
		if (bat / 7 == ball) 
		{
			ball_bounce_left();
		} else if (2 * (bat / 7) == ball) 
		{
			ball_bounce_middle();

		} else 
		{
			ball_bounce_right();
		}
	}
}

/** Reset variables so board can receive balls later. */
void reset_receiver(void)
{
	receive_go_to = 1;
	receive_prev = 0;
	received = false;
	receiving_left = false;
	receiving_right = false;	
}

/** Check if ball has been received and undergo actions to ball. */
void check_received_ball(void)
{
	// received ball and check if ball hits the bat
	if (received && receive_go_to == 4) 
	{
		receive_ball();
		reset_receiver();
	}
	
}

/** Display the received ball in the correct direction. */
void check_receiving_direction(void)
{
	// received a ball so show ball fly into screen
	if (received && receive_go_to <= 3) 
	{
		// check if ball should travel to the left, right or middle
		// then update the bitmap corresponding to the direction
		if (receiving_left) 
		{
			bitmap[receive_go_to] = bitmap[receive_prev] * 2;
		} else if (receiving_right) 
		{
			bitmap[receive_go_to] = bitmap[receive_prev] / 2;
		} else 
		{
			bitmap[receive_go_to] = bitmap[receive_prev];
		}
		bitmap[receive_prev] = 0x00;
		receive_prev++;
		receive_go_to++;
	}
}
