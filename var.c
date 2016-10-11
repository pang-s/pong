/** @file   var.c
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
    @brief Store variables commonly used in the game.
*/


#include "pio.h"
#include "var.h"

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
uint8_t current_column = 0;
uint8_t rec_ball;

// variables used to keep track of the ball
uint8_t direction = 0;
int prev_column = 0;
int prev_shot_row = 3;
int new_shot_row = 2;
int receive_go_to = 1;
int receive_prev = 0;
uint8_t flybit;
bool flyout = false;

// declaring variables to know where the ball should bounce
int bounce_straight_to;
int bounce_straight_from;
int bounce_left_to;
int bounce_left_from;
int bounce_right_to;
int bounce_right_from;

// variables to know if the ball should bounce to a certain direction
bool bounce_straight = false;
bool bounce_left = false;
bool bounce_right = false;

// variables to know if the ball should reflect to a certain direction
bool reflect_right = false;
bool reflect_left = false;
bool receiving_left = false;
bool receiving_right = false;

// initial game statuses
int communicated = NO_CONTACT;
bool ball_shot = false;
bool received = false;
bool opp_start = false;
bool bounce = false;
bool delete_col_0 = false;
bool start_shot = false;
bool lose = false;
bool win = false;
bool game_on = true;
bool restarted = false;

/** Define PIO pins driving LED matrix rows. */
const pio_t rows[] = 
{
    LEDMAT_ROW1_PIO, LEDMAT_ROW2_PIO, LEDMAT_ROW3_PIO,
    LEDMAT_ROW4_PIO, LEDMAT_ROW5_PIO, LEDMAT_ROW6_PIO,
    LEDMAT_ROW7_PIO
};


/** Define PIO pins driving LED matrix columns. */
const pio_t cols[] = 
{
    LEDMAT_COL1_PIO, LEDMAT_COL2_PIO, LEDMAT_COL3_PIO,
    LEDMAT_COL4_PIO, LEDMAT_COL5_PIO
};


/** Initially place bat at bottom middle of screen. */
uint8_t bitmap[] = 
{
    0x00, 0x00, 0x00, 0x00, 0b0011100
};


