/** @file   joystick.c
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
    @brief Function to take user controls.
*/

#include "navswitch.h"
#include "pio.h"
#include "var.h"
#include "joystick.h"

/** Check player controls. */
void check_control(void)
{
	// Check if player wants to go left
	if (navswitch_push_event_p(NAVSWITCH_SOUTH) && bitmap[4] < 112) 
	{
		bitmap[4] = bitmap[4] * 2;  // move the bat to the left
		// if ball hasnt been shot yet, move the ball with the bat
		if (ball_shot == false) 
		{
			bitmap[3] = bitmap[3] * 2; // move ball with bat.
		}
	}
	
	// Check if player wants to go right
	if (navswitch_push_event_p(NAVSWITCH_NORTH) && bitmap[4] > 7) 
	{
		bitmap[4] = bitmap[4] / 2;  // move the bat to the right
		// if ball hasnt been shot yet, move the ball with the bat
		if (ball_shot == false) 
		{
			bitmap[3] = bitmap[3] / 2; // move ball with bat.
		}
	}
	
	// Check button has been pushed and ball hasn't been shot yet 
	// and opponent do not have ball initially.
	if (opp_start == false && ball_shot == false
		&& navswitch_push_event_p(NAVSWITCH_PUSH)) 
	{
		start_shot = true;   // shoot ball
		ball_shot = true;	// can only shoot once in a game
	}
}

/** If ball shot, show ball path across screen. */
void ball_shot_path(void)
{
	// check if ball shot and ball is not at column less than 0
	if (ball_shot && new_shot_row >= 0) 
	{
		bitmap[new_shot_row] = bitmap[prev_shot_row];
		bitmap[prev_shot_row] = 0x00;
		prev_shot_row--;
		new_shot_row--;
	}
	
	// activated when ball is at col 0
	if (delete_col_0) 
	{
		bitmap[0] = 0x00; // delete ball from col 0
		delete_col_0 = false;
	}
}
