/** @file   var.h
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
    @brief Store variables commonly used in the game.
*/

#ifndef VAR_H
#define VAR_H

#define NO_CONTACT 0
#define SENT_SIGNAL 1
#define GOT_SIGNAL 2
#define FLY_LEFT 0b1000
#define FLY_RIGHT 0b10000
#define WINNER_TEXT "WINNER :)"
#define LOSER_TEXT "LOSER :("

// used in board
uint8_t current_column;
uint8_t rec_ball;


// used in button
int prev_shot_row;
int new_shot_row;
uint8_t flybit;
bool flyout;
int bounce_straight_to;
int bounce_straight_from;
int bounce_left_to;
int bounce_left_from;
int bounce_right_to;
int bounce_right_from;
int receive_go_to;
int receive_prev;
bool reflect_right;
bool reflect_left;
bool bounce_straight;
bool bounce_left;
bool bounce_right;
bool receiving_left;
bool receiving_right;


// used in both
int prev_column;
uint8_t direction;
bool ball_shot;
bool received;
int communicated;
bool opp_start;
bool bounce;
bool delete_col_0;
bool start_shot;

bool lose;
bool win;
bool game_on;
bool restarted;

/** Define PIO pins driving LED matrix rows. */
const pio_t rows[7];


/** Define PIO pins driving LED matrix columns. */
const pio_t cols[5];


/** Initially place bat at bottom middle of screen. */
uint8_t bitmap[5];

/** Lookup table needed for reversing bits. */
unsigned char lookup[16];

#endif
