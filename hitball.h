/** @file   hitball.h
    @author Pang Suwanaposee psu43 and Alex Lie lli134
    @date   7 Oct 2016
    @brief Functions to hit/bounce the ball back if it hits the bat.
*/

#ifndef HITBALL_H
#define HITBALL_H

/** Bounce ball straight back. */
void bounce_straight_back(void);

/** Bounce ball back to the left. */
void bounce_back_left(void);

/** Bounce ball back to the right. */
void bounce_back_right(void);

/** Check which way ball should bounce and call corresponding
 * functions that will bounce the ball back. */
void check_bounce_back(void);

#endif
