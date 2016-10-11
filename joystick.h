/** @file   joystick.h
    @author Pang Suwanaposee and Alex Lie
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

/** Check button has been pushed to restart game. */
void check_button_restart(void);

/** Check player controls. */
void check_control(void);

/** If ball shot, show ball path across screen. */
void ball_shot_path(void);

#endif
