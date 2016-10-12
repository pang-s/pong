/** @file   hitball.c
    @author Pang Suwanaposee psu43 33330630 and Alex Lie lli134 66969353
    @date   7 Oct 2016
    @brief Functions to hit/bounce the ball back if it hits the bat.
*/

#include "pio.h"
#include "var.h"
#include "hitball.h"

/** Bounce ball straight back. */
void bounce_straight_back(void)
{
	// update ball to show in next column
	bitmap[bounce_straight_to] = bitmap[bounce_straight_from];
	bitmap[bounce_straight_from] = 0x00; // clear ball from prev col
	bounce_straight_from--;
	bounce_straight_to--;
}

/** Bounce ball back to the left. */
void bounce_back_left(void)
{
	// check if ball is reflected 
	if (bitmap[bounce_left_from] != 64 && reflect_right == false) 
	{
		// update ball to show in next column
		bitmap[bounce_left_to] = 2 * bitmap[bounce_left_from];
	} 
	else 
	{
		// ball reflected, update ball to show in next column
		bitmap[bounce_left_to] = bitmap[bounce_left_from] / 2;
		reflect_right = true;
		direction = FLY_RIGHT;   // fly to the right
	}
	bitmap[bounce_left_from] = 0x00; // clear ball from prev col
	bounce_left_from--;
	bounce_left_to--;
}

/** Bounce ball back to the right. */
void bounce_back_right(void)
{
	// check if ball is reflected 
	if (bitmap[bounce_right_from] != 1 && reflect_left == false) 
	{
		// update ball to show in next column
		bitmap[bounce_right_to] = bitmap[bounce_right_from] / 2;
	} 
	else 
	{
		// ball reflected, update ball to show in next column
		bitmap[bounce_right_to] = bitmap[bounce_right_from] * 2;
		reflect_left = true;
		direction = FLY_LEFT;    // fly to the left
	}
	bitmap[bounce_right_from] = 0x00; // clear ball from prev col
	bounce_right_from--;
	bounce_right_to--;
}

/** Check which way ball should bounce and call corresponding
 * functions that will bounce the ball back. */
void check_bounce_back(void)
{
	// bounce straight back
	if (bounce_straight && bounce_straight_to >= 0) 
	{
		bounce_straight_back();
	}

	// bounce to the left
	if (bounce_left && bounce_left_to >= 0) 
	{
		bounce_back_left();
	}

	// bounce to the right
	if (bounce_right && bounce_right_to >= 0) 
	{
		bounce_back_right();
	}
}
