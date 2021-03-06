/** @file   getball.h
    @author Pang Suwanaposee psu43 33330630 and Alex Lie lli134 66969353
    @date   7 Oct 2016
    @brief Function to receive the ball onto the bat or fly out.
*/

#ifndef GETBALL_H
#define GETBALL_H

/** Check if ball needs to be shown flying out of screen. */
void check_flyout(void);

/** Bounce ball to the left as it hits right of bat. */
void ball_bounce_left(void);

/** Bounce ball to the middle. */
void ball_bounce_middle(void);

/** Bounce ball to the right as it hits left of bat. */
void ball_bounce_right(void);

/** Check actions after receiving ball. */
void receive_ball(void);

/** Reset variables so board can receive balls later. */
void reset_receiver(void);

/** Check if ball has been received and undergo actions to ball. */
void check_received_ball(void);

/** Display the received ball in the correct direction. */
void check_receiving_direction(void);

#endif
