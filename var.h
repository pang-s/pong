/** @file   var.h
    @author Pang Suwanaposee psu43 and Alex Lie lli134
    @date   7 Oct 2016
    @brief Store variables commonly used in the game.
*/

#ifndef VAR_H
#define VAR_H

/* Define states of communication. */
#define NO_CONTACT 0
#define SENT_SIGNAL 1
#define GOT_SIGNAL 2

/* Define the direction of ball (integrated in a message). */
#define FLY_LEFT 0b1000
#define FLY_RIGHT 0b10000

/* Define text that will scroll across screen. */
#define WINNER_TEXT "WINNER :)"
#define LOSER_TEXT "LOSER :("

/* Define signals that can be sent/received. */
#define SIGNAL1 'S'
#define SIGNAL_BALL 'B'
#define SIGNAL_WIN 'W'

// variables used when updating the game board
uint8_t current_column;
uint8_t rec_ball;

// variables used to keep track of the ball
uint8_t direction;
int prev_column;
int prev_shot_row;
int new_shot_row;
int receive_go_to;
int receive_prev;
uint8_t flybit;
bool flyout;

// declaring variables to know where the ball should bounce
int bounce_straight_to;
int bounce_straight_from;
int bounce_left_to;
int bounce_left_from;
int bounce_right_to;
int bounce_right_from;

// variables to know if the ball should bounce to a certain direction
bool bounce_straight;
bool bounce_left;
bool bounce_right;

// variables to know if the ball should reflect to a certain direction
bool reflect_right;
bool reflect_left;
bool receiving_left;
bool receiving_right;

// initial game statuses
int communicated;
bool ball_shot;
bool received;
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
