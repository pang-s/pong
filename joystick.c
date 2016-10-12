/** @file   joystick.c
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
    @brief Function to take user controls.
*/

#include "navswitch.h"
#include "pio.h"
#include "tinygl.h"
#include "button.h"
#include "var.h"
#include "joystick.h"

/** Check if player clicked to go left. */
void check_button_left(void)
{
	if (navswitch_push_event_p(NAVSWITCH_SOUTH) && bitmap[4] < 112) 
	{
		bitmap[4] = bitmap[4] * 2;  // move the bat to the left
		// if ball hasnt been shot yet, move the ball with the bat
		if (ball_shot == false) 
		{
			bitmap[3] = bitmap[3] * 2; // move ball with bat.
		}
	}	
}

/** Check if player clicked to go right. */
void check_button_right(void)
{
	if (navswitch_push_event_p(NAVSWITCH_NORTH) && bitmap[4] > 7) 
	{
		bitmap[4] = bitmap[4] / 2;  // move the bat to the right
		// if ball hasnt been shot yet, move the ball with the bat
		if (ball_shot == false) 
		{
			bitmap[3] = bitmap[3] / 2; // move ball with bat.
		}
	}	
}

/** Check button has been pushed to shoot ball. */
void check_button_push(void)
{
	// Check ball hasn't been shot and opponent do not start with ball.
	if (opp_start == false && ball_shot == false
		&& navswitch_push_event_p(NAVSWITCH_PUSH)) 
	{
		start_shot = true;   // shoot ball
		ball_shot = true;	// can only shoot once in a game
	}
}

/** Reset variables to reset game. */
void reset_game(void)
{
	// reset text so that it won't always say you lose
	tinygl_text (WINNER_TEXT); 
	// reset variables so ball is updated properly
	prev_shot_row = 3;
	new_shot_row = 2;
	prev_column = 0;
	direction = 0;
	// reset game status
	ball_shot = false;
	delete_col_0 = false;
	lose = false;
	win = false;
	game_on = true;
	restarted = true;
	// reset screen with bat in middle
	bitmap[0] = 0x00;
	bitmap[1] = 0x00;
	bitmap[2] = 0x00;
	bitmap[4] = 0b0011100; // this is the bat at the middle
}

/** Check button has been pushed to restart game. */
void check_button_restart(void)
{
	// Check game finished.
	if ((win || lose) && button_push_event_p (BUTTON1)) 
	{
		if(win)
		{
			//start with the ball
			bitmap[3] = 0b0001000; // place ball at middle
			opp_start = false;
		}
		else
		{	
			opp_start = true;
		}
		reset_game();
		
	}
}

/** Check player controls. */
void check_control(void)
{
	check_button_left();
	check_button_right();
	check_button_push();
	check_button_restart();
}

/** Player shot ball so show ball path across screen. */
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
