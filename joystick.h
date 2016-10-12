/** @file   joystick.h
    @author Pang Suwanaposee psu43 and Alex Lie lli134
    @date   7 Oct 2016
    @brief Function to take user controls.
*/

#ifndef JOYSTICK_H
#define JOYSTICK_H

/** Check if player clicked to go left. */
void check_button_left(void);

/** Check if player clicked to go right. */
void check_button_right(void);

/** Check button has been pushed to shoot ball. */
void check_button_push(void);

/** Reset variables to reset game. */
void reset_game(void);

/** Check button has been pushed to restart game. */
void check_button_restart(void);

/** Check player controls. */
void check_control(void);

/** Player shot ball so show ball path across screen. */
void ball_shot_path(void);

#endif
