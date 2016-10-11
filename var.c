/** @file   var.c
    @author Pang Suwanaposee and Alex Lie
    @date   7 Oct 2016
    @brief Store variables commonly used in the game.
*/


#include "pio.h"
#include "var.h"

#define NO_CONTACT 0
#define SENT_SIGNAL 1
#define GOT_SIGNAL 2
#define FLY_LEFT 0b1000
#define FLY_RIGHT 0b10000


// used in board
uint8_t current_column = 0;
uint8_t rec_ball;


// used in button
int prev_shot_row = 3;
int new_shot_row = 2;
uint8_t flybit;
bool flyout = false;
int bounce_straight_to;
int bounce_straight_from;
int bounce_left_to;
int bounce_left_from;
int bounce_right_to;
int bounce_right_from;
int receive_go_to = 1;
int receive_prev = 0;
bool reflect_right = false;
bool reflect_left = false;
bool bounce_straight = false;
bool bounce_left = false;
bool bounce_right = false;
bool receiving_left = false;
bool receiving_right = false;


// used in both
int prev_column = 0;
uint8_t direction = 0;
bool ball_shot = false;
bool received = false;
int communicated = 0;
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

/** Lookup table needed for reversing bits. */
unsigned char lookup[16] = 
{
    0x0, 0x8, 0x4, 0xc, 0x2, 0xa, 0x6, 0xe,
    0x1, 0x9, 0x5, 0xd, 0x3, 0xb, 0x7, 0xf
};
