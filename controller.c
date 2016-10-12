/** @file   controller.c
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
*/


#include "system.h"
/*
#include "button.h"
*/
#include "navswitch.h"
#include "controller.h"



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
		ball_shot = true;   // shoot ball
	}
}


/*

void check_conditions(void)
{
	check_control(); // check controls from user
	ball_shot_path(); // check if need to show ball shot path
	check_flyout(); // check if need to show ball fly off screen
	check_received_ball(); // check if ball is received
	check_receiving_direction(); // check direction of received ball
}
*/
